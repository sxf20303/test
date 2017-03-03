/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014 saki t_saki@serenegiant.com
 *
 * File name: UVCCamera.h
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * All files in the folder are under this Apache License, Version 2.0.
 * Files in the jni/libjpeg, jni/libusb and jin/libuvc folder may have a different license, see the respective files.
*/

//#pragma interface

#ifndef UVCCAMERA_H_
#define UVCCAMERA_H_

#include "libUVCCamera.h"
#include <pthread.h>
#include <string>
#include <android/native_window.h>
#include "UVCPreview.h"
#include "Extension.h"
#include <camera/ICamera.h>

class UVCContext {
	std::string mUsbFs;
	uvc_context_t *mContext;

public:
	UVCContext();
	~UVCContext();

	UVCContext(const UVCContext &) = delete;
	UVCContext &operator=(const UVCContext &) = delete;

	uvc_error_t reset(const char *usbFs = nullptr);
	uvc_context_t *getContext() const {
		return mContext;
	}
	operator uvc_context_t *() const {
		return mContext;
	}
};
class UVCCamera: public ICamera {
	//char *mUsbFs;
//	estring mUsbFs;
//	uvc_context_t *mContext;
	UVCContext mContext;
	int mFd;
	uvc_device_t *mDevice;
	uvc_device_handle_t *mDeviceHandle;
//	UVCPreview *mPreview;
	UVCPreview::Data preview;
public:
	UVCCamera();
	virtual ~UVCCamera();

	int connect(int vid, int pid, int fd, const char *usbfs);
	int release();
	int setPreviewSize(int width, int height, int mode);
	int setPreviewDisplay(ANativeWindow *preview_window);
	//int setFrameCallback(JNIEnv *env, jobject frame_callback_obj, int pixel_format);
	int startPreview();
	int stopPreview();
	//int setCaptureDisplay(ANativeWindow *capture_window);

	int setRotateType(bool rotate_enable, int rotate_type);
	int setFlip(bool filp_enable);
	int captureSnapshot(const char* path);

	int setLight(int value);
	int setExposure(int value);

public:
	// 相机应保证该函数返回后,则在原有callback上的调用也已运行完并停止.
	virtual void setPreviewCallback(IPreviewCallback *cb) override;
};

#endif /* UVCCAMERA_H_ */
