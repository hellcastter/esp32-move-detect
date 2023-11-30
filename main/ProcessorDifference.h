//
// Created by Victor Muryn on 29.11.2023.
//

#ifndef MAIN_PROCESSORDIFFERENCE_H
#define MAIN_PROCESSORDIFFERENCE_H

#include "Processor.h"
#include "Camera.h"
#include <cstdint>
#include <cstring>


class ProcessorDifference: public Processor {
private:
    uint8_t* prev;
    size_t width{};
    size_t height{};
    size_t len{};
    Camera cam = Camera(PIXFORMAT_GRAYSCALE, FRAMESIZE_QVGA);
    uint8_t threshold = 30;
public:
    ProcessorDifference();
    camera_fb_t* iterate() override;

    ~ProcessorDifference() override;
};


#endif //MAIN_PROCESSORDIFFERENCE_H
