#include "ProcessorMedianDif.h"

const char* MedDifTAG = "ProcessorMedianDifferenceTAG";

ProcessorMedianDif::ProcessorMedianDif(Camera* camera){
    cam = camera;
    for(size_t iter = 0; iter < LAST_FRAMES_NUM; iter++){
        auto fb = cam->take_picture();

        prev[iter] = new uint8_t[fb->len];
        std::copy(fb->buf, fb->buf + fb->len, prev[iter]);

        cam->free_picture();
    }
    
    auto fb = cam->take_picture();
    
    width = fb->width;
    height = fb->height;
    len = fb->len;

    same = new bool[fb->len];
    cam->free_picture();

    ESP_LOGI(MedDifTAG, "Processor was initialized");
}

ProcessorMedianDif::~ProcessorMedianDif() {
    for(size_t iter = 0; iter < LAST_FRAMES_NUM; iter++){
        free(prev[iter]);
    }
    free(same);
}

uint8_t ProcessorMedianDif::medianValue(size_t pixIdx){
    for(size_t iter = 0; iter < LAST_FRAMES_NUM; iter++){
        size_t min = 0;
        size_t max = 0;
        for(size_t jter = 0; jter < LAST_FRAMES_NUM; jter++){
            if(prev[iter][pixIdx] > prev[jter][pixIdx]){
                min++;
                max++;
            } else if(prev[iter][pixIdx] == prev[jter][pixIdx] && iter != jter){
                max++;
            }
        }
        if (min <= LAST_FRAMES_NUM/2 && max >= LAST_FRAMES_NUM/2){
            return prev[iter][pixIdx];
        }
    }
    return prev[prevToChange > 0 ? (prevToChange - 1) : LAST_FRAMES_NUM][pixIdx];
}

camera_fb_t* ProcessorMedianDif::iterate(){
    auto fb = cam->take_picture();

    if (!fb)
        return nullptr;

    memset(same, true, fb->len);

    for (size_t iter = 0; iter < fb->len; ++iter) {
        uint8_t background = medianValue(iter);
        if ( (fb->buf[iter] > background && fb->buf[iter] - background > threshold) ||
             (fb->buf[iter] < background && background - fb->buf[iter] > threshold) ) {
            prev[prevToChange][iter] = fb->buf[iter];
            fb->buf[iter] = 255;
        } else {
            prev[prevToChange][iter] = fb->buf[iter];
        }
    }

    prevToChange = (prevToChange + 1) % LAST_FRAMES_NUM;

    return fb;
}