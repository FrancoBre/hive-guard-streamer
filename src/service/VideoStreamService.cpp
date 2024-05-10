#include "VideoStreamService.h"

camera_fb_t* VideoStreamService::frame = nullptr;

camera_fb_t* VideoStreamService::captureFrame() {
    if(frame) {
        esp_camera_fb_return(frame);
    }
    frame = esp_camera_fb_get();
    return frame;
}

uint8_t* VideoStreamService::processFrame(size_t* len) {
    uint8_t *_jpg_buf = nullptr;
    if (frame->format != PIXFORMAT_JPEG) {
        bool jpeg_converted = frame2jpg(frame, 80, &_jpg_buf, len);
        if (!jpeg_converted) {
            Logger.print(__FILE__, __LINE__, "JPEG compression failed");
            return nullptr;
        }
    } else {
        *len = frame->len;
        _jpg_buf = frame->buf;
    }
    return _jpg_buf;
}

void VideoStreamService::releaseFrame() {
    if(frame) {
        esp_camera_fb_return(frame);
        frame = nullptr;
    }
}