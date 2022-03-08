#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#define LOGD(fmt, ...) fprintf(stderr, "[D] [%s:%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)
#define LOGE(fmt, ...) fprintf(stderr, "[E] [%s:%d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__)

int xioctl(int fd, int request, void* arg) {
    int r;
    do {
        r = ioctl(fd, request, arg);
        if (-1 == r) perror("ioctl");
    } while (-1 == r && EINTR == errno);
    return r;
}

int main() {
    auto camera = open("/dev/video0", O_RDWR);

    if (camera == -1) {
        LOGE("Failed opening video device: %s", strerror(errno));
        return 0;
    }

    v4l2_capability caps{};
    if (-1 == xioctl(camera, VIDIOC_QUERYCAP, &caps)) {
        LOGE("Failed querying capabilities: %s", strerror(errno));
        return 0;
    }

    // Set format
    v4l2_format fmt{};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 320;
    fmt.fmt.pix.height = 240;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;
    if (-1 == xioctl(camera, VIDIOC_S_FMT, &fmt)) {
        LOGE("Failed setting pixel format: %s", strerror(errno));
        return 0;
    }

    // Request buffer
    v4l2_requestbuffers req{};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (-1 == xioctl(camera, VIDIOC_REQBUFS, &req)) {
        LOGE("Failed requesting buffer: %s", strerror(errno));
        return 0;
    }

    // Query buffer
    v4l2_buffer vbuf{};
    void* buffer;
    vbuf = v4l2_buffer();
    vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vbuf.memory = V4L2_MEMORY_MMAP;
    vbuf.index = 0;
    if (-1 == xioctl(camera, VIDIOC_QUERYBUF, &vbuf)) {
        LOGE("Failed querying buffer: %s", strerror(errno));
        return 0;
    }
    buffer = mmap(nullptr, vbuf.length, PROT_READ | PROT_WRITE, MAP_SHARED, camera, vbuf.m.offset);
    LOGD("Query buffer: Lenght: %d Address: %p", vbuf.length, buffer);

    if (-1 == xioctl(camera, VIDIOC_STREAMON, &vbuf.type)) {
        LOGE("Failed starting capture: %s", strerror(errno));
        munmap(buffer, vbuf.length);
        return 0;
    }

    if (-1 == ioctl(camera, VIDIOC_QBUF, &vbuf)) {
        LOGE("Failed queueing buffer: %s", strerror(errno));
        return 0;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(camera, &fds);
    timeval tv{};
    tv.tv_sec = 2;

    LOGD("Waiting for frame");
    if (-1 == select(camera + 1, &fds, nullptr, nullptr, &tv)) {
        LOGE("Failed waiting for frame: %s", strerror(errno));
        return 0;
    }

    LOGD("Retrieving frame");
    if (-1 == xioctl(camera, VIDIOC_DQBUF, &vbuf)) {
        LOGE("Failed retrieving frame: %s", strerror(errno));
        return 0;
    }

    LOGD("Stopping stream");
    munmap(buffer, vbuf.length);
    if (-1 == xioctl(camera, VIDIOC_STREAMOFF, &vbuf.type)) {
        LOGE("Failed when stream off: %s", strerror(errno));
    }
    close(camera);

    return 0;
}
