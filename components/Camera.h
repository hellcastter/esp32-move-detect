//
// Created by Victor Muryn on 29.11.2023.
//

#ifndef MAIN_CAMERA_H
#define MAIN_CAMERA_H

#include "Camera.config.h"
#include <esp_system.h>
#include "esp_camera.h"
#include <esp_log.h>


#define CONFIG_XCLK_FREQ 20000000

class Camera {
private:
    camera_fb_t* frame = nullptr;
    camera_config_t camera_config = {
        .pin_pwdn  = CAM_PIN_PWDN,
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

        .xclk_freq_hz = CONFIG_XCLK_FREQ,
        .ledc_timer = LEDC_TIMER_0,
        .ledc_channel = LEDC_CHANNEL_0,

        .fb_count = 1,
        .grab_mode = CAMERA_GRAB_WHEN_EMPTY
    };
public:
    Camera(pixformat_t pixformat, framesize_t framesize);
    camera_fb_t* take_picture();
    void free_picture();
    ~Camera();
};


#endif //MAIN_CAMERA_H
