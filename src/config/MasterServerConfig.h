#ifndef MASTERSERVERCONFIG_H
#define MASTERSERVERCONFIG_H

#include <WiFiClient.h>
#include <cstring>
#include "utils/CustomSerial.h"
#include <Preferences.h>

class MasterServerConfig {
public:
    static String getMasterIp(String jsonData);
    static String masterServerIp;

private:
    static bool askIpIfItsMaster(WiFiClient &client, const String &ip, const String &jsonData);
};


#endif //MASTERSERVERCONFIG_H
