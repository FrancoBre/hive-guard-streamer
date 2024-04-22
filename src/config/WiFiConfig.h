#ifndef WIFICONFIG_H
#define WIFICONFIG_H

#include "WiFi.h"
#include "utils/CustomSerial.h"

class WiFiConfig {
public:
    static bool connect();

    static void disconnect();

    static void reconnect();
};

#endif //WIFICONFIG_H
