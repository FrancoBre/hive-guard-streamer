#include "WiFiConfig.h"

const char* ssid = "Speedy-Fibra-C19C2E";
const char* password = "qazwsxed";

bool WiFiConfig::connect() {
    WiFi.begin(ssid, password);

    while (WiFiClass::status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }

    if (WiFiClass::status() == WL_CONNECTED) {
        Serial.println("Connected to WiFi");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        return false;
    }
}