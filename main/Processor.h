//
// Created by Victor Muryn on 29.11.2023.
//

#ifndef MAIN_PROCESSOR_H
#define MAIN_PROCESSOR_H

#include <cstdint>
#include "esp_camera.h"

class Processor {
public:
    virtual void setup();
    virtual camera_fb_t* iterate();

    virtual ~Processor() = default;
};

#endif //MAIN_PROCESSOR_H
