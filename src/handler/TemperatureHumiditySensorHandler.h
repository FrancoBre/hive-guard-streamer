#ifndef TEMPERATUREHUMIDITYSENSORHANDLER_H
#define TEMPERATUREHUMIDITYSENSORHANDLER_H

#include <string>
#include "esp_http_server.h"
#include "DHT.h"

class TemperatureHumiditySensorHandler {
public:
    static esp_err_t handle(httpd_req_t *req);

    static void begin();

private:
    static std::string getWebPage();

    static float readDHTTemperature();

    static float readDHTHumidity();
};

#endif // TEMPERATUREHUMIDITYSENSORHANDLER_H
