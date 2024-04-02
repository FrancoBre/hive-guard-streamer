#include "WiFiConfig.h"
#include <WiFiManager.h>

bool WiFiConfig::connect() {
    WiFiClass::mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    WiFiManager wm;

    wm.resetSettings();
    wm.setConnectTimeout(60);
    return wm.autoConnect("AutoConnectAP","12345678");
}