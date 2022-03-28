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

bool try_mmap(int camera) {
    LOGD("Trying mmap");
    // Request buffer
    v4l2_requestbuffers req{};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (-1 == xioctl(camera, VIDIOC_REQBUFS, &req)) {
        LOGE("Failed requesting buffer: %s", strerror(errno));
        return false;
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
        return false;
    }
    buffer = mmap(nullptr, vbuf.length, PROT_READ | PROT_WRITE, MAP_SHARED, camera, vbuf.m.offset);
    LOGD("Query buffer: Lenght: %d Address: %p", vbuf.length, buffer);

    if (-1 == xioctl(camera, VIDIOC_STREAMON, &vbuf.type)) {
        LOGE("Failed starting capture: %s", strerror(errno));
        munmap(buffer, vbuf.length);
        return false;
    }

    if (-1 == ioctl(camera, VIDIOC_QBUF, &vbuf)) {
        LOGE("Failed queueing buffer: %s", strerror(errno));
        return false;
    }

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(camera, &fds);
    timeval tv{};
    tv.tv_sec = 2;

    LOGD("Waiting for frame");
    if (-1 == select(camera + 1, &fds, nullptr, nullptr, &tv)) {
        LOGE("Failed waiting for frame: %s", strerror(errno));
        return false;
    }

    LOGD("Retrieving frame");
    if (-1 == xioctl(camera, VIDIOC_DQBUF, &vbuf)) {
        LOGE("Failed retrieving frame: %s", strerror(errno));
        return false;
    }

    LOGD("Stopping stream");
    munmap(buffer, vbuf.length);
    if (-1 == xioctl(camera, VIDIOC_STREAMOFF, &vbuf.type)) {
        LOGE("Failed when stream off: %s", strerror(errno));
        return false;
    }

    return true;
}

