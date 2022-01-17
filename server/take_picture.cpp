#include <mutex>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include "take_picture.h"

namespace OHOS::HITCamera {

    namespace {
        std::mutex mtx;

        int xioctl(int fd, int request, void* arg) {
            int r;
            do {
                r = ioctl(fd, request, arg);
                if (-1 == r) perror("ioctl");
            } while (-1 == r && EINTR == errno);
            return r;
        }
    }

#define FAIL(reason) close(fd); mtx.unlock(); return reason
#define CHECK_MAP(func) if (!(func)) {           \
            LOGE("%s", "Failed mapping memory"); \
            FAIL(FAILED_MAP);                    \
        }

    std::variant<sptr<Ashmem>, int> TakeOnePicture(uint32_t width, uint32_t height) {
        mtx.lock();
        int fd = open("/dev/video0", O_RDWR);
        if (fd == -1) {
            LOGE("Failed opening video device: %s", strerror(errno));
            FAIL(FAILED_OPEN_DEVICE);
        }

        v4l2_capability caps{};
        if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &caps)) {
            LOGE("Failed querying capabilities: %s", strerror(errno));
            FAIL(FAILED_QUERY_CAPABILITIES);
        }

        // Set format
        v4l2_format fmt{};
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = width;
        fmt.fmt.pix.height = height;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
        fmt.fmt.pix.field = V4L2_FIELD_NONE;
        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
            LOGE("Failed setting pixel format: %s", strerror(errno));
            FAIL(FAILED_SET_FORMAT);
        }

        // Request buffer
        v4l2_requestbuffers req{};
        req.count = 1;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;
        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
            LOGE("Failed requesting buffer: %s", strerror(errno));
            FAIL(FAILED_REQUEST_BUFFER);
        }

        // Query buffer
        v4l2_buffer buf{};
        void* buffer;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = 0;
        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)) {
            LOGE("Failed querying buffer: %s", strerror(errno));
            FAIL(FAILED_QUERY_BUFFER);
        }
        buffer = mmap(nullptr, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        LOGD("Query buffer: Lenght: %d Address: %p", buf.length, buffer);

        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = 0;

        if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) {
            LOGE("Failed queueing buffer: %s", strerror(errno));
            FAIL(FAILED_QUEUE_BUFFER);
        }

        if (-1 == xioctl(fd, VIDIOC_STREAMON, &buf.type)) {
            LOGE("Failed starting capture: %s", strerror(errno));
            FAIL(FAILED_START_CAPTURE);
        }

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        timeval tv{};
        tv.tv_sec = 2;

        LOGD("%s", "Waiting for frame");
        if (-1 == select(fd + 1, &fds, nullptr, nullptr, &tv)) {
            LOGE("Failed waiting for frame: %s", strerror(errno));
            FAIL(FAILED_WAIT_FRAME);
        }

        LOGD("%s", "Retrieving frame");
        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
            LOGE("Failed retrieving frame: %s", strerror(errno));
            FAIL(FAILED_RETRIEVE_FRAME);
        }

        LOGD("%s", "Writing image");
        auto size = static_cast<int32_t>(buf.bytesused);
        auto ashmem = Ashmem::CreateAshmem("hit_picture", size);
        CHECK_MAP(ashmem != nullptr)
        CHECK_MAP(ashmem->MapReadAndWriteAshmem())
        CHECK_MAP(ashmem->WriteToAshmem(buffer, size, 0))

        if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &buf.type)) {
            LOGE("Failed when stream off: %s", strerror(errno));
            ashmem->UnmapAshmem();
            ashmem->CloseAshmem();
            FAIL(FAILED_STREAM_OFF);
        }

        close(fd);
        mtx.unlock();
        return ashmem;
    }
}
