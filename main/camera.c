// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: https://pvs-studio.com

/**
 * This example takes a picture every 5s and print its size on serial monitor.
 */

// =============================== SETUP ======================================

// 1. Board setup (Uncomment):

/**
 * 2. Enable PSRAM (if you have one) using idf menuconfig or on sdkconfig:
 *
 * CONFIG_ESP32_SPIRAM_SUPPORT=y
 *
 * More info on
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/kconfig.html#config-esp32-spiram-support
 */

// ================================ CODE ======================================

#include <esp_log.h>
#include <esp_system.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// support IDF 5.x
#ifndef portTICK_RATE_MS
#define portTICK_RATE_MS portTICK_PERIOD_MS
#endif

#include "esp_camera.h"
#include "config.h"
#include "vector.h"

static const char *TAG = "example:take_picture";

#if ESP_CAMERA_SUPPORTED
static camera_config_t camera_config = {
    .fb_location = CAMERA_FB_IN_PSRAM,

    .pin_pwdn = CAM_PIN_PWDN,
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

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,

    .pixel_format = PIXFORMAT_GRAYSCALE, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_QVGA,    //QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

    .jpeg_quality = 12, //0-63, for OV series camera sensors, lower number means higher quality
    .fb_count = 1,       //When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

static esp_err_t init_camera(void)
{
    //initialize the camera
    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Camera Init Failed");
        return err;
    }

    return ESP_OK;
}
#endif

void save_to_array(const uint8_t* buf, uint8_t* array, size_t size) {
    for (size_t i = 0; i < size; i++) {
        array[i] = buf[i];
    }
}

#define THRESHOLD 20
#define WIDTH 240
#define HEIGHT 320

void findDifference(uint8_t* pic1, const uint8_t* pic2, const size_t size) {
    for (size_t i = 0; i < size; ++i) {
        pic1[i] = abs(pic1[i] - pic2[i]) > THRESHOLD ? 1 : 0;
    }
}

// Define the neighbors array
size_t neighbors[][2] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};

// Define the dfs function
void dfs(size_t x, size_t y, bool* visited, const uint8_t* picture, size_t* sizes) {
    size_t index = y * WIDTH + x;

    if (x < WIDTH && y < HEIGHT && !visited[index] && picture[index] == 1) {
        visited[index] = 1;

        sizes[0] = sizes[0] < x ? sizes[0] : x;
        sizes[1] = sizes[1] < y ? sizes[1] : y;

        sizes[2] = sizes[2] > x ? sizes[2] : x;
        sizes[3] = sizes[3] > y ? sizes[3] : y;

        for (size_t i = 0; i < 4; i++) {
            size_t dx = neighbors[i][0];
            size_t dy = neighbors[i][1];
            dfs(x + dx, y + dy, visited, picture, sizes);
        }
    }
}

void findContours(const uint8_t* picture, const size_t size, Array** contours) {
    bool *visited = calloc(size, sizeof(bool));

    for (size_t i = 0; i < HEIGHT; ++i) {
        for (size_t j = 0; j < WIDTH; ++j) {
            size_t index = i * WIDTH + j;

            if (picture[index] == 1 && !visited[index]) {
                size_t* sizes = (size_t*)malloc(4 * sizeof(size_t));
                sizes[0] = WIDTH;
                sizes[1] = HEIGHT;
                sizes[2] = 0;
                sizes[3] = 0;
//                        {WIDTH, HEIGHT, 0, 0};

                // ESP_LOGI(TAG, "[APP] Free memory: %lu bytes", esp_get_free_heap_size());
                insertItem(*contours, sizes);
                dfs(j, i, visited, picture, sizes);
            }
        }
    }

    free(visited);
}


void app_main(void)
{
    if (ESP_OK != init_camera()) {
        return;
    }

    size_t size = 76800;
    uint8_t *prev_pic = NULL;

    while (1)
    {
        ESP_LOGI(TAG, "Taking picture...");
        camera_fb_t *pic = esp_camera_fb_get();

        uint8_t *curr_pic = (uint8_t*) malloc(size * sizeof(uint8_t));

        save_to_array(pic->buf, curr_pic, size);
        esp_camera_fb_return(pic);

        // analysis
        if (prev_pic != NULL) {
            findDifference(prev_pic, curr_pic, size);

            Array* contours;
            arrayInit(&contours);

            findContours(prev_pic, size, &contours);

            ESP_LOGI(TAG, "%zu", contours->size);
            ESP_LOGI(TAG, "[APP] Free memory: %lu bytes", esp_get_free_heap_size());


            for (size_t i = 0; i < contours->size; ++i) {
                ESP_LOGI(TAG, "%d %d %d %d", getItem(contours, i)[0], getItem(contours, i)[1], getItem(contours, i)[2], getItem(contours, i)[3]);
                free(getItem(contours, i));
            }

            freeArray(contours);
            free(prev_pic);
        }

        prev_pic = curr_pic;

        vTaskDelay(5000 / portTICK_RATE_MS);
    }
}