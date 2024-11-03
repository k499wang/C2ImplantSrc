#pragma once

#define REGISTER_PATH "/api/agents/register-agent"
#define TASK_PATH  "/api/tasks/getTasks"
#define UPDATE_TASK_PATH "/api/tasks/updateTasks"
#define BASE_URL "http://127.0.0.1:2000"
#define ID = "athena"

std::string registerUrl = std::string(BASE_URL) + REGISTER_PATH;
std::string taskUrl = std::string(BASE_URL) + TASK_PATH ;
std::string updateTaskUrl = std::string(BASE_URL) + UPDATE_TASK_PATH;
std::string id = "athena";