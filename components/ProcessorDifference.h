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
    size_t width{};
    size_t height{};
    size_t len{};
    Camera cam = Camera(PIXFORMAT_GRAYSCALE, FRAMESIZE_QQVGA);
    uint8_t threshold = 70;
    int r = 5;
    bool* same;

    void dfs(camera_fb_t* fb, size_t pos, bool draw);
public:
    ProcessorDifference();
    camera_fb_t* iterate() override;

    ~ProcessorDifference();
};


#endif //MAIN_PROCESSORDIFFERENCE_H
