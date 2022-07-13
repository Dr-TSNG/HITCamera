/*
* Copyright (C) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
import {ErrorCallback, AsyncCallback, Callback} from './basic';
//import {VideoPlayer, UsartPlayer} from './@ohos.multimedia.media'

/**
 * @name hitcamerajs
 * @since 6
 * @sysCap SystemCapability.Multimedia.Rpi4bCamera
 * @import import hitcamera from '@ohos.Multimedia.hitcamerajs';
 * @permission
 * @brief
 */

declare namespace hitcamerajs {
  /*
  * photo data.
  */
  interface PictureHandle {
    id: number;
    size: number;
    buffer: number;

  }
  /**
   * Function describe.
   * @devices
   * @sysCap SystemCapability.Multimedia.hit_camera
   */
  function capture(width: number, height: number): PictureHandle;
  function release(handle: PictureHandle): void;

}//namespace hitcamerajs

export default hitcamerajs;
