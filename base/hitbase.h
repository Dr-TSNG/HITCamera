#pragma onceonce

#define LOGD(msg, ...) fprintf(stderr, "D: [%s] [line %d]: " msg "\n", __FILE__, __LINE__, __VA_ARGS__)
#define LOGI(msg, ...) fprintf(stderr, "I: [%s] [line %d]: " msg "\n", __FILE__, __LINE__, __VA_ARGS__)
#define LOGE(msg, ...) fprintf(stderr, "E: [%s] [line %d]: " msg "\n", __FILE__, __LINE__, __VA_ARGS__)

namespace OHOS::HITCamera {

    struct PictureHandle {
        size_t size;
        uint8_t* content;

        inline ~PictureHandle() {
            delete[] content;
        }
    };
}
