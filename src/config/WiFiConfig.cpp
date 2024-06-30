#include "WiFiConfig.h"

// TODO no pushear!
const char* ssid = "";
const char* password = "";

Preferences WiFiConfig::preferences;

bool WiFiConfig::connect() {
    WiFi.begin(ssid, password);

    while (WiFiClass::status() != WL_CONNECTED) {
        delay(1000);
        Logger.print(__FILE__, __LINE__, "Connecting to WiFi...");
    }

    if (WiFiClass::status() == WL_CONNECTED) {
        Logger.print(__FILE__, __LINE__, "Connected to WiFi");
        Logger.print(__FILE__, __LINE__, "IP address: ", WiFi.localIP());

        preferences.begin("WiFiCreds", false);
        preferences.putString("ssid", ssid);
        preferences.putString("password", password);
        preferences.end();

        Logger.print(__FILE__, __LINE__, "WiFi credentials saved to preferences:", ssid, password);

        return true;
    } else {
        return false;
    }
}


void WiFiConfig::disconnect() {
    WiFi.disconnect();
}

void WiFiConfig::reconnect() {
    preferences.begin("WiFiCreds", false);
    String ssid = preferences.getString("ssid", "");
    String password = preferences.getString("password", "");
    preferences.end();

    WiFi.begin(ssid.c_str(), password.c_str());

    while (WiFiClass::status() != WL_CONNECTED) {
        delay(1000);
        Logger.print(__FILE__, __LINE__, "Reconnecting to WiFi...");
    }

    if (WiFiClass::status() == WL_CONNECTED) {
        Logger.print(__FILE__, __LINE__, "Reconnected to WiFi");
    } else {
        Logger.print(__FILE__, __LINE__, "Failed to reconnect to WiFi");
    }
}
