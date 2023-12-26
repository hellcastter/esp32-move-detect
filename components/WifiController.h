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

#include "WifiCredits.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define MAXIMUM_RETRY 5
#define S_RETRY_SUM 5
#define TAG_WIFI "Connect_WiFi"

void event_handler(void *arg, esp_event_base_t event_base,
                   int32_t event_id, void *event_data);

class WifiController {
public:
    WifiController();
};

#endif //ESP32_MOVE_DETECT_WIFICONTROLLER_H
