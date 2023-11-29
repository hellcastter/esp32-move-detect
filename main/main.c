#include <esp_system.h>
#include <nvs_flash.h>
#include <esp_log.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"

#include "camera_pins.h"
#include "connect_wifi.h"

static const char *TAG = "esp32-cam Webserver";

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/bmp\r\nContent-Length: %u\r\n\r\n";

#define CONFIG_XCLK_FREQ 20000000 

static esp_err_t init_camera(void)
{
    camera_config_t camera_config = {
        .pin_pwdn  = CAM_PIN_PWDN,
        .pin_reset = CAM_PIN_RESET,
        .pin_xclk = CAM_PIN_XCLK,
        .pin_sccb_sda = CAM_PIN_SIOD,
        .pin_sccb_scl = CAM_PIN_SIOC,

        .pin_d7 = CAM_PIN_D7,
        .pin_d6 = CAM_PIN_D6,
        .pin_d5 = CAM_PIN_D5,
        .pin_d4 = CAM_PIN_D4,
        .pin_d3 = CAM_PIN_D3,
        .pin_d2 = CAM_PIN_D2,
        .pin_d1 = CAM_PIN_D1,
        .pin_d0 = CAM_PIN_D0,
        .pin_vsync = CAM_PIN_VSYNC,
        .pin_href = CAM_PIN_HREF,
        .pin_pclk = CAM_PIN_PCLK,

        .xclk_freq_hz = CONFIG_XCLK_FREQ,
        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,

        .pixel_format = PIXFORMAT_GRAYSCALE,
        .frame_size = FRAMESIZE_QQVGA,

        .fb_count = 1,
        .grab_mode = CAMERA_GRAB_WHEN_EMPTY};//CAMERA_GRAB_LATEST. Sets when buffers should be filled
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        return err;
    }
    return ESP_OK;
}

esp_err_t jpg_stream_httpd_handler(httpd_req_t *req){
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _bmp_buf_len;
    uint8_t * _bmp_buf;
    char * part_buf[64];
    static int64_t last_frame = 0;
    if(!last_frame) {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if(res != ESP_OK){
        return res;
    }

    // gets first image, to compare with
    // later updates automaticaly
    fb = esp_camera_fb_get();
    uint8_t* prevFb = (uint8_t*)malloc(fb->len * sizeof(uint8_t));
    memcpy(prevFb, fb->buf, fb->len);
    esp_camera_fb_return(fb);

    while(true){
        fb = esp_camera_fb_get();

        uint8_t threshold = 30;
        for(size_t iter = 0; iter < fb->len; iter++){
            // if (fb->buf[iter] > prevFb[iter]){
            //     prevFb[iter] = fb->buf[iter];
            //     fb->buf[iter] = fb->buf[iter] - prevFb[iter];
            // } else if (fb->buf[iter] < prevFb[iter]){
            //     prevFb[iter] = fb->buf[iter];
            //     fb->buf[iter] = prevFb[iter] - fb->buf[iter];
            // }

            if (fb->buf[iter] > prevFb[iter] && fb->buf[iter] - prevFb[iter] > threshold){
                prevFb[iter] = fb->buf[iter];
                fb->buf[iter] = 255;
            } else if (fb->buf[iter] < prevFb[iter] && prevFb[iter] - fb->buf[iter] > threshold){
                prevFb[iter] = fb->buf[iter];
                fb->buf[iter] = 255;
            }

            // } else {
            //     prevFb[iter] = fb->buf[iter];
            //     fb->buf[iter] = 0;
            // }
        }

        if (!fb) {
            ESP_LOGE(TAG, "Camera capture failed");
            res = ESP_FAIL;
            break;
        }

        bool bmp_converted = frame2bmp(fb, &_bmp_buf, &_bmp_buf_len);
        if(!bmp_converted){
            ESP_LOGE(TAG, "BMP compression failed");
            esp_camera_fb_return(fb);
            res = ESP_FAIL;
        }

        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
        }
        if(res == ESP_OK){
            size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _bmp_buf_len);

            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }
        if(res == ESP_OK){
            res = httpd_resp_send_chunk(req, (const char *)_bmp_buf, _bmp_buf_len);
        }

        esp_camera_fb_return(fb);
        if(res != ESP_OK){
            break;
        }
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

    free(prevFb);
    free(_bmp_buf);
    last_frame = 0;
    return res;
}

httpd_uri_t uri_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = jpg_stream_httpd_handler,
    .user_ctx = NULL};
httpd_handle_t setup_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t stream_httpd  = NULL;

    if (httpd_start(&stream_httpd , &config) == ESP_OK)
    {
        httpd_register_uri_handler(stream_httpd , &uri_get);
    }

    return stream_httpd;
}

void app_main()
{
    esp_err_t err;

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    connect_wifi();

    if (wifi_connect_status)
    {
        err = init_camera();
        if (err != ESP_OK)
        {
            printf("err: %s\n", esp_err_to_name(err));
            return;
        }
        setup_server();
        ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running\n");
    }
    else
        ESP_LOGI(TAG, "Failed to connected with Wi-Fi, check your network Credentials\n");
}