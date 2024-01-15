#include "esp_log.h"

#include "ProcessorDefault.h"
#include "ProcessorDifference.h"
#include "ProcessorDifferenceBox.h"
#include "ProcessorAverageDif.h"
#include "ProcessorAverageBox.h"
#include "ProcessorMedianDif.h"
#include "ProcessorMedianBox.h"

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

        auto* cam = new Camera(PIXFORMAT_GRAYSCALE, FRAMESIZE_QQVGA);

        Processor* processorDefault = new ProcessorDefault(cam);

        Processor* simple = new ProcessorDifference(cam);
        Processor* simpleBox = new ProcessorDifferenceBox(cam);

        Processor* average = new ProcessorAverageDif(cam);
        Processor* averageBox = new ProcessorAverageBox(cam);

        Processor* median = new ProcessorMedianDif(cam);
        Processor* medianBox = new ProcessorMedianBox(cam);

        server.add_url("/", processorDefault);

        server.add_url("/simple", simple);
        server.add_url("/average", average);
        server.add_url("/median", median);
        
        server.add_url("/simpleBox", simpleBox);
        server.add_url("/averageBox", averageBox);
        server.add_url("/medianBox", medianBox);

        ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running\n");
    } else {
        ESP_LOGI(TAG, "Failed to connected with Wi-Fi, check your network Credentials\n");
    }
}