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

void loop() {
  webSocket.loop();
  uint64_t now = millis();

  if(now - messageTimestamp > 30) {
    messageTimestamp = now;

    camera_fb_t * fb = NULL;

    // Take Picture with Camera
    fb = esp_camera_fb_get();
    if(!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    webSocket.sendBIN(fb->buf,fb->len);
    Serial.println("Image sent");
    esp_camera_fb_return(fb);

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if(isnan(h)) {
      h = hmem;
    } else {
      hmem = h;
    }

    if(isnan(t)) {
      t = tmem;
    } else {
      tmem = t;
    }

    String output = "temp=" + String(t, 2) + ",hum=" + String(h, 2) + ",light=12;state:ON_BOARD_LED_1=" + String(flashlight);
    Serial.println(output);

    client.send(output);
  }
}

/*
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
 */
