//
// Created by Victor Muryn on 29.11.2023.
//

#ifndef MAIN_PROCESSORDIFFERENCESIMPLE_H
#define MAIN_PROCESSORDIFFERENCESIMPLE_H

#include "Processor.h"
#include "Camera.h"
#include <cstdint>
#include <cstring>
#include <queue>


class ProcessorDifferenceSimple: public virtual Processor {
private:
    uint8_t* prev;
    Camera* cam;
    uint8_t threshold = 100;

public:
    ProcessorDifferenceSimple(Camera* camera);
    camera_fb_t* iterate() override;

    ~ProcessorDifferenceSimple();
};


#endif //MAIN_PROCESSORDIFFERENCESIMPLE_H