bool try_user_pointer(int camera) {
    LOGD("Trying user pointer");
    // Request buffer
    v4l2_requestbuffers req{};
    req.count = 1;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_USERPTR;
    if (-1 == xioctl(camera, VIDIOC_REQBUFS, &req)) {
        LOGE("Failed requesting buffer: %s", strerror(errno));
        return false;
    }

    // Query buffer
    v4l2_buffer vbuf{};
    void* buffer;
    vbuf = v4l2_buffer();
    vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vbuf.memory = V4L2_MEMORY_USERPTR;
    vbuf.index = 0;

    if (-1 == xioctl(camera, VIDIOC_STREAMON, &vbuf.type)) {
        LOGE("Failed starting capture: %s", strerror(errno));
        return false;
    }

    if (-1 == ioctl(camera, VIDIOC_QBUF, &vbuf)) {
        LOGE("Failed queueing buffer: %s", strerror(errno));
        return false;
    }

    buffer = reinterpret_cast<void*>(vbuf.m.userptr);
    LOGD("Query buffer: Lenght: %d Address: %p", vbuf.length, buffer);

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(camera, &fds);
    timeval tv{};
    tv.tv_sec = 2;

    LOGD("Waiting for frame");
    if (-1 == select(camera + 1, &fds, nullptr, nullptr, &tv)) {
        LOGE("Failed waiting for frame: %s", strerror(errno));
        return false;
    }

    LOGD("Retrieving frame");
    if (-1 == xioctl(camera, VIDIOC_DQBUF, &vbuf)) {
        LOGE("Failed retrieving frame: %s", strerror(errno));
        return false;
    }

    LOGD("Stopping stream");
    munmap(buffer, vbuf.length);
    if (-1 == xioctl(camera, VIDIOC_STREAMOFF, &vbuf.type)) {
        LOGE("Failed when stream off: %s", strerror(errno));
        return false;
    }

    return true;
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

    LOGD("");
    LOGD("Driver: %s", caps.driver);
    LOGD("Card: %s", caps.card);
    LOGD("Bus: %s", caps.bus_info);
    LOGD("Version: %08x", caps.version);
    LOGD("V4L2_CAP_VIDEO_CAPTURE: %d", caps.capabilities & V4L2_CAP_VIDEO_CAPTURE);
    LOGD("V4L2_CAP_VIDEO_CAPTURE_MPLANE: %d", caps.capabilities & V4L2_CAP_VIDEO_CAPTURE_MPLANE);
    LOGD("V4L2_CAP_VIDEO_OUTPUT: %d", caps.capabilities & V4L2_CAP_VIDEO_OUTPUT);
    LOGD("V4L2_CAP_VIDEO_OUTPUT_MPLANE: %d", caps.capabilities & V4L2_CAP_VIDEO_OUTPUT_MPLANE);
    LOGD("V4L2_CAP_VIDEO_M2M: %d", caps.capabilities & V4L2_CAP_VIDEO_M2M);
    LOGD("V4L2_CAP_VIDEO_M2M_MPLANE: %d", caps.capabilities & V4L2_CAP_VIDEO_M2M_MPLANE);
    LOGD("V4L2_CAP_VIDEO_OVERLAY: %d", caps.capabilities & V4L2_CAP_VIDEO_OVERLAY);
    LOGD("V4L2_CAP_VBI_CAPTURE: %d", caps.capabilities & V4L2_CAP_VBI_CAPTURE);
    LOGD("V4L2_CAP_VBI_OUTPUT: %d", caps.capabilities & V4L2_CAP_VBI_OUTPUT);
    LOGD("V4L2_CAP_SLICED_VBI_CAPTURE: %d", caps.capabilities & V4L2_CAP_SLICED_VBI_CAPTURE);
    LOGD("V4L2_CAP_SLICED_VBI_OUTPUT: %d", caps.capabilities & V4L2_CAP_SLICED_VBI_OUTPUT);
    LOGD("V4L2_CAP_RDS_CAPTURE: %d", caps.capabilities & V4L2_CAP_RDS_CAPTURE);
    LOGD("V4L2_CAP_VIDEO_OUTPUT_OVERLAY: %d", caps.capabilities & V4L2_CAP_VIDEO_OUTPUT_OVERLAY);
    LOGD("V4L2_CAP_HW_FREQ_SEEK: %d", caps.capabilities & V4L2_CAP_HW_FREQ_SEEK);
    LOGD("V4L2_CAP_RDS_OUTPUT: %d", caps.capabilities & V4L2_CAP_RDS_OUTPUT);
    LOGD("V4L2_CAP_TUNER: %d", caps.capabilities & V4L2_CAP_TUNER);
    LOGD("V4L2_CAP_AUDIO: %d", caps.capabilities & V4L2_CAP_AUDIO);
    LOGD("V4L2_CAP_RADIO: %d", caps.capabilities & V4L2_CAP_RADIO);
    LOGD("V4L2_CAP_MODULATOR: %d", caps.capabilities & V4L2_CAP_MODULATOR);
    LOGD("V4L2_CAP_SDR_CAPTURE: %d", caps.capabilities & V4L2_CAP_SDR_CAPTURE);
    LOGD("V4L2_CAP_EXT_PIX_FORMAT: %d", caps.capabilities & V4L2_CAP_EXT_PIX_FORMAT);
    LOGD("V4L2_CAP_SDR_OUTPUT: %d", caps.capabilities & V4L2_CAP_SDR_OUTPUT);
    LOGD("V4L2_CAP_READWRITE: %d", caps.capabilities & V4L2_CAP_READWRITE);
    LOGD("V4L2_CAP_ASYNCIO: %d", caps.capabilities & V4L2_CAP_ASYNCIO);
    LOGD("V4L2_CAP_STREAMING: %d", caps.capabilities & V4L2_CAP_STREAMING);
    LOGD("V4L2_CAP_TOUCH: %d", caps.capabilities & V4L2_CAP_TOUCH);
    LOGD("V4L2_CAP_DEVICE_CAPS: %d", caps.capabilities & V4L2_CAP_DEVICE_CAPS);
    LOGD("");

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

    if (try_mmap(camera)) {
        LOGD("Mmap succeed!!!");
    } else {
        LOGD("Mmap failed");
    }
    if (try_user_pointer(camera)) {
        LOGD("User pointer succeed!!!");
    } else {
        LOGD("User pointer failed");
    }

    close(camera);

    return 0;
}
