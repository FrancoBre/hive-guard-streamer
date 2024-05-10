#include "MasterServerConfig.h"

Preferences preferences;

String MasterServerConfig::masterServerIp = preferences.getString("masterIp", "");

String MasterServerConfig::getMasterIp() {
    if (!masterServerIp.isEmpty()) {
        return masterServerIp;
    }

    Logger.print(__FILE__, __LINE__, "Searching for master server");

    WiFiClient client;
    char ip[16];
//    for (int thirdOctet = 0; thirdOctet <= 1; thirdOctet++) {
//        for (int fourthOctet = 1; fourthOctet <= 254; fourthOctet++) {
    for (int thirdOctet = 1; thirdOctet <= 1; thirdOctet++) {
        for (int fourthOctet = 50; fourthOctet <= 70; fourthOctet++) {
            snprintf(ip, sizeof(ip), "192.168.%d.%d", thirdOctet, fourthOctet);

            Logger.print(__FILE__, __LINE__, "Checking if master server is IP: ", ip);
            askIpIfItsMaster(client, ip);

            if (!MasterServerConfig::masterServerIp.isEmpty()) {
                Logger.print(__FILE__, __LINE__, "Master server found at IP: ",
                             MasterServerConfig::masterServerIp.c_str());
                preferences.putString("masterIp", masterServerIp);
                return masterServerIp;
            }
        }
    }
    return {};
}

bool MasterServerConfig::askIpIfItsMaster(WiFiClient &client, const String &ip) {
    if (client.connect(ip.c_str(), 8000)) {
        client.println("GET /isMaster HTTP/1.1");
        client.println("Host: " + ip);
        client.println("Connection: close");
        client.println(); // HTTP requests end with a blank line

        while (client.connected()) {
            if (client.available()) {
                String line = client.readString();
                Logger.print(__FILE__, __LINE__, line);
            }
            client.stop();
        }
    }
    return false;
}
