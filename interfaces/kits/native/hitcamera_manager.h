#pragma once

#include <unordered_map>
#include <utility>
#include <variant>

#include "hitcamera_service_proxy.h"
#include "udp_piece.h"

namespace OHOS::HITCamera {

    struct PictureHandle {
        int32_t id;
        int32_t size;
        int64_t buffer;
    };

    class CameraManager {
        DISALLOW_COPY_AND_MOVE(CameraManager);

    public:
        static CameraManager* getInstance();

        std::variant<PictureHandle, int> Capture(uint32_t width, uint32_t height);

        int BindRemote(int port);

        std::variant<PictureHandle, int> GetRemote() const;

        void UnbindRemote();

        void Release(PictureHandle handle);

    private:
        sptr<IHITCameraService> mServiceProxy;
        std::unordered_map<int, sptr<Ashmem>> mShmMap;
        int mRemoteSock = 0;
        std::unique_ptr<UdpPiece> mUdpPiece;

        inline explicit CameraManager(sptr<IHITCameraService>&& proxy)
                : mServiceProxy(proxy) {}
    };
}
