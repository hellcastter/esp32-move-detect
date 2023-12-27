#include "ProcessorAverageBox.h"

const char* AvgBoxTAG = "ProcessorAverageBoxTAG";

ProcessorAverageBox::ProcessorAverageBox(Camera* camera){
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

    ESP_LOGI(AvgBoxTAG, "Processor was initialized");
}

ProcessorAverageBox::~ProcessorAverageBox() {
    for(size_t iter = 0; iter < LAST_FRAMES_NUM; iter++){
        free(prev[iter]);
    }
    free(same);
}

uint8_t ProcessorAverageBox::averageValue(size_t pixIdx){
    uint32_t sum = 0;
    for(size_t iter = 0; iter < LAST_FRAMES_NUM; iter++){
        sum += prev[iter][pixIdx];
    }
    return (uint8_t)(sum / LAST_FRAMES_NUM);
}

camera_fb_t* ProcessorAverageBox::iterate(){
    auto fb = cam->take_picture();

    if (!fb)
        return nullptr;

    memset(same, true, fb->len);

    for (size_t iter = 0; iter < fb->len; ++iter) {
        uint8_t background = averageValue(iter);
        if ( (fb->buf[iter] > background && fb->buf[iter] - background > threshold) ||
             (fb->buf[iter] < background && background - fb->buf[iter] > threshold) ) {
            // set all pixels in radius 'r' as changed
            for (int i = -r; i <= r; ++i) {
                for (int j = -r; j <= r; ++j) {
                    if (iter + i * width + j < len)
                        same[iter + i * width + j] = false;
                    
                }
            }
        }

        prev[prevToChange][iter] = fb->buf[iter];
    }

    // group all changed pixels
    for (size_t i = 0; i < len; i += 1) {
        if (!same[i]) {
            dfs(fb, i, false);
            i += r;
        }
    }

    // group all changed groups and display them
    for (size_t i = 0; i < len; i += 1) {
        if (!same[i]) {
            dfs(fb, i, true);
            i += r;
        }
    }

    prevToChange = (prevToChange + 1) % LAST_FRAMES_NUM;

    return fb;
}

void ProcessorAverageBox::dfs(camera_fb_t *fb, size_t orPos, bool draw) {
    std::queue<size_t> q;
    q.push(orPos);

    std::pair<size_t, size_t> bottomRight{0, 0};
    std::pair<size_t, size_t> topLeft{width - 1, height - 1};

    // classical dfs
    while (!q.empty()) {
        auto pos = q.back();
        q.pop();

        if (same[pos]) continue;
        same[pos] = true;

        auto top = pos / width;
        auto left = pos % width;

        // detect top left and bottom right pixels of group
        if (left < topLeft.first) topLeft.first = left;
        if (left > bottomRight.first) bottomRight.first = left;

        if (top < topLeft.second) topLeft.second = top;
        if (top > bottomRight.second) bottomRight.second = top;

        // add to queue all neighbour pixels
        for (int i = -1; i <= 1; ++i) {
            for (int j = -1; j <= 1; ++j) {
                if (i == 0 && j == 0) continue;
                if (top + i >= height || left + j >= width || pos + i * width + j >= len) continue;

                if (!same[pos + i * width + j])
                    q.push(pos + i * width + j);
            }
        }
    }

    if (draw) {
        // draw white rectangular
        for (auto i = topLeft.first; i <= bottomRight.first; ++i) {
            fb->buf[topLeft.second * width + i] = 255;
            fb->buf[bottomRight.second * width + i] = 255;
        }

        for (auto i = topLeft.second; i <= bottomRight.second; ++i) {
            fb->buf[i * width + topLeft.first] = 255;
            fb->buf[i * width + bottomRight.first] = 255;
        }
    } else {
        if ((bottomRight.first - topLeft.first) <= r || (bottomRight.second - topLeft.second) <= r)
            return;

        // group for the dfs
        for (auto i = topLeft.first; i <= bottomRight.first; ++i) {
            same[topLeft.second * width + i] = false;
            same[bottomRight.second * width + i] = false;
        }

        for (auto i = topLeft.second; i <= bottomRight.second; ++i) {
            same[i * width + topLeft.first] = false;
            same[i * width + bottomRight.first] = false;
        }
    }
}
