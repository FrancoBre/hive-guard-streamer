#ifndef ENDPOINT_H
#define ENDPOINT_H

#include "esp_http_server.h"

class Endpoint {
public:
    Endpoint(const char *uri, http_method method, esp_err_t (*param)(httpd_req_t *)) {
        uri_handler.uri = uri;
        uri_handler.method = method;
        uri_handler.handler = param;
        uri_handler.user_ctx = nullptr;
    }

    httpd_uri_t* getUriHandler() {
        return &uri_handler;
    }

private:
    httpd_uri_t uri_handler{};
};

#endif // ENDPOINT_H