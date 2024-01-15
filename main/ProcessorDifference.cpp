//
// Created by Victor Muryn on 29.11.2023.
//
#include "ProcessorDifference.h"

const char* PDBoxTag = "ProcessorDifferenceTAG";

ProcessorDifference::ProcessorDifference(Camera *camera) {
    cam = camera;

    auto fb = cam->take_picture();

    prev = new uint8_t[fb->len];
    std::copy(fb->buf, fb->buf + fb->len, prev);

    width = fb->width;
    height = fb->height;
    len = fb->len;

    same = new bool[fb->len];

    ESP_LOGI(PDBoxTag, "ProcessorDifference was inited");

    cam->free_picture();
}


ProcessorDifference::~ProcessorDifference() {
    delete[] prev;
    delete[] same;
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

    std::vector<Rect> rects;
    // group all changed pixels
    for (size_t i = 0; i < len; i += 1) {
        if (!same[i]) {
            rects.emplace_back( dfs(i) );
            i += r;
        }
    }

    auto merged_rects = merge_rects(rects);

    draw(merged_rects, fb);

    return fb;
}

Rect ProcessorDifference::dfs(size_t orPos) {
    std::queue<size_t> q;
    q.push(orPos);

    Point bottomRight{0, 0};
    Point topLeft{width - 1, height - 1};

    // classical dfs
    while (!q.empty()) {
        auto pos = q.back();
        q.pop();

        if (same[pos]) continue;
        same[pos] = true;

        auto top = pos / width;
        auto left = pos % width;

        // detect top left and bottom right pixels of group
        if (left < topLeft.x) topLeft.x = left;
        if (left > bottomRight.x) bottomRight.x = left;

        if (top < topLeft.y) topLeft.y = top;
        if (top > bottomRight.y) bottomRight.y = top;

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

    return {topLeft, bottomRight};
}

std::vector<Rect> ProcessorDifference::merge_rects(std::vector<Rect> rects) {
    std::sort(rects.begin(), rects.end(), [](Rect a, Rect b) {
        return a.first.x < b.first.x;
    });

    std::vector<Rect> result;

    for (int i = 0; i < rects.size(); ++i) {
        auto rect = rects[i];

        if (rect.merged) continue;

        for (int j = i + 1; j < rects.size(); ++j) {
            if (
                    !rects[j].merged &&
                    // top left point of second rect is in first rect
                    ( (rect.first.x <= rects[j].first.x && rect.first.y <= rects[j].first.y &&
                       rect.second.x >= rects[j].first.x && rect.second.y >= rects[j].first.y
                      ) || (
                              // bottom right point of second rect is in first rect
                              rect.first.x <= rects[j].second.x && rect.first.y <= rects[j].second.y &&
                              rect.second.x >= rects[j].second.x && rect.second.y >= rects[j].second.y
                      ) )
                    ) {
                rect.first.x = std::min(rect.first.x, rects[j].first.x);
                rect.first.y = std::min(rect.first.y, rects[j].first.y);

                rect.second.x = std::max(rect.second.x, rects[j].second.x);
                rect.second.y = std::max(rect.second.y, rects[j].second.y);

                rects[j].merged = true;
            }
        }

        rects[i].merged = true;
        result.emplace_back(rect);
    }

    return result;
}

void ProcessorDifference::draw(const std::vector<Rect> &rects, camera_fb_t *fb) {
    for (auto rect : rects) {
        if ((rect.second.x - rect.first.x) <= r || (rect.second.y - rect.first.y) <= r)
            return;

        for (auto i = rect.first.x; i <= rect.second.x; ++i) {
            fb->buf[rect.first.y * width + i] = 255;
            fb->buf[rect.second.y * width + i] = 255;
        }

        for (auto i = rect.first.y; i <= rect.second.y; ++i) {
            fb->buf[i * width + rect.first.x] = 255;
            fb->buf[i * width + rect.second.x] = 255;
        }
    }
}
