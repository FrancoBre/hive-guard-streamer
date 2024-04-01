#include "VideoStreamHandler.h"
#include "esp_camera.h"
#include <Arduino.h>

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

esp_err_t VideoStreamHandler::handle(httpd_req_t *req) {
    camera_fb_t *fb = nullptr;
    struct timeval _timestamp{};
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t *_jpg_buf = nullptr;
    char *part_buf[128];

    static int64_t last_frame = 0;
    if (!last_frame) {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, STREAM_CONTENT_TYPE);
    if (res != ESP_OK) {
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "X-Framerate", "60");

#if CONFIG_LED_ILLUMINATOR_ENABLED
    isStreaming = true;
enable_led(true);
#endif

    while (true) {
        fb = esp_camera_fb_get();
        if (!fb) {
            log_e("Camera capture failed");
            res = ESP_FAIL;
        } else {
            _timestamp.tv_sec = fb->timestamp.tv_sec;
            _timestamp.tv_usec = fb->timestamp.tv_usec;

            if (fb->format != PIXFORMAT_JPEG) {
                bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);
                esp_camera_fb_return(fb);
                fb = nullptr;
                if (!jpeg_converted) {
                    log_e("JPEG compression failed");
                    res = ESP_FAIL;
                }
            } else {
                _jpg_buf_len = fb->len;
                _jpg_buf = fb->buf;
            }
        }
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, STREAM_BOUNDARY, strlen(STREAM_BOUNDARY));
        }
        if (res == ESP_OK) {
            size_t hlen = snprintf((char *) part_buf, 128, STREAM_PART, _jpg_buf_len, _timestamp.tv_sec,
                                   _timestamp.tv_usec);
            res = httpd_resp_send_chunk(req, (const char *) part_buf, hlen);
        }
        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char *) _jpg_buf, _jpg_buf_len);
        }
        if (fb) {
            esp_camera_fb_return(fb);
            fb = nullptr;
            _jpg_buf = nullptr;
        } else if (_jpg_buf) {
            free(_jpg_buf);
            _jpg_buf = nullptr;
        }
        if (res != ESP_OK) {
            log_e("Send frame failed");
            break;
        }
        int64_t fr_end = esp_timer_get_time();

        int64_t frame_time = fr_end - last_frame;
        frame_time /= 1000;
#if ARDUHAL_LOG_LEVEL >= ARDUHAL_LOG_LEVEL_INFO
        uint32_t avg_frame_time = ra_filter_run(&ra_filter, frame_time);
#endif
        log_i("MJPG: %uB %ums (%.1ffps), AVG: %ums (%.1ffps)",
              (uint32_t) (_jpg_buf_len),
              (uint32_t) frame_time, 1000.0 / (uint32_t) frame_time,
              avg_frame_time, 1000.0 / avg_frame_time);
    }

#if CONFIG_LED_ILLUMINATOR_ENABLED
    isStreaming = false;
enable_led(false);
#endif

    return res;
}