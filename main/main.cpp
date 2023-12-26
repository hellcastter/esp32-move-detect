#include <esp_system.h>
#include <nvs_flash.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_log.h"

extern "C" {
#include "connect_wifi.h"
}

#include "ProcessorDifference.h"
#include "Server.h"
#include "WifiController.h"

#define TAG "esp32-cam Webserver"

#define CONFIG_XCLK_FREQ 20000000

extern "C" void app_main(void)
{
//    esp_err_t err;
    WifiConfig wifiConfig;
    WifiController wifi;
    wifi.setUpConnection(wifiConfig);

    if (wifi_connect_status)
    {
        Server server;
        ProcessorDifference proc;
        server.add_url("/", &proc);

        ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running\n");
    }
    else
        ESP_LOGI(TAG, "Failed to connected with Wi-Fi, check your network Credentials\n");
}