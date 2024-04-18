#ifndef DHTHANDLER_H
#define DHTHANDLER_H

#include <string>
#include "esp_http_server.h"
#include "DHT.h"

class DHTHandler {
public:
    static esp_err_t handle(httpd_req_t *req);
    void begin();

private:
    static std::string getWebPage();
    static float readDHTTemperature();
    static float readDHTHumidity();
};

#endif // DHTHANDLER_H
