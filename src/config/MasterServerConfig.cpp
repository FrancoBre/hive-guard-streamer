#include "MasterServerConfig.h"

Preferences preferences;

String MasterServerConfig::masterServerIp = preferences.getString("masterIp", "");

bool testConnection(WiFiClient &client) {
    const char *serverIp = "httpbin.org";
    Logger.print(__FILE__, __LINE__, "TEST_CONNECTION - Attempting to connect to: ", serverIp);
    if (client.connect(serverIp, 80)) {
        Logger.print(__FILE__, __LINE__, "TEST_CONNECTION - Connected to server");
        client.println("GET /get HTTP/1.1");
        client.println("Host: " + String(serverIp));
        client.println("Connection: close");
        client.println(); // HTTP requests end with a blank line

        while (client.connected()) {
            if (client.available()) {
                String line = client.readString();
                Logger.print(__FILE__, __LINE__, "TEST_CONNECTION - Received from server: ", line.c_str());
            }
        }
        client.stop();
        Logger.print(__FILE__, __LINE__, "TEST_CONNECTION - Connection closed");
        return true;
    }
    Logger.print(__FILE__, __LINE__, "TEST_CONNECTION - Failed to connect to server");
    return false;
}

String MasterServerConfig::getMasterIp(String jsonData) {
    WiFiClient client;
    // Add a mock request to a known IP to test the connection
    Logger.print(__FILE__, __LINE__, "TEST_CONNECTION - Testing connection");
    bool connectionTestResult = testConnection(client);
    Logger.print(__FILE__, __LINE__, "TEST_CONNECTION - Connection test result: ",
                 connectionTestResult ? "Success" : "Failure");

    if (!masterServerIp.isEmpty()) {
        Logger.print(__FILE__, __LINE__, "Master server IP is not empty: ", masterServerIp.c_str());
        return masterServerIp;
    }

    Logger.print(__FILE__, __LINE__, "Searching for master server");
    Logger.print(__FILE__, __LINE__, "Payload: ", jsonData.c_str());

    char ip[16] = "192.168.1.56";
//    for (int thirdOctet = 0; thirdOctet <= 1; thirdOctet++) {
//        for (int fourthOctet = 1; fourthOctet <= 254; fourthOctet++) {
//    for (int thirdOctet = 1; thirdOctet <= 1; thirdOctet++) {
//        for (int fourthOctet = 50; fourthOctet <= 70; fourthOctet++) {
//            snprintf(ip, sizeof(ip), "192.168.%d.%d", thirdOctet, fourthOctet);
    snprintf(ip, sizeof(ip), "192.168.1.56");

    Logger.print(__FILE__, __LINE__, "Checking if master server is IP: ", ip);
    askIpIfItsMaster(client, ip, jsonData);

    if (!MasterServerConfig::masterServerIp.isEmpty()) {
        Logger.print(__FILE__, __LINE__, "Master server found at IP: ",
                     MasterServerConfig::masterServerIp.c_str());
        preferences.putString("masterIp", masterServerIp);
        return masterServerIp;
    }
//        }
//    }
    return {};
}

bool MasterServerConfig::askIpIfItsMaster(WiFiClient &client, const String &ip, const String &jsonData) {
    Logger.print(__FILE__, __LINE__, "ASK_IP_IF_ITS_MASTER - Attempting to connect to: ", ip.c_str());
    if (client.connect(ip.c_str(), 8000)) {
        Logger.print(__FILE__, __LINE__, "ASK_IP_IF_ITS_MASTER - Connected to IP");
        client.println("POST /isMaster HTTP/1.1");
        client.println("Host: " + ip);
        client.println("Content-Type: application/json");
        client.println("Content-Length: " + String(jsonData.length()));
        client.println(); // HTTP requests end with a blank line
        client.println(jsonData);
        Logger.print(__FILE__, __LINE__, "ASK_IP_IF_ITS_MASTER - Sent JSON data: ", jsonData.c_str());

        while (client.connected()) {
            if (client.available()) {
                String line = client.readString();
                Logger.print(__FILE__, __LINE__, "ASK_IP_IF_ITS_MASTER - Received from IP: ", line.c_str());
                if (line.indexOf("Master: Yes") != -1) {
                    MasterServerConfig::masterServerIp = ip;
                    Logger.print(__FILE__, __LINE__, "Master server found at IP: ", ip.c_str());
                    preferences.putString("masterIp", masterServerIp);
                    return true;
                }
            }
        }
        client.stop();
        Logger.print(__FILE__, __LINE__, "ASK_IP_IF_ITS_MASTER - Connection to IP closed");
    } else {
        Logger.print(__FILE__, __LINE__, "ASK_IP_IF_ITS_MASTER - Failed to connect to IP");
    }
    return false;
}