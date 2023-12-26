//
// Created by Victor Muryn on 29.11.2023.
//

#include <algorithm>
#include "ProcessorDifference.h"

const char* ProcessorDifferenceTAG = "ProcessorDifferenceTAG";

ProcessorDifference::ProcessorDifference() {
    auto fb = cam.take_picture();

    prev = new uint8_t[fb->len];
    std::copy(fb->buf, fb->buf + fb->len, prev);

    width = fb->width;
    height = fb->height;
    len = fb->len;

    same = new bool[fb->len];

    ESP_LOGI(ProcessorDifferenceTAG, "ProcessorDifference was inited");

    cam.free_picture();
}

camera_fb_t* ProcessorDifference::iterate() {
    auto fb = cam.take_picture();


    if (!fb)
        return nullptr;

    memset(same, true, fb->len);

    for (size_t iter = 0; iter < fb->len; ++iter) {
        if ( (fb->buf[iter] > prev[iter] && fb->buf[iter] - prev[iter] > threshold) ||
             (fb->buf[iter] < prev[iter] && prev[iter] - fb->buf[iter] > threshold) ) {
            for (int i = -r; i <= r; ++i) {
                for (int j = -r; j <= r; ++j) {
                    if (iter + i * width + j < len)
                        same[iter + i * width + j] = false;
                    
                }
            }
        }

        prev[iter] = fb->buf[iter];
    }

    // n^2
    for (size_t i = 0; i < len; i += 1) {
        if (!same[i]) {
            dfs(fb, i, false);
            i += r;
        }
    }

    // n^2
    for (size_t i = 0; i < len; i += 1) {
        if (!same[i]) {
            dfs(fb, i, true);
            i += r;
        }
    }

    cam.free_picture();

    return fb;
}

ProcessorDifference::~ProcessorDifference() {
    delete[] prev;
//    delete cam;
}

void ProcessorDifference::dfs(camera_fb_t *fb, size_t orPos, bool draw) {
    std::queue<size_t> q;
    q.push(orPos);

    std::pair<size_t, size_t> bottomRight{0, 0};
    std::pair<size_t, size_t> topLeft{width - 1, height - 1};

    while (!q.empty()) {
        auto pos = q.back();
        q.pop();

        if (same[pos]) continue;
        same[pos] = true;

        auto top = pos / width;
        auto left = pos % width;

        if (left < topLeft.first) topLeft.first = left;
        if (left > bottomRight.first) bottomRight.first = left;

        if (top < topLeft.second) topLeft.second = top;
        if (top > bottomRight.second) bottomRight.second = top;

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

        // draw white rectangular
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
