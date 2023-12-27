#include "ProcessorAverageBox.h"

const char* AvgBoxTAG = "ProcessorAverageBoxTAG";

ProcessorAverageBox::ProcessorAverageBox(Camera* camera){
    cam = camera;
    for (size_t iter = 0; iter < LAST_FRAMES_NUM; iter++){
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
    for (size_t iter = 0; iter < LAST_FRAMES_NUM; iter++){
        free(prev[iter]);
    }
    free(same);
}

uint8_t ProcessorAverageBox::averageValue(size_t pixIdx){
    uint32_t sum = 0;
    for (size_t iter = 0; iter < LAST_FRAMES_NUM; iter++){
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

    prevToChange = (prevToChange + 1) % LAST_FRAMES_NUM;

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

Rect ProcessorAverageBox::dfs(size_t orPos) {
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

std::vector<Rect> ProcessorAverageBox::merge_rects(std::vector<Rect> rects) {
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

void ProcessorAverageBox::draw(const std::vector<Rect> &rects, camera_fb_t *fb) {
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
