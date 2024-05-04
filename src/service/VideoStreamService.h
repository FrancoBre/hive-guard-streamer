#ifndef VIDEOSTREAMSERVICE_H
#define VIDEOSTREAMSERVICE_H

#include "esp_camera.h"
#include "utils/CustomSerial.h"

class VideoStreamService {
public:
    static camera_fb_t* captureFrame();
    static uint8_t* processFrame(size_t* len);
    static void releaseFrame();

private:
    static camera_fb_t* frame;
};

#endif //VIDEOSTREAMSERVICE_H