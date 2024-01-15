//
// Created by Victor Muryn on 29.11.2023.
//

#ifndef MAIN_PROCESSORDIFFERENCE_H
#define MAIN_PROCESSORDIFFERENCE_H

#include "Processor.h"
#include "Camera.h"
#include <cstdint>
#include <cstring>
#include <queue>


class ProcessorDifference: public virtual Processor {
private:
    uint8_t* prev;
    Camera* cam;
    uint8_t threshold = 100;

public:
    ProcessorDifference(Camera* camera);
    camera_fb_t* iterate() override;

    ~ProcessorDifference();
};


#endif //MAIN_PROCESSORDIFFERENCE_H
