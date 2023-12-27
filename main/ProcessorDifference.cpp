//
// Created by Victor Muryn on 29.11.2023.
//

#include <algorithm>
#include "ProcessorDifference.h"

const char* ProcessorDifferenceTAG = "ProcessorDifferenceTAG";

ProcessorDifference::ProcessorDifference(Camera *camera) {
    cam = camera;

    auto fb = cam->take_picture();

    prev = new uint8_t[fb->len];
    std::copy(fb->buf, fb->buf + fb->len, prev);
    
    width = fb->width;
    height = fb->height;
    len = fb->len;

    same = new bool[fb->len];
   
    ESP_LOGI(ProcessorDifferenceTAG, "ProcessorDifference was inited");

    cam->free_picture();
}

camera_fb_t* ProcessorDifference::iterate() {
    auto fb = cam->take_picture();

    if (!fb)
        return nullptr;

    memset(same, true, fb->len);

    // iterate over all image
    for (size_t iter = 0; iter < fb->len; ++iter) {
        // check whether pixel changed
        if ( (fb->buf[iter] > prev[iter] && fb->buf[iter] - prev[iter] > threshold) ||
             (fb->buf[iter] < prev[iter] && prev[iter] - fb->buf[iter] > threshold) ) {

            // set all pixels in radius 'r' as changed
            for (int i = -r; i <= r; ++i) {
                for (int j = -r; j <= r; ++j) {
                    if (iter + i * width + j < len)
                        same[iter + i * width + j] = false;
                    
                }
            }
        }

        prev[iter] = fb->buf[iter];
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

    return fb;
}

ProcessorDifference::~ProcessorDifference() {
    delete[] prev;
    delete[] same;
}

void ProcessorDifference::dfs(camera_fb_t *fb, size_t orPos, bool draw) {
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
