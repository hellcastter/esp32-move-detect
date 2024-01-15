#ifndef MAIN_PROCESSOR_DEFAULT_H
#define MAIN_PROCESSOR_DEFAULT_H


#include "Processor.h"
#include "Camera.h"
#include <cstdint>
#include <cstring>
#include <queue>


class ProcessorDefault: public virtual Processor {
private:
    Camera* cam;

public:
    ProcessorDefault(Camera* camera);
    camera_fb_t* iterate() override;

    ~ProcessorDefault() = default;
};


#endif //MAIN_PROCESSOR_DEFAULT_H
