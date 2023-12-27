#include "ProcessorAverageDif.h"

const char* AvgDifTAG = "ProcessorAverageDifferenceTAG";

ProcessorAverageDif::ProcessorAverageDif(uint8_t threshold_in) {
    threshold = threshold_in;

    for(size_t iter = 0; iter < LAST_FRAMES_NUM; iter++){
        auto fb = cam.take_picture();

        prev[iter] = new uint8_t[fb->len];
        std::copy(fb->buf, fb->buf + fb->len, prev[iter]);

        cam.free_picture();
    }
    
    auto fb = cam.take_picture();
    
    width = fb->width;
    height = fb->height;
    len = fb->len;

    same = new bool[fb->len];
    cam.free_picture();

    ESP_LOGI(AvgDifTAG, "Processor was initialized");
}

ProcessorAverageDif::~ProcessorAverageDif() {
    for(size_t iter = 0; iter < LAST_FRAMES_NUM; iter++){
        free(prev[iter]);
    }
    free(same);
}

uint8_t ProcessorAverageDif::averageValue(size_t pixIdx){
    uint32_t sum = 0;
    for(size_t iter = 0; iter < LAST_FRAMES_NUM; iter++){
        sum += prev[iter][pixIdx];
    }
    return (uint8_t)(sum / LAST_FRAMES_NUM);
}

camera_fb_t* ProcessorAverageDif::iterate(){
    auto fb = cam.take_picture();

    if (!fb)
        return nullptr;

    memset(same, true, fb->len);

    for (size_t iter = 0; iter < fb->len; ++iter) {
        uint8_t background = averageValue(iter);
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