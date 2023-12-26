#include <esp_system.h>
#include <nvs_flash.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_log.h"

// extern "C" {
// #include "connect_wifi.h"
// }

// added
#include "WifiController.h"
#include "WifiConfig.h"
// added

#include "ProcessorDifference.h"

#define TAG "esp32-cam Webserver"

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const size_t _STREAM_LEN = strlen(_STREAM_BOUNDARY);
static const char* _STREAM_PART = "Content-Type: image/bmp\r\nContent-Length: %u\r\n\r\n";

#define CONFIG_XCLK_FREQ 20000000

esp_err_t jpg_stream_httpd_handler(httpd_req_t *req) {
    auto processImage = ProcessorDifference();

    esp_err_t res = ESP_OK;
    size_t _bmp_buf_len;
    uint8_t *_bmp_buf;
    char *part_buf[64];
    static int64_t last_frame = 0;

    if (!last_frame) {
       last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK){
        return res;
    }

    auto fb = processImage.iterate();
    frame2bmp(fb, &_bmp_buf, &_bmp_buf_len);
    size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _bmp_buf_len);

    while (true) {
        fb = processImage.iterate();

        if (!fb) continue;

        bool bmp_converted = frame2bmp(fb, &_bmp_buf, &_bmp_buf_len);
        if (!bmp_converted) {
            ESP_LOGE(TAG, "BMP compression failed");
            res = ESP_FAIL;
        }

        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, _STREAM_LEN);
        }

        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }

        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char *)_bmp_buf, _bmp_buf_len);
        }

        if (res != ESP_OK)
            break;

        int64_t fr_end = esp_timer_get_time();
        int64_t frame_time = fr_end - last_frame;
        last_frame = fr_end;
        frame_time /= 1000;
        ESP_LOGI(TAG, "MBMP: %uKB %ums (%.1ffps) free mem:%u Kb",
           (uint16_t)(_bmp_buf_len/1024),
           (uint16_t)frame_time,
           1000.0 / (uint16_t)frame_time,
           (uint16_t)(esp_get_free_heap_size()/1000));

        free(_bmp_buf);
    }

    return res;
}

httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = jpg_stream_httpd_handler,
    .user_ctx = nullptr
};

void bla() {
    auto processImage = ProcessorDifference();

    static int64_t last_frame = 0;

    if (!last_frame) {
       last_frame = esp_timer_get_time();
    }

    camera_fb_t* fb;

    while (true) {
        fb = processImage.iterate();

        if (!fb) continue;

        int64_t fr_end = esp_timer_get_time();
        int64_t frame_time = fr_end - last_frame;
        last_frame = fr_end;
        frame_time /= 1000;
        ESP_LOGI(TAG, "%ums (%.1ffps) free mem:%u Kb",
           (uint16_t)frame_time,
           1000.0 / (uint16_t)frame_time,
           (uint16_t)(esp_get_free_heap_size()/1000));
    }
}

httpd_handle_t setup_server()
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t stream_httpd  = nullptr;

    if (httpd_start(&stream_httpd , &config) == ESP_OK)
    {
        httpd_register_uri_handler(stream_httpd , &uri_get);
    }

    return stream_httpd;
}

extern "C" void app_main(void)
{
//    esp_err_t err;

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    // changed
    // connect_wifi();
    WifiConfig wifiConfig;
    WifiController a;
    a.setUpConnection(wifiConfig);


    if (wifiConfig.getWifiConnectStatus())
    {
        setup_server();
        ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running\n");
    }
    else
        ESP_LOGI(TAG, "Failed to connected with Wi-Fi, check your network Credentials\n");
}