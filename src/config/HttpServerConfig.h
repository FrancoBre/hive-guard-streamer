#ifndef HTTPSERVERCONFIG_H
#define HTTPSERVERCONFIG_H

#include "esp_http_server.h"
#include "handler/Endpoint.h"
#include <vector>

class HttpServerConfig {
public:
    HttpServerConfig();
    bool addEndpoint(Endpoint *endpoint);
    bool start();

private:
    httpd_handle_t camera_httpd;
    std::vector<Endpoint*> endpoints;
};

#endif // HTTPSERVERCONFIG_H
