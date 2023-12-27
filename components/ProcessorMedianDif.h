#ifndef MAIN_PROCESSOR_MEDIAN_DIFFERENCE_H
#define MAIN_PROCESSOR_MEDIAN_DIFFERENCE_H

#include "Processor.h"
#include "Camera.h"
#include <cstdint>
#include <cstring>
#include <algorithm>

#define LAST_FRAMES_NUM 5

class ProcessorMedianDif: public virtual Processor {
private:
    Camera* cam;
    uint8_t threshold = 70;

    size_t width;
    size_t height;
    size_t len;

    bool* same;
    uint8_t prevToChange = 0;

    uint8_t* prev [LAST_FRAMES_NUM];

    uint8_t medianValue(size_t pixIdx);

public:
    ProcessorMedianDif(Camera* camera);
    camera_fb_t* iterate() override;
    ~ProcessorMedianDif();
};

#endif //MAIN_PROCESSOR_MEDIAN_DIFFERENCE_H