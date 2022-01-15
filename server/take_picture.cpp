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

    sptr<PictureHandle> TakeOnePicture() {
#define FAIL() close(fd); mtx.unlock(); return nullptr

        mtx.lock();
        int fd = open("/dev/video0", O_RDWR);
        if (fd == -1) {
            LOGE("Failed opening video device: %s", strerror(errno));
            FAIL();
        }

        v4l2_capability caps = {0};
        if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &caps)) {
            LOGE("Failed querying capabilities: %s", strerror(errno));
            FAIL();
        }

        // Set format
        v4l2_format fmt = {0};
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width = 320;
        fmt.fmt.pix.height = 240;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
        fmt.fmt.pix.field = V4L2_FIELD_NONE;
        if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt)) {
            LOGE("Failed setting pixel format: %s", strerror(errno));
            FAIL();
        }

        // Request buffer
        v4l2_requestbuffers req = {0};
        req.count = 1;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;
        if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
            LOGE("Failed requesting buffer: %s", strerror(errno));
            FAIL();
        }

        // Query buffer
        v4l2_buffer buf = {0};
        void* buffer;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = 0;
        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf)) {
            LOGE("Failed querying buffer: %s", strerror(errno));
            FAIL();
        }
        buffer = mmap(nullptr, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
        LOGD("Query buffer: Lenght: %d Address: %p", buf.length, buffer);

        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = 0;

        if (-1 == ioctl(fd, VIDIOC_QBUF, &buf)) {
            LOGE("Failed queueing buffer: %s", strerror(errno));
            FAIL();
        }

        if (-1 == xioctl(fd, VIDIOC_STREAMON, &buf.type)) {
            LOGE("Failed starting capture: %s", strerror(errno));
            FAIL();
        }

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(fd, &fds);
        timeval tv = {0};
        tv.tv_sec = 2;

        LOGD("%s", "Waiting for frame");
        if (-1 == select(fd + 1, &fds, nullptr, nullptr, &tv)) {
            LOGE("Failed waiting for frame: %s", strerror(errno));
            FAIL();
        }

        LOGD("%s", "Retrieving frame");
        if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
            LOGE("Failed retrieving frame: %s", strerror(errno));
            FAIL();
        }

        LOGD("%s", "Writing image");
        sptr<PictureHandle> handle = new PictureHandle{buf.bytesused, new uint8_t[buf.bytesused]};
        memcpy(handle->content, buffer, handle->size);

        if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &buf.type)) {
            LOGE("Failed when stream off: %s", strerror(errno));
            FAIL();
        }

        close(fd);
        mtx.unlock();
        return handle;
    }
}
