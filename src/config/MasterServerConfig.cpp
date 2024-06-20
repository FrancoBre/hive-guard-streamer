#include "MasterServerConfig.h"
#include <WiFi.h>
#include <WiFiUdp.h>

const unsigned int udpPort = 12345; // Port for UDP communication
WiFiUDP udp;

String udpBroadcastDiscovery(const String &jsonData);

Preferences preferences;

String MasterServerConfig::masterServerIp = preferences.getString("masterIp", "");

String MasterServerConfig::getMasterIp(String jsonData) {
    Logger.print(__FILE__, __LINE__, "Starting getMasterIp with JSON data: ", jsonData.c_str());

    udp.begin(udpPort);
    Logger.print(__FILE__, __LINE__, "UDP initialized with port: ", String(udpPort).c_str());

    String masterIp = udpBroadcastDiscovery(jsonData);
    if (!masterIp.isEmpty()) {
        Logger.print(__FILE__, __LINE__, "Master server IP found: ", masterIp.c_str());
        return masterIp;
    } else {
        Logger.print(__FILE__, __LINE__, "Master server IP not found after UDP broadcast");
    }
    return {};
}

String udpBroadcastDiscovery(const String &jsonData) {
    Logger.print(__FILE__, __LINE__, "Starting udpBroadcastDiscovery with JSON data: ", jsonData.c_str());

    IPAddress broadcastIp = ~WiFi.subnetMask() | WiFi.gatewayIP();
    Logger.print(__FILE__, __LINE__, "Calculated broadcast IP: ", broadcastIp.toString().c_str());

    udp.beginPacket(broadcastIp, udpPort);
    udp.write(reinterpret_cast<const uint8_t *>(jsonData.c_str()), jsonData.length());
    udp.endPacket();
    Logger.print(__FILE__, __LINE__, "Broadcast packet sent");

    uint32_t start = millis();
    const char spinner[] = "|/-";
    int spinnerIndex = 0;
    while (millis() - start < 25000) { // Wait for 25 seconds to get a response
        Logger.print(__FILE__, __LINE__, "Waiting for response... ", spinner[spinnerIndex]);
        spinnerIndex = (spinnerIndex + 1) % sizeof(spinner);

        int packetSize = udp.parsePacket();
        if (packetSize) {
            Logger.print(__FILE__, __LINE__, "Packet received with size: ", String(packetSize).c_str());

            char incomingPacket[255];
            int len = udp.read(incomingPacket, 255);
            if (len > 0) {
                incomingPacket[len] = 0;
            }

            String response = String(incomingPacket);
            Logger.print(__FILE__, __LINE__, "Received response from master server: ", response.c_str());

            // Get the IP address of the server that sent the packet
            IPAddress serverIp = udp.remoteIP();
            String masterIp = serverIp.toString();
            preferences.putString("masterIp", masterIp);
            Logger.print(__FILE__, __LINE__, "Master server IP saved successfully: ", masterIp.c_str());
            return masterIp;
        } else {
            delay(500); // Delay for half a second before checking for the next packet
        }
    }
    Logger.print(__FILE__, __LINE__, "Timeout reached, no response received");
    return "";
    return "";
}