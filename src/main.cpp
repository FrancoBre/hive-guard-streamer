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

void initializeHttpServer(int i);

String getStreamerData_ToSendToMasterServer(int wsPort, int appPort);

std::pair<int, int> findAvailablePorts();

AsyncWebServer *server;

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

    std::pair<int, int> ports = findAvailablePorts();
    int wsPort = ports.first;
    int appPort = ports.second;

    String jsonData = getStreamerData_ToSendToMasterServer(wsPort, appPort);

    initializeHttpServer(appPort);

    String masterIp = MasterServerConfig::getMasterIp(jsonData);

    if (masterIp.isEmpty()) {
        Logger.print(__FILE__, __LINE__, "Master server IP not found");
        ESP.restart();
    }
    WebSocketConfig::initialize(masterIp, wsPort);
}

String getStreamerData_ToSendToMasterServer(int wsPort, int appPort) {
    int randomId = random(1000, 9999);
    String ip = WiFi.localIP().toString();

    return "{"
           "\"id\": \"esp32cam" + String(randomId) + "\","
                                                     "\"wsPort\": \"" + String(wsPort) + "\","
                                                                                         "\"appPort\": \"" +
           String(appPort) + "\","
                             "\"saveSensorData\": true,"
                             "\"detectObjects\": true,"
                             "\"class\": \"cam-instance\","
                             "\"display\": \"Cam #" + String(randomId) + "\","
                                                                         "\"ip\": \"" + ip + "\","
                                                                                             "\"commands\": [{"
                                                                                             "\"id\": \"ON_BOARD_LED\", \"name\": \"Camera flashlight\", \"class\": \"led-light\", \"state\": 0"
                                                                                             "}]""}";
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

    if (fb->format != PIXFORMAT_JPEG) { return; }

    client.sendBinary((const char *) fb->buf, fb->len);
    esp_camera_fb_return(fb);

    float h = TemperatureHumiditySensorHandler::readDHTHumidity();
    float t = TemperatureHumiditySensorHandler::readDHTTemperature();


    String output =
            "temp=" + String(t, 2) + ",hum=" + String(h, 2) + ",light=12;state:ON_BOARD_LED_1=" + String(flashlight);

    client.send(output);
}

void initializeHttpServer(int appPort) {
    server = new AsyncWebServer(appPort);

    server->on(
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
                 ":", appPort,
                 "/iAmMaster\n");

    server->on("/status", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger.print(__FILE__, __LINE__, "Status endpoint was hit");
        request->send(200, "text/plain", "Server is running");
    });
    Logger.print(__FILE__, __LINE__, "Status endpoint created! Go to http://", WiFi.localIP().toString().c_str(), ":",
                 appPort,
                 "/status\n");

    server->on("/logs", HTTP_GET, [](AsyncWebServerRequest *request) {
        Logger.print(__FILE__, __LINE__, "Logs endpoint was hit");
        String allLogs = LogsHandler::getLogs();
        request->send(200, "text/html", allLogs);
    });
    Logger.print(__FILE__, __LINE__, "Logs endpoint created! Go to http://", WiFi.localIP().toString().c_str(), ":",
                 appPort,
                 "/logs\n");

    server->begin();

    Logger.print(__FILE__, __LINE__, "HTTP server started successfully!");
}

std::pair<int, int> findAvailablePorts() {
    WiFiClient client;
    std::pair<int, int> ports(-1, -1); // Initialize to -1, -1 (invalid ports)

    for (int port = 8001; port < 9000; port++) {
        if (!client.connect(WiFi.localIP(), port)) {
            if (ports.first == -1) {
                ports.first = port;
            } else {
                ports.second = port;
                break;
            }
        }
    }
    return ports;
}

