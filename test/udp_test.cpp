#include <chrono>
#include <cstdio>
#include "hitcamera_manager.h"

using namespace OHOS;
using namespace OHOS::HITCamera;

using namespace std;
using namespace chrono;

static void WriteToFile(const PictureHandle& handle, const char* file) {
    FILE* fp = fopen(file, "wb");
    fwrite((const void*) handle.buffer, 1, handle.size, fp);
    fclose(fp);
}

int main() {
    printf("Begin test\n");
    auto manager = CameraManager::getInstance();
    if (manager == nullptr) {
        printf("Failed to get CameraManager instance\n");
        return -1;
    }
    if (manager->BindRemote(5678) != ERR_OK) {
        printf("Failed to bind remote\n");
        return -1;
    }
    for (int i = 1; i <= 3; i++) {
        auto t0 = system_clock::now();
        auto res = manager->GetRemote();
        if (auto handle = std::get_if<PictureHandle>(&res)) {
            auto t1 = system_clock::now();
            auto duration = duration_cast<microseconds>(t1 - t0);
            auto dd = double(duration.count()) * microseconds::period::num / microseconds::period::den;
            printf("Picture %d cost %lf\n", i, dd);
            if (i == 3) WriteToFile(*handle, "/data/test/output.jpg");
            manager->Release(*handle);
        } else {
            int err = std::get<int>(res);
            printf("Failed to capture picture, error code: %d\n", err);
            return 0;
        }
    }
    manager->UnbindRemote();

    printf("End release\n");
    return 0;
}
