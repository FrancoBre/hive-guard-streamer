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
#include "config/WebSocketConfig.h"

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

    if (masterIp.isEmpty()) {
        Logger.print(__FILE__, __LINE__, "Master server IP not found");
        ESP.restart();
    }
    WebSocketConfig::initialize(masterIp);
}

unsigned long lastBatteryCheck = 0;

void loop() {
    if (WiFiClass::status() != WL_CONNECTED) {
        Logger.print(__FILE__, __LINE__, "WiFi disconnected");
        return;
    }

    // Battery check doesn't work when WiFi is connected
//    if (millis() - lastBatteryCheck >= 600000) {
//        WiFiConfig::disconnect();
//        BatteryConfig::checkBatteryLevel();
//        WiFiConfig::reconnect();
//        lastBatteryCheck = millis()
//    }

    client.poll();

    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        esp_camera_fb_return(fb);
        return;
    }

    if (fb-> format != PIXFORMAT_JPEG) { return; }

    client.sendBinary((const char*) fb-> buf, fb-> len);
    esp_camera_fb_return(fb);

    float h = TemperatureHumiditySensorHandler::readDHTHumidity();
    float t = TemperatureHumiditySensorHandler::readDHTTemperature();

    String output = "temp=" + String(t, 2) + ",hum=" + String(h, 2) + ",light=12;state:ON_BOARD_LED_1=" + String(flashlight);
    Logger.print(__FILE__, __LINE__, output);

    client.send(output);
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