//
// Created by Victor Muryn on 29.11.2023.
//

#include <algorithm>
#include "ProcessorDifferenceSimple.h"

const char* PCTag = "ProcessorDifferenceSimpleTAG";

ProcessorDifferenceSimple::ProcessorDifferenceSimple(Camera *camera) {
    cam = camera;

    auto fb = cam->take_picture();

    prev = new uint8_t[fb->len];
    std::copy(fb->buf, fb->buf + fb->len, prev);

    ESP_LOGI(PCTag, "ProcessorDifferenceSimple was inited");

    cam->free_picture();
}

camera_fb_t* ProcessorDifferenceSimple::iterate() {
    auto fb = cam->take_picture();

    if (!fb)
        return nullptr;

    // iterate over all image
    for (size_t iter = 0; iter < fb->len; ++iter) {
        // check whether pixel changed
        if ( (fb->buf[iter] > prev[iter] && fb->buf[iter] - prev[iter] > threshold) ||
             (fb->buf[iter] < prev[iter] && prev[iter] - fb->buf[iter] > threshold) ) {
            prev[iter] = fb->buf[iter];
            fb->buf[iter] = 255;
        } else {
            prev[iter] = fb->buf[iter];
        }
    }

    return fb;
}

ProcessorDifferenceSimple::~ProcessorDifferenceSimple() {
    delete[] prev;
}
