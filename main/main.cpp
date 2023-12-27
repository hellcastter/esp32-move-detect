#include <esp_system.h>
#include <nvs_flash.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#include "esp_camera.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_log.h"

#include "ProcessorDifference.h"
#include "ProcessorAverageDif.h"
#include "Server.h"
#include "WifiController.h"

#define TAG "esp32-cam Webserver"

#define CONFIG_XCLK_FREQ 20000000

extern bool wifi_connect_status;

extern "C" void app_main(void)
{    
    WifiController wifi;

    if (wifi_connect_status)
    {
        Server server;
        Processor* proc = new ProcessorDifference();
        // Processor* procAvg = new ProcessorAverageDif(40);
        server.add_url("/", proc);
        // server.add_url("/", procAvg);

        ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running\n");
    }
    else{
        ESP_LOGI(TAG, "Failed to connected with Wi-Fi, check your network Credentials\n");
    }
}