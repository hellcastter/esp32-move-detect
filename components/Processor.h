//
// Created by Victor Muryn on 29.11.2023.
//

#ifndef MAIN_PROCESSOR_H
#define MAIN_PROCESSOR_H

#include <cstdint>
#include "esp_camera.h"

struct Point {
    size_t x, y;

    Point(size_t x, size_t y) : x(x), y(y) {}
};

struct Rect {
    Point first, second;
    bool merged = false;
    Rect (Point f, Point s) : first(f), second(s) {}
};

class Processor {
public:
    virtual camera_fb_t* iterate() = 0;
};

#endif //MAIN_PROCESSOR_H
