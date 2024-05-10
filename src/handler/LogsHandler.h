#ifndef LOGSHANDLER_H
#define LOGSHANDLER_H

#include <vector>
#include <string>
#include "esp_http_server.h"

class LogsHandler {
public:
    static esp_err_t handle(httpd_req_t *req);
    static void addLog(const std::string& log);
    static String getLogs();

private:
    static std::vector<std::string> logs;
};

#endif // LOGSHANDLER_H