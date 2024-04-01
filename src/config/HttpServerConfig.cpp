#include "esp_camera.h"
#include "esp_http_server.h"
#include "HttpServerConfig.h"

HttpServerConfig::HttpServerConfig() : camera_httpd(nullptr) {}

bool HttpServerConfig::addEndpoint(Endpoint *endpoint) {
    endpoints.push_back(endpoint);
    return true;
}

bool HttpServerConfig::start() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    esp_err_t res = httpd_start(&this->camera_httpd, &config);
    if (res == ESP_OK) {
        for (auto &endpoint : endpoints) {
            httpd_register_uri_handler(this->camera_httpd, endpoint->getUriHandler());
        }
        return true;
    } else {
        return false;
    }
}
