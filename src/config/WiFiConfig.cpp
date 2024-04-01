#include "WiFiConfig.h"
#include "WiFi.h"

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

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