#include "LogsHandler.h"

std::vector<std::string> LogsHandler::logs;

esp_err_t LogsHandler::handle(httpd_req_t *req) {
    std::string allLogs = getLogs();
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, allLogs.c_str(), allLogs.length());
    return ESP_OK;
}

void LogsHandler::addLog(const std::string &log) {
    const size_t MAX_LOGS = 1000;
    if (logs.size() >= MAX_LOGS) {
        logs.erase(logs.begin());
    }
    logs.push_back(log);
}

std::string LogsHandler::getLogs() {
    std::string allLogs;
    for (const auto &log: logs) {
        allLogs += log + "<br>";
    }
    return allLogs;
}
