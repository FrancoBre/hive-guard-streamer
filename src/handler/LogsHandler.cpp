#include <WString.h>
#include "LogsHandler.h"

std::vector<std::string> LogsHandler::logs;

esp_err_t LogsHandler::handle(httpd_req_t *req) {
    String allLogs = getLogs();
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

String LogsHandler::getLogs() {
    String allLogs;
    for (const auto &log: logs) {
        allLogs += String(log.c_str()) + "<br>";
    }
    return allLogs;
}
