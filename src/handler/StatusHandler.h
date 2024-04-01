#ifndef STATUSHANDLER_H
#define STATUSHANDLER_H

class StatusHandler {
public:
    static esp_err_t handle(httpd_req_t *req);
};

#endif // STATUSHANDLER_H
