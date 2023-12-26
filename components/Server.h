#ifndef ESP32_MOVE_DETECT_SERVER
#define ESP32_MOVE_DETECT_SERVER

#include <esp_system.h>
#include <nvs_flash.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include <string>

#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "Processor.h"

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const size_t _STREAM_LEN = strlen(_STREAM_BOUNDARY);
static const char* _STREAM_PART = "Content-Type: image/bmp\r\nContent-Length: %u\r\n\r\n";

#define SERVER_TAG "Server"

class Server {
private:
    esp_err_t err;
    httpd_handle_t stream_httpd;
public:
    Server();
    esp_err_t add_url(char* name, Processor* processor);
};

esp_err_t uri_handler(httpd_req_t* req);

#endif // ESP32_MOVE_DETECT_SERVER
