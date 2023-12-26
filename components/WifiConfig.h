#ifndef ESP32_MOVE_DETECT_WIFICONFIG_H
#define ESP32_MOVE_DETECT_WIFICONFIG_H

// added
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

// #define WIFI_AUTH_WPA2_PSK
#define WIFI_SSID "DmytroiPhone"
#define WIFI_PASSWORD ""
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

#define MAXIMUM_RETRY 5
#define S_RETRY_SUM 5

class WifiConfig {
public:
    // Declare getConfig as inline
    inline static wifi_config_t getConfig() {
        wifi_config_t config = {.sta = {}};
        strlcpy((char *)config.sta.ssid, WIFI_SSID, sizeof(config.sta.ssid));
        strlcpy((char *)config.sta.password, WIFI_PASSWORD, sizeof(config.sta.password));
        return config;
    }

    inline const char *getWifiConfigTag() {
        return "Connect_WiFi";
    }

    inline EventGroupHandle_t getWifiEventGroup() {
        static EventGroupHandle_t s_wifi_event_group;
        return s_wifi_event_group;
    }

    inline int getWifiConnectStatus() {
        static int wifi_connect_status = 0;
        return wifi_connect_status;
    }

    inline int getRetryNum() {
        static int s_retry_num = 0;
        return s_retry_num;
    }

    inline static int getMaxRetry() {
        return 5;
    }

    inline static int getSRetrySum() {
        return 5;
    }
};
#endif //ESP32_MOVE_DETECT_WIFICONFIG_H
