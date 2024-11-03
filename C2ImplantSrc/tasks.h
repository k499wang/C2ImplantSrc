#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Task {
	std::string taskName;
	std::string taskId;   // Unique identifier for the task
	json taskData;
};

