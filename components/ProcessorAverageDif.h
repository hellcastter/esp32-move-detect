#ifndef MAIN_PROCESSOR_AVG_DIFFERENCE_H
#define MAIN_PROCESSOR_AVG_DIFFERENCE_H

#include "Processor.h"
#include "Camera.h"
#include <cstdint>
#include <cstring>
#include <queue>

#define LAST_FRAMES_NUM 5

class ProcessorAverageDif: public virtual Processor {
private:
    Camera* cam;
    uint8_t threshold = 70;

    size_t width;
    size_t height;
    size_t len;

    bool* same;
    uint8_t prevToChange = 0;

    uint8_t* prev [LAST_FRAMES_NUM];

    uint8_t averageValue(size_t pixIdx);

public:
    ProcessorAverageDif(Camera* camera);
    camera_fb_t* iterate() override;
    ~ProcessorAverageDif();
};

#endif //MAIN_PROCESSOR_AVG_DIFFERENCE_H