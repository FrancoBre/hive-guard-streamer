#ifndef TEMPERATUREHUMIDITYSENSORHANDLER_H
#define TEMPERATUREHUMIDITYSENSORHANDLER_H

#include <string>
#include "esp_http_server.h"
#include "DHT.h"

class TemperatureHumiditySensorHandler {
public:
    static esp_err_t handle(httpd_req_t *req);

    static void begin();

    static float readDHTTemperature();

    static float readDHTHumidity();

private:
    static std::string getWebPage();
};

#endif // TEMPERATUREHUMIDITYSENSORHANDLER_H
