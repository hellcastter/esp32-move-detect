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
#include <algorithm>

struct Point {
    size_t x, y;

    Point(size_t x, size_t y) : x(x), y(y) {}
};

struct Rect {
    Point first, second;
    bool merged = false;
    Rect (Point f, Point s) : first(f), second(s) {}
};

class ProcessorDifference: public virtual Processor {
private:
    uint8_t* prev;
    size_t width{};
    size_t height{};
    size_t len{};
    Camera* cam;
    uint8_t threshold = 70;
    int r = 5;
    bool* same;

    Rect dfs(camera_fb_t* fb, size_t pos, bool draw);
    std::vector<Rect> merge_rects(std::vector<Rect> rects);
    void draw(const std::vector<Rect>& rects, camera_fb_t* fb);
public:
    ProcessorDifference(Camera* camera);
    camera_fb_t* iterate() override;

    ~ProcessorDifference();
};


#endif //MAIN_PROCESSORDIFFERENCE_H
