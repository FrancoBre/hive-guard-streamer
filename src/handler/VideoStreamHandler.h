#ifndef VIDEOSTREAMHANDLER_H
#define VIDEOSTREAMHANDLER_H

#include "esp_http_server.h"
#include "utils/CustomSerial.h"

class VideoStreamHandler {
public:
    static esp_err_t handle(httpd_req_t *req);
};

#endif // VIDEOSTREAMHANDLER_H
