#ifndef WIFICONFIG_H
#define WIFICONFIG_H

#include <Preferences.h>
#include "WiFi.h"
#include "WiFiManager.h"
#include "utils/CustomSerial.h"

class WiFiConfig {
public:
    static bool connect();

    static void disconnect();

    static void reconnect();

private:
    static Preferences preferences;
};

#endif //WIFICONFIG_H
