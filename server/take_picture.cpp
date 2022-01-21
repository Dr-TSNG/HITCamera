#include <mutex>
#include <atomic>
#include <cerrno>
#include <thread>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include "take_picture.h"

namespace OHOS::HITCamera {

    namespace {
        int camera = -1;
        v4l2_buffer vbuf;
        void* buffer;
        std::mutex mtx; // Client invocation lock
        uint32_t widthCached, heightCached;

        std::mutex watchDogMtx;
        std::atomic_bool watchDogCancel = false;
        std::atomic<time_t> timeStamp;

        void WatchDogImpl();

        int xioctl(int fd, int request, void* arg) {
            int r;
            do {
                r = ioctl(fd, request, arg);
                if (-1 == r) perror("ioctl");
            } while (-1 == r && EINTR == errno);
            return r;
        }

        int StartStream() { // Synchronized by Capture() with mtx
            if (camera != -1) return FAILED_OCCUPIED;

            camera = open("/dev/video0", O_RDWR);
            if (camera == -1) {
                LOGE("Failed opening video device: %s", strerror(errno));
                return FAILED_OPEN_DEVICE;
            }

            v4l2_capability caps{};
            if (-1 == xioctl(camera, VIDIOC_QUERYCAP, &caps)) {
                LOGE("Failed querying capabilities: %s", strerror(errno));
                return FAILED_QUERY_CAPABILITIES;
            }

            // Set format
            v4l2_format fmt{};
            fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            fmt.fmt.pix.width = widthCached;
            fmt.fmt.pix.height = heightCached;
            fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
            fmt.fmt.pix.field = V4L2_FIELD_NONE;
            if (-1 == xioctl(camera, VIDIOC_S_FMT, &fmt)) {
                LOGE("Failed setting pixel format: %s", strerror(errno));
                return FAILED_SET_FORMAT;
            }

            // Request buffer
            v4l2_requestbuffers req{};
            req.count = 1;
            req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            req.memory = V4L2_MEMORY_MMAP;
            if (-1 == xioctl(camera, VIDIOC_REQBUFS, &req)) {
                LOGE("Failed requesting buffer: %s", strerror(errno));
                return FAILED_REQUEST_BUFFER;
            }

            // Query buffer
            vbuf = v4l2_buffer();
            vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            vbuf.memory = V4L2_MEMORY_MMAP;
            vbuf.index = 0;
            if (-1 == xioctl(camera, VIDIOC_QUERYBUF, &vbuf)) {
                LOGE("Failed querying buffer: %s", strerror(errno));
                return FAILED_QUERY_BUFFER;
            }
            buffer = mmap(nullptr, vbuf.length, PROT_READ | PROT_WRITE, MAP_SHARED, camera, vbuf.m.offset);
            LOGD("Query buffer: Lenght: %d Address: %p", vbuf.length, buffer);

            if (-1 == xioctl(camera, VIDIOC_STREAMON, &vbuf.type)) {
                LOGE("Failed starting capture: %s", strerror(errno));
                munmap(buffer, vbuf.length);
                return FAILED_START_STREAM;
            }

            return NO_ERROR;
        }

        void StopStream() {// Synchronized by WatchDog with watchDogMtx
            LOGD("Stopping stream");
            munmap(buffer, vbuf.length);
            if (-1 == xioctl(camera, VIDIOC_STREAMOFF, &vbuf.type)) {
                LOGE("Failed when stream off: %s", strerror(errno));
            }
            close(camera);
            camera = -1;
        }

        void WatchDogImpl() {
            watchDogMtx.lock();
            while (true) {
                if (watchDogCancel) break;
                if (time(nullptr) - 5 > timeStamp) break;
                std::this_thread::sleep_for(std::chrono::microseconds(100));
            }
            /*
             * Prevent such situation:
             * ClientThread    WatchDogThread
             *      X          StoppingStream
             *    Enter              |
             *      |                X
             *   Capture <-- Boom!
             */
            bool lockClientMtx = mtx.try_lock();
            StopStream();
            watchDogMtx.unlock();
            if (lockClientMtx) mtx.unlock();
        }

        void CancelStream() {
            watchDogCancel = true;
            watchDogMtx.lock();
            watchDogMtx.unlock();
            watchDogCancel = false;
        }
    }

#define FAIL(reason) mtx.unlock(); return reason
#define CHECK_MAP(func) if (!(func)) {     \
            LOGE("Failed mapping memory"); \
            FAIL(FAILED_MAP);              \
        }

    std::variant<sptr<Ashmem>, int> TakeOnePicture(uint32_t width, uint32_t height) {
        timeStamp = std::numeric_limits<time_t>::max(); // Prevent WatchDog invoke `StopStream` when taking picture
        mtx.lock();

        if (width != widthCached || height != heightCached) { // Pixel not corresponding, cancel stream
            widthCached = width;
            heightCached = height;
            if (camera != -1) CancelStream();
        }

        if (watchDogMtx.try_lock()) { // Stream off, make start
            watchDogMtx.unlock();
            int error = StartStream();
            if (error != NO_ERROR) {
                StopStream();
                FAIL(error);
            }
            std::thread(WatchDogImpl).detach();
        }

        if (-1 == ioctl(camera, VIDIOC_QBUF, &vbuf)) {
            LOGE("Failed queueing buffer: %s", strerror(errno));
            FAIL(FAILED_QUEUE_BUFFER);
        }

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(camera, &fds);
        timeval tv{};
        tv.tv_sec = 2;

        LOGD("Waiting for frame");
        if (-1 == select(camera + 1, &fds, nullptr, nullptr, &tv)) {
            LOGE("Failed waiting for frame: %s", strerror(errno));
            CancelStream();
            FAIL(FAILED_WAIT_FRAME);
        }

        LOGD("Retrieving frame");
        if (-1 == xioctl(camera, VIDIOC_DQBUF, &vbuf)) {
            LOGE("Failed retrieving frame: %s", strerror(errno));
            CancelStream();
            FAIL(FAILED_RETRIEVE_FRAME);
        }

        LOGD("Writing image");
        auto size = static_cast<int32_t>(vbuf.bytesused);
        auto ashmem = Ashmem::CreateAshmem("hit_picture", size);
        CHECK_MAP(ashmem != nullptr)
        CHECK_MAP(ashmem->MapReadAndWriteAshmem())
        CHECK_MAP(ashmem->WriteToAshmem(buffer, size, 0))

        timeStamp = time(nullptr); // Set timeStamp to now
        mtx.unlock();
        return ashmem;
    }

#undef FAIL
#undef CHECK_MAP
}
