#ifndef WEBSOCKETCONFIG_H
#define WEBSOCKETCONFIG_H

#include <stdio.h>
//#include <ArduinoWebsockets.h>
//#include ".components/ArduinoWebsockets/src/ArduinoWebsockets.h"
#include "utils/CustomSerial.h"
#include "../../components/ArduinoWebsockets/src/tiny_websockets/client.hpp"

#define FLASH_PIN 4

using namespace websockets;

extern WebsocketsClient client;
extern int flashlight;

class WebSocketConfig {
public:
    static void initialize(String masterServerIp);

private:
    static void onEventsCallback(WebsocketsEvent event, String data);

    static void onMessageCallback(WebsocketsMessage message);

};


#endif //WEBSOCKETCONFIG_H
