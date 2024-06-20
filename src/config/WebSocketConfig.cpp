#include "WebSocketConfig.h"

WebsocketsClient client;
int flashlight = 0;

void WebSocketConfig::initialize(String masterServerIp, int wsPort) {
    Logger.print(__FILE__, __LINE__, "Initializing WebSocket with masterServerIp: ", masterServerIp.c_str(), " and wsPort: ", wsPort);

    String websocket_server = "ws://" + masterServerIp + ":" + wsPort + "/";
    const char* websocket_server_cstr = websocket_server.c_str();

    Logger.print(__FILE__, __LINE__, "Constructed WebSocket server string: ", websocket_server_cstr);

    client.onMessage(onMessageCallback);
    client.onEvent(onEventsCallback);

    while (!client.connect(websocket_server_cstr)) {
        Logger.print(__FILE__, __LINE__, "Connecting to the websocket server...");
        delay(500);
    }
}

void WebSocketConfig::onEventsCallback(WebsocketsEvent event, String data) {
    if (event == WebsocketsEvent::ConnectionOpened) {
        Logger.print(__FILE__, __LINE__, "Connection Opened");
    } else if (event == WebsocketsEvent::ConnectionClosed) {
        Logger.print(__FILE__, __LINE__, "Connection Closed");
    } else if (event == WebsocketsEvent::GotPing) {
        Logger.print(__FILE__, __LINE__, "Got a Ping!");
    } else if (event == WebsocketsEvent::GotPong) {
        Logger.print(__FILE__, __LINE__, "Got a Pong!");
    }
}

void WebSocketConfig::onMessageCallback(WebsocketsMessage message) {
    String data = message.data();
    int index = data.indexOf("=");

    if (index != -1) {
        String key = data.substring(0, index);
        String value = data.substring(index + 1);

        if (key == "ON_BOARD_LED_1") {
            if (value.toInt() == 1) {
                flashlight = 1;
                digitalWrite(FLASH_PIN, HIGH);
            } else {
                flashlight = 0;
                digitalWrite(FLASH_PIN, LOW);
            }
        }

        Logger.print(__FILE__, __LINE__, "Key: ", key, "Value: ", value);
    }
}
