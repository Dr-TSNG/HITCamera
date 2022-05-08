#include <chrono>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include "hitcamera_manager.h"

using namespace OHOS;
using namespace OHOS::HITCamera;

using namespace std;
using namespace chrono;

int main() {
    printf("Begin test\n");
	auto manager = CameraManager::getInstance();
    if (manager == nullptr) {
        printf("Failed to get CameraManager instance\n");
        return -1;
    }
	for (int i = 1; i <= 10; i++) {
		auto t0 = system_clock::now();
		PictureHandle handle = manager->Capture(320, 240);
		if (handle.id == -1) {
			printf("Failed to get image!\n");
			return 0;
		}
		auto t1 = system_clock::now();
		auto duration = duration_cast<microseconds>(t1 - t0);
		auto dd = double(duration.count()) * microseconds::period::num / microseconds::period::den;
		printf("Picture %d cost %lf\n", i, dd);
		manager->Release(handle);
	}
	
	printf("End release\n");
	return 0;
}
