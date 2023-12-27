#ifndef MAIN_PROCESSOR_MEDIAN_BOX_H
#define MAIN_PROCESSOR_MEDIAN_BOX_H

#include "Processor.h"
#include "Camera.h"
#include <cstdint>
#include <cstring>
#include <queue>
#include <algorithm>

#define LAST_FRAMES_NUM 5

class ProcessorMedianBox: public virtual Processor {
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

    void dfs(camera_fb_t* fb, size_t pos, bool draw);
    uint8_t medianValue(size_t pixIdx);

public:
    ProcessorMedianBox(Camera* camera);
    camera_fb_t* iterate() override;
    ~ProcessorMedianBox();
};

#endif //MAIN_PROCESSOR_MEDIAN_BOX_H