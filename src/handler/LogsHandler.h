#ifndef LOGSHANDLER_H
#define LOGSHANDLER_H

#include <vector>
#include <string>
#include "esp_http_server.h"

class LogsHandler {
public:
    static esp_err_t handle(httpd_req_t *req);
    static void addLog(const std::string& log);

private:
    static std::vector<std::string> logs;
    static std::string getLogs();
};

#endif // LOGSHANDLER_H