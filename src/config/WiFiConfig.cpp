#include "WiFiConfig.h"
#include "WiFi.h"

const char *ssid = "Speedy-Fibra-C19";
const char *password = "qazwsxed";

bool WiFiConfig::connect() {
    WiFi.begin(ssid, password);

    while (WiFiClass::status() != WL_CONNECTED) {
        delay(1000);
        Logger.print(__FILE__, __LINE__, "Connecting to WiFi...\n");
    }

    if (WiFiClass::status() == WL_CONNECTED) {
        Logger.print(__FILE__, __LINE__, "Connected to WiFi! IP address: ", WiFi.localIP().toString().c_str());
        return true;
    } else {
        return false;
    }
}

void WiFiConfig::disconnect() {
//    WiFi.disconnect();
    WiFiClass::mode(WIFI_OFF);
}

void WiFiConfig::reconnect() {
    WiFi.begin(ssid, password);

    while (WiFiClass::status() != WL_CONNECTED) {
        delay(1000);
        return;
    }
}