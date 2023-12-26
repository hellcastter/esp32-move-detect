//
// Created by Victor Muryn on 29.11.2023.
//

#ifndef MAIN_PROCESSOR_H
#define MAIN_PROCESSOR_H

#include <cstdint>
#include "esp_camera.h"

class Processor {
public:
    virtual camera_fb_t* iterate() = 0;
};

#endif //MAIN_PROCESSOR_H
