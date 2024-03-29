//
// Created by Victor Muryn on 29.11.2023.
//

#ifndef MAIN_PROCESSORDIFFERENCEBOX_H
#define MAIN_PROCESSORDIFFERENCEBOX_H

#include "Processor.h"
#include "Camera.h"
#include <cstdint>
#include <cstring>
#include <queue>
#include <algorithm>

class ProcessorDifferenceBox: public virtual Processor {
private:
    uint8_t* prev;
    size_t width{};
    size_t height{};
    size_t len{};
    Camera* cam;
    uint8_t threshold = 70;
    int r = 5;
    bool* same;

    Rect dfs(size_t pos);
    std::vector<Rect> merge_rects(std::vector<Rect> rects);
    void draw(const std::vector<Rect>& rects, camera_fb_t* fb);
public:
    ProcessorDifferenceBox(Camera* camera);
    camera_fb_t* iterate() override;

    ~ProcessorDifferenceBox();
};


#endif //MAIN_PROCESSORDIFFERENCEBOX_H
