#include "MasterServerConfig.h"

Preferences preferences;

String MasterServerConfig::masterIp = preferences.getString("masterIp", "");

String MasterServerConfig::getMasterIp() {
    if (!masterIp.isEmpty()) {
        return masterIp;
    }

    Logger.print(__FILE__, __LINE__, "Searching for master server");

    WiFiClient client;
    char ip[16];
    for (int thirdOctet = 0; thirdOctet <= 1; thirdOctet++) {
        for (int fourthOctet = 1; fourthOctet <= 254; fourthOctet++) {
            snprintf(ip, sizeof(ip), "192.168.%d.%d", thirdOctet, fourthOctet);

            Logger.print(__FILE__, __LINE__, "Checking if master server is IP: ", ip);
            if (isIpMaster(client, ip)) {
                Logger.print(__FILE__, __LINE__, "Master server found at IP: ", ip);
                masterIp = ip;
                preferences.putString("masterIp", masterIp);
                return masterIp;
            }
        }
    }
    return {};
}

bool MasterServerConfig::isIpMaster(WiFiClient &client, const String &ip) {
    if (client.connect(ip.c_str(), 8000)) {
        client.println("GET /isMaster HTTP/1.1");
        client.println("Host: " + ip);
        client.println("Connection: close");
        client.println(); // HTTP requests end with a blank line

        while (client.connected()) {
            if (client.available()) {
                String line = client.readString();
                Logger.print(__FILE__, __LINE__, line);
                if (line.startsWith("1")) {
                    return true;
                }
            }
            client.stop(); // Close the connection
        }
    }
    return false;
}
