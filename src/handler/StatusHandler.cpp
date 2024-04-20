#include <esp_http_server.h>
#include "StatusHandler.h"

esp_err_t StatusHandler::handle(httpd_req_t *req) {
    const char *resp_str = "I'm alive";
    httpd_resp_send(req, resp_str, strlen(resp_str));
    return ESP_OK;
}
