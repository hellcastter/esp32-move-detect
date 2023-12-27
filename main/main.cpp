#include "esp_log.h"

#include "ProcessorDifference.h"
#include "ProcessorDifferenceSimple.h"
#include "ProcessorAverageDif.h"
#include "ProcessorMedianDif.h"
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

        Processor* proc_dfs = new ProcessorDifference(cam);
        Processor* proc_simple = new ProcessorDifferenceSimple(cam);
        Processor* procAvg = new ProcessorAverageDif(cam);
        Processor* procMed = new ProcessorMedianDif(cam);

        server.add_url("/simple_dfs", proc_dfs);
        server.add_url("/simple", proc_simple);
        server.add_url("/average", procAvg);
        server.add_url("/median", procMed);

        ESP_LOGI(TAG, "ESP32 CAM Web Server is up and running\n");
    }
    else{
        ESP_LOGI(TAG, "Failed to connected with Wi-Fi, check your network Credentials\n");
    }
}