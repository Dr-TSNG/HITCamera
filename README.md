## HITCameraService

### 编译

1. 将仓库文件放入 `foundation/multimedia/hit_camea_standard` 中

2. 修改`productdefine/common/products/rk3568.json`，加入一行

   ```
   "multimedia:multimedia_hit_camera_standard":{},
   ```

3. 修改 `base/startup/init_lite/services/etc/init.cfg`，在指定位置加入一行

   ```
   "mkdir /data/misc/cameraserver 0700 cameraserver cameraserver",
   + "chown hitcameraserver hitcameraserver /dev/video0",
   ```

4. 修改`base/startup/init_lite/services/etc/passwd`，加入一行

   ```
   hitcameraserver:x:1048:1048:::/bin/false
   ```

### Native 接口

#### GN 依赖

```
include_dirs = [
    "//foundation/multimedia/hit_camera_standard/interfaces/kits/native",
    "//foundation/multimedia/hit_camera_standard/services/include",
]

deps = [
   "//utils/native/base:utils",
     "//foundation/multimedia/hit_camera_standard/interfaces/kits/native:hit_camera_client"
]

external_deps = [
    "hiviewdfx_hilog_native:libhilog",
    "ipc:ipc_core",
    "samgr_standard:samgr_proxy",
]
```

#### C++ 接口

头文件：`hitcamera_manager.h`

`PictureHandle`：描述了一个存放在内存中的图片，起始地址位于 `buffer`，长度为 `size`

```cpp
struct PictureHandle {
    int32_t id;
    int32_t size;
    int64_t buffer;
}
```

`CameraManager`：操作模块

```cpp
class CameraManager {
    static CameraManager* getInstance();
    std::variant<PictureHandle, int> Capture(uint32_t width, uint32_t height);
    int BindRemote(int port);
    std::variant<PictureHandle, int> GetRemote() const;
    void UnbindRemote();
    void Release(PictureHandle handle);
}
```

`getInstance()`：获取相机实例，成功返回一个 `CameraManager` 实例，失败返回 `nullptr`

`Capture(uint32_t width, uint32_t height)`：以 width × height 的像素拍一张照片，成功返回一个 `PictureHandle`，失败返回一个整形错误码（具体定义在 `hitbase.h` 下的 `Codes` 中）

`BindRemote(int port)`：绑定端口 `port` 监听远程发送的图片，返回一个整形错误码（无错误返回 ERR_OK）

`GetRemote()`：从监听端口远程获取一张照片，成功返回一个 `PictureHandle`，失败返回一个整形错误码

`Release(PictureHandle handle)`：释放一张照片的内存

#### 注意事项

处理完一张照片后，一定要调用 `Release` 释放 `PictureHandle`，否则会造成内存泄露

具体事例参考 `test/test.cpp`、`test/udp_test.cpp`

### JS 接口

#### 接口

```js
declare namespace hitcamerajs {
  /*
   * photo data.
   */
  interface PictureHandle {
    id: number;
    size: number;
    buffer: number;
  }
  function Capture(width: number, height: number): PictureHandle;
  function Release(handle: PictureHandle): void;
}
```

#### 使用

```js
import cm from '@ohos.Multimedia.hitcamerajs';

var ph = cm.capture(width, height);
...
cm.Release(ph);
```
