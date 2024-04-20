#include "TemperatureHumiditySensorHandler.h"
#include "utils/CustomSerial.h"

#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

esp_err_t TemperatureHumiditySensorHandler::handle(httpd_req_t *req) {
    std::string webPage = getWebPage();
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, webPage.c_str(), webPage.length());
    return ESP_OK;
}

void TemperatureHumiditySensorHandler::begin() {
    dht.begin();
}

float TemperatureHumiditySensorHandler::readDHTTemperature() {
    float t = dht.readTemperature();
    if (isnan(t)) {
        Logger.print(__FILE__, __LINE__, "Failed to read from DHT sensor!");
        return -1;
    } else {
        Logger.print(__FILE__, __LINE__, "Temperature: ", t);
        return t;
    }
}

float TemperatureHumiditySensorHandler::readDHTHumidity() {
    float h = dht.readHumidity();
    if (isnan(h)) {
        Logger.print(__FILE__, __LINE__, "Failed to read from DHT sensor!");
        return -1;
    } else {
        Logger.print(__FILE__, __LINE__, "Humidity: ", h);
        return h;
    }
}

std::string TemperatureHumiditySensorHandler::getWebPage() {
    char msg[1500];
    float temperature = readDHTTemperature();
    float humidity = readDHTHumidity();
    snprintf(msg, 1500,
             "<html>\
  <head>\
    <meta http-equiv='refresh' content='4'/>\
    <meta name='viewport' content='width=device-width, initial-scale=1'>\
    <link rel='stylesheet' href='https://use.fontawesome.com/releases/v5.7.2/css/all.css' integrity='sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr' crossorigin='anonymous'>\
    <title>ESP32 DHT Server</title>\
    <style>\
    html { font-family: Arial; display: inline-block; margin: 0px auto; text-align: center;}\
    h2 { font-size: 3.0rem; }\
    p { font-size: 3.0rem; }\
    .units { font-size: 1.2rem; }\
    .dht-labels{ font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}\
    </style>\
  </head>\
  <body>\
      <h2>Datos del sensor DHT</h2>\
      <p>\
        <i class='fas fa-thermometer-half' style='color:#ca3517;'></i>\
        <span class='dht-labels'>Temperatura</span>\
        <span>%.2f</span>\
        <sup class='units'>°C</sup>\
      </p>\
      <p>\
        <i class='fas fa-tint' style='color:#00add6;'></i>\
        <span class='dht-labels'>Humedad</span>\
        <span>%.2f</span>\
        <sup class='units'>%</sup>\
      </p>\
  </body>\
</html>",
             temperature, humidity
    );
    return std::string(msg);
}
