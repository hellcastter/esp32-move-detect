//
// Created by Victor Muryn on 29.11.2023.
//
#include "Camera.h"

const char *CAMERA_TAG = "Camera";

Camera::Camera(pixformat_t pixformat, framesize_t framesize) {
    camera_config.pixel_format = pixformat;
    camera_config.frame_size = framesize;

    esp_err_t err = esp_camera_init(&camera_config);
    if (err != ESP_OK) {
        ESP_LOGE(CAMERA_TAG, "init failed\n");
        return;
    }

    ESP_LOGI(CAMERA_TAG, "camera was inited");
}

Camera::~Camera() {
    if (frame != nullptr) {
        free_picture();
    }

    delete[] frame;
    delete[] frame;

    esp_camera_deinit();
}

camera_fb_t *Camera::take_picture() {
    if (pictureTaken) {
        free_picture();
    }

    frame = esp_camera_fb_get();

    if (!frame) {
        ESP_LOGE(CAMERA_TAG, "Camera capture failed");
        return nullptr;
    }

    pictureTaken = true;
    return frame;
}

void Camera::free_picture() {
    esp_camera_fb_return(frame);
    pictureTaken = false;
}
