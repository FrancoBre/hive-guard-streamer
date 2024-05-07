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

void initializeHttpServer();

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
    auto *statusEndpoint = new Endpoint("/status", HTTP_GET, StatusHandler::handle);
    Logger.print(__FILE__, __LINE__, "Status endpoint created! Go to http://", WiFi.localIP().toString().c_str(),
                 "/status\n");

    auto *videoStreamEndpoint = new Endpoint("/stream", HTTP_GET, VideoStreamHandler::handle);
    Logger.print(__FILE__, __LINE__, "Video stream endpoint created! Go to http://", WiFi.localIP().toString().c_str(),
                 "/stream\n");

    auto *logsEndpoint = new Endpoint("/logs", HTTP_GET, LogsHandler::handle);
    Logger.print(__FILE__, __LINE__, "Logs endpoint created! Go to http://", WiFi.localIP().toString().c_str(),
                 "/logs\n");

    auto *dhtEndpoint = new Endpoint("/sensor", HTTP_GET, TemperatureHumiditySensorHandler::handle);
    Logger.print(__FILE__, __LINE__, "DHT endpoint created! Go to http://", WiFi.localIP().toString().c_str(),
                 "/sensor\n");

    HttpServerConfig httpServerConfig;
    httpServerConfig.addEndpoint(statusEndpoint);
    httpServerConfig.addEndpoint(videoStreamEndpoint);
    httpServerConfig.addEndpoint(logsEndpoint);
    httpServerConfig.addEndpoint(dhtEndpoint);

    if (!httpServerConfig.start()) {
        Logger.print(__FILE__, __LINE__, "HTTP server failed to start");
        return;
    } else {
        Logger.print(__FILE__, __LINE__, "HTTP server started");
    }
}