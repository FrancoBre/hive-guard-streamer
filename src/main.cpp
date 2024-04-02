#include <WiFi.h>
#include "Arduino.h"
#include "config/CameraConfig.h"
#include "config/WiFiConfig.h"
#include "handler/VideoStreamHandler.h"
#include "handler/StatusHandler.h"
#include "config/HttpServerConfig.h"
#include "handler/Endpoint.h"

void setup() {
    delay(5000);
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    Serial.println("Starting ESP32 Video Stream setup");

    if (!CameraConfig::setup()) {
        Serial.println("Camera setup failed");
        return;
    } else {
        Serial.println("Camera setup successful");
    }

    if (!WiFiConfig::connect()) {
        Serial.println("WiFi connection failed");
        return;
    } else {
        Serial.println("WiFi connected");
    }

    auto* statusEndpoint = new Endpoint("/status", HTTP_GET, StatusHandler::handle);
    Serial.println("Status endpoint created! Go to http://" + WiFi.localIP().toString() + "/status");

    auto* videoStreamEndpoint = new Endpoint("/stream", HTTP_GET, VideoStreamHandler::handle);
    Serial.println("Video stream endpoint created! Go to http://" + WiFi.localIP().toString() + "/stream");

    HttpServerConfig httpServerConfig;
    httpServerConfig.addEndpoint(statusEndpoint);
    httpServerConfig.addEndpoint(videoStreamEndpoint);

    if (!httpServerConfig.start()) {
        Serial.println("Failed to start HTTP server");
        return;
    } else {
        Serial.println("HTTP server started");
    }
}

void loop() {
    delay(10000);

    if (WiFiClass::status() != WL_CONNECTED) {
        Serial.println("WiFi not connected");
        return;
    }
}