#include "WiFiConfig.h"

Preferences WiFiConfig::preferences;

bool WiFiConfig::connect() {
    WiFiClass::mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    WiFiManager wm;

    wm.resetSettings();
    wm.setConnectTimeout(60);
    bool res = wm.autoConnect("AutoConnectAP","12345678");

    if (res) {
        preferences.begin("wifi", false);
        preferences.putString("ssid", WiFi.SSID());
        preferences.putString("password", WiFi.psk());
        preferences.end();
    }

    return res;
}

void WiFiConfig::disconnect() {
    WiFiClass::mode(WIFI_OFF);
}

void WiFiConfig::reconnect() {
    preferences.begin("wifi", true);
    String ssid = preferences.getString("ssid", "");
    String password = preferences.getString("password", "");
    preferences.end();

    if (ssid != "" && password != "") {
        WiFi.begin(ssid.c_str(), password.c_str());

        while (WiFiClass::status() != WL_CONNECTED) {
            delay(1000);
            return;
        }
    }
}