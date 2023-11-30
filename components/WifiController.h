#ifndef ESP32_MOVE_DETECT_WIFICONTROLLER_H
#define ESP32_MOVE_DETECT_WIFICONTROLLER_H

#include <esp_system.h>
#include <nvs_flash.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "driver/gpio.h"
#include <lwip/sockets.h>
#include <lwip/sys.h>
#include <lwip/api.h>
#include <lwip/netdb.h>

extern int wifi_connect_status;

class WifiController {
private:
    void connect_wifi(void);
    static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
public:
    bool wifi_connect_status;
    static const char *TAG;
    int s_retry_num;
    EventGroupHandle_t s_wifi_event_group;
    

    // new method
    void setUpConnection(wifi_config_t wifi_config);

};

#endif //ESP32_MOVE_DETECT_WIFICONTROLLER_H
