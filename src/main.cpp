#include <WiFi.h>
#include "Arduino.h"
#include "config/CameraConfig.h"
#include "config/WiFiConfig.h"
#include "config/TimeConfig.h"
#include "handler/VideoStreamHandler.h"
#include "handler/StatusHandler.h"
#include "config/HttpServerConfig.h"
#include "handler/Endpoint.h"
#include "handler/LogsHandler.h"
#include "handler/TemperatureHumiditySensorHandler.h"
#include "utils/CustomSerial.h"
#include "config/BatteryConfig.h"
#include "config/MasterServerConfig.h"
#include "ESPAsyncWebServer.h"

void initializeHttpServer();

AsyncWebServer server(80);

void setup() {
    delay(5000);
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    Logger.print(__FILE__, __LINE__, "Starting ESP32 Video Stream setup");

    if (!CameraConfig::setup()) {
        Logger.print(__FILE__, __LINE__, "Camera setup failed");
        return;
    } else {
        Logger.print(__FILE__, __LINE__, "Camera setup successful");
    }

    pinMode(13, INPUT);
    Logger.print(__FILE__, __LINE__, "GPIO 13 set as input");

    if (!WiFiConfig::connect()) {
        Logger.print(__FILE__, __LINE__, "WiFi connection failed");
        return;
    } else {
        Logger.print(__FILE__, __LINE__, "WiFi connected");
    }

    TimeConfig::initialize();

    BatteryConfig::initialize();

    initializeHttpServer();

    String masterIp = MasterServerConfig::getMasterIp();
    // TODO configure websocket connection to master server with masterIp
}

void loop() {
    delay(100000);

    if (WiFiClass::status() != WL_CONNECTED) {
        Logger.print(__FILE__, __LINE__, "WiFi disconnected");
        return;
    }

    // Battery check doesn't work when WiFi is connected
    WiFiConfig::disconnect();
    BatteryConfig::checkBatteryLevel();
    WiFiConfig::reconnect();
}

void initializeHttpServer() {
    server.on(
            "/iAmMaster",
            HTTP_POST,
            [](AsyncWebServerRequest *request) {},
            NULL,
            [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {

                for (size_t i = 0; i < len; i++) {
                    Serial.write(data[i]);
                }

                Serial.println();
                MasterServerConfig::masterServerIp = request->client()->remoteIP().toString();
                Logger.print(__FILE__, __LINE__, "Master server IP saved successfully");
                Logger.print(__FILE__, __LINE__, "Ready for websocket connection");

                request->send(200);
            });
    Logger.print(__FILE__, __LINE__, "Am I master? endpoint created! Go to http://", WiFi.localIP().toString().c_str(),
                 "/iAmMaster\n");

    server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/plain", "Server is running");
    });
    Logger.print(__FILE__, __LINE__, "Status endpoint created! Go to http://", WiFi.localIP().toString().c_str(),
                 "/status\n");

    server.on("/logs", HTTP_GET, [](AsyncWebServerRequest *request) {
        String allLogs = LogsHandler::getLogs();
        request->send(200, "text/html", allLogs);
    });
    Logger.print(__FILE__, __LINE__, "Logs endpoint created! Go to http://", WiFi.localIP().toString().c_str(),
                 "/logs\n");

    server.begin();

    Logger.print(__FILE__, __LINE__, "HTTP server started successfully!");
}