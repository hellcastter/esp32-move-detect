//
// Created by Victor Muryn on 29.11.2023.
//

#include <algorithm>
#include "ProcessorDefault.h"

const char* PDTag = "ProcessorDefaultTAG";

ProcessorDefault::ProcessorDefault(Camera *camera) {
    cam = camera;
    ESP_LOGI(PDTag, "ProcessorDefault was inited");
}

camera_fb_t* ProcessorDefault::iterate() {
    auto fb = cam->take_picture();

    return fb;
}
