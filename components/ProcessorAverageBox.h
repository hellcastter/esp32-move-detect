#ifndef MAIN_PROCESSOR_AVG_BOX_H
#define MAIN_PROCESSOR_AVG_BOX_H

#include "Processor.h"
#include "Camera.h"
#include <cstdint>
#include <cstring>
#include <queue>
#include <algorithm>

#define LAST_FRAMES_NUM 5

class ProcessorAverageBox: public virtual Processor {
private:
    Camera* cam;
    uint8_t threshold = 70;

    size_t width;
    size_t height;
    size_t len;

    bool* same;
    uint8_t prevToChange = 0;

    uint8_t* prev [LAST_FRAMES_NUM];
    int r = 5;

    uint8_t averageValue(size_t pixIdx);

    Rect dfs(size_t pos);
    std::vector<Rect> merge_rects(std::vector<Rect> rects);
    void draw(const std::vector<Rect>& rects, camera_fb_t* fb);
public:
    ProcessorAverageBox(Camera* camera);
    camera_fb_t* iterate() override;
    ~ProcessorAverageBox();
};

#endif //MAIN_PROCESSOR_AVG_BOX_H