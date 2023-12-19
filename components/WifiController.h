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


#include "WifiConfig.h"

class WifiController {
private:
    void connect_wifi(wifi_config_t wifi_config);
    static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
public:
    static WifiConfig conf_class;

    // new method
    void setUpConnection(WifiConfig wifi_config);
};

#endif //ESP32_MOVE_DETECT_WIFICONTROLLER_H
