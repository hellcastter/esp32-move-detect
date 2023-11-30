//
// Created by Victor Muryn on 29.11.2023.
//

#include "ProcessorDifference.h"

const char* ProcessorDifferenceTAG = "ProcessorDifferenceTAG";

ProcessorDifference::ProcessorDifference() {
    auto fb = cam.take_picture();

    prev = (uint8_t*)malloc(fb->len * sizeof(uint8_t));
    memcpy(prev, fb->buf, fb->len);

//    width = fb->width;
//    height = fb->height;
//    len = fb->len;

    ESP_LOGI(ProcessorDifferenceTAG, "ProcessorDifference was inited");

    cam.free_picture();
}

camera_fb_t* ProcessorDifference::iterate() {
    auto fb = cam.take_picture();

    for (size_t iter = 0; iter < fb->len; iter++){
        if (fb->buf[iter] > prev[iter] && fb->buf[iter] - prev[iter] > threshold) {
            prev[iter] = fb->buf[iter];
            fb->buf[iter] = 255;
        } else if (fb->buf[iter] < prev[iter] && prev[iter] - fb->buf[iter] > threshold) {
            prev[iter] = fb->buf[iter];
            fb->buf[iter] = 255;
        } else {
            prev[iter] = fb->buf[iter];
        }
    }

    cam.free_picture();

    return fb;
}

ProcessorDifference::~ProcessorDifference() {
    delete prev;
//    delete cam;
}
