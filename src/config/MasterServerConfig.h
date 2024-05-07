#ifndef MASTERSERVERCONFIG_H
#define MASTERSERVERCONFIG_H

#include <WiFiClient.h>
#include <string.h>
#include "utils/CustomSerial.h"
#include <Preferences.h>

class MasterServerConfig {
public:
    static String getMasterIp();

private:
    static String masterIp;
    static bool isIpMaster(WiFiClient &client, const String &ip);
};


#endif //MASTERSERVERCONFIG_H
