#include "Server.h"

esp_err_t uri_handler(httpd_req_t* req){
    Processor* processImage = static_cast<Processor*>(req->user_ctx);
    
    esp_err_t res = ESP_OK;
    size_t _bmp_buf_len;
    uint8_t *_bmp_buf;
    char *part_buf[64];
    static int64_t last_frame = 0;

    if (!last_frame) {
        last_frame = esp_timer_get_time();
    }

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK){
        return res;
    }

    auto fb = processImage->iterate();
    frame2bmp(fb, &_bmp_buf, &_bmp_buf_len);

    size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _bmp_buf_len);

    while (true) {
        fb = processImage->iterate();

        bool bmp_converted = frame2bmp(fb, &_bmp_buf, &_bmp_buf_len);
        if (!bmp_converted) {
            ESP_LOGE(SERVER_TAG, "BMP compression failed");
            res = ESP_FAIL;
        }

        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, _STREAM_LEN);
        }

        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
        }

        if (res == ESP_OK) {
            res = httpd_resp_send_chunk(req, (const char *)_bmp_buf, _bmp_buf_len);
        }

        if (res != ESP_OK)
            break;

        int64_t fr_end = esp_timer_get_time();
        int64_t frame_time = fr_end - last_frame;
        last_frame = fr_end;
        frame_time /= 1000;
        ESP_LOGI(SERVER_TAG, "MBMP: %uKB %ums (%.1ffps) free mem:%u Kb",
                 (uint16_t)(_bmp_buf_len/1024),
                 (uint16_t)frame_time,
                 1000.0 / (uint16_t)frame_time,
                 (uint16_t)(esp_get_free_heap_size()/1000));

        delete _bmp_buf;
    }

    delete _bmp_buf;
    last_frame = 0;
    return res;
}

Server::Server(){
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    stream_httpd = nullptr;

    err = httpd_start(&stream_httpd, &config);
}

esp_err_t Server::add_url(char* name, Processor *processor) {
    httpd_uri_t uri_get = {
            .uri = name,
            .method = HTTP_GET,
            .handler = uri_handler,
            // .handler = &uri_handler,
            .user_ctx = processor,
    };

    if (err == ESP_OK) {
        httpd_register_uri_handler(stream_httpd, &uri_get);
    }

    return err;
}
