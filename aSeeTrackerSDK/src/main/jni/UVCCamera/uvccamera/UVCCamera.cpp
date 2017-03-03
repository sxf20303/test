/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014 saki t_saki@serenegiant.com
 *
 * File name: UVCCamera.cpp
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

#include <stdlib.h>
#include <linux/time.h>
#include <unistd.h>
#include <string.h>
#include "UVCCamera.h"
#include "libuvc_internal.h"
#include "aSeeDeviceCtrl.h"

#define	LOCAL_DEBUG 0

UVCContext::UVCContext()
{
	mUsbFs = "";
	mContext = nullptr;
}

UVCContext::~UVCContext() {
	reset();
}

uvc_error_t UVCContext::reset(const char *usbFs) {
	uvc_error_t result = UVC_SUCCESS;
	usbFs = (usbFs == nullptr ? "" : usbFs);

	LOGI("mUsbFs = <%s>, usbFs = <%s>, IsEqual = %s", mUsbFs.c_str(), usbFs, (mUsbFs == usbFs? "true" : "false"));
	// 不管usbFs与mUsbFs是否相等，均需重新初始化mContext
	if (mContext) {
		uvc_exit(mContext);
	}
	mContext = nullptr;

	mUsbFs = usbFs;
	if (mUsbFs != "") {
		result = uvc_init2(&mContext, NULL, mUsbFs.c_str());
	}

	return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// UVCCamera
UVCCamera::UVCCamera()
:	mFd(0),
	//mUsbFs(NULL),
	//mContext(NULL),
	mDevice(NULL),
	mDeviceHandle(NULL) //,
	//mPreview(NULL)
{
	ENTER();
	EXIT();
}

UVCCamera::~UVCCamera() {
	ENTER();
	release();
//	if (mContext) {
//		uvc_exit(mContext);
//		mContext = NULL;
//	}
//	if (mUsbFs) {
//		free(mUsbFs);
//		mUsbFs = NULL;
//	}
	mContext.reset();
	EXIT();
}

int UVCCamera::connect(int vid, int pid, int fd, const char *usbfs) {    //  打开  Camera 摄像头
	ENTER();

	LOGE("=== vid = %d,pid=%d,fd=%d,usbfs=%s\n",vid,pid,fd,usbfs);

	uvc_error_t result = UVC_ERROR_BUSY;
	if (!mDeviceHandle && fd) {
		result = mContext.reset(usbfs);
		if (UNLIKELY(result < 0)) {
			LOGD("failed to init libuvc");
			RETURN(result, int);
		}

		fd = dup(fd);
		result = uvc_find_device2(mContext, &mDevice, vid, pid, NULL, fd);
		if (LIKELY(!result)) {
			result = uvc_open(mDevice, &mDeviceHandle);    //  打开 设备
			if (LIKELY(!result)) {
#if LOCAL_DEBUG
				uvc_print_diag(mDeviceHandle, stderr);
#endif

				brightness(mDeviceHandle, 2);
				//exposure(mDeviceHandle, 338);

				mFd = fd;
				UVCPreview *p = new UVCPreview(preview, mDeviceHandle, vid, pid);   //  UVC  Camera
				assert(preview.uvcPreview == p);
			} else {
				LOGE("could not open camera:err=%d", result);
				uvc_unref_device(mDevice);
				mDevice = NULL;		//  SAFE_DELETE(mDevice);
				mDeviceHandle = NULL;
				close(fd);
			}

		} else {
			LOGE("could not find camera:err=%d", result);
			close(fd);
		}
	} else {
		LOGW("camera is already opened. you should release first");
	}
	RETURN(result, int);
}

int UVCCamera::release() {
	ENTER();
	stopPreview();
	if (LIKELY(mDeviceHandle)) {
		SAFE_DELETE(preview.uvcPreview);
		uvc_close(mDeviceHandle);
		mDeviceHandle = NULL;
	}
	if (LIKELY(mDevice)) {
		uvc_unref_device(mDevice);
		mDevice = NULL;
	}
	if (LIKELY(mFd)) {
		close(mFd);
		mFd = 0;
	}
	RETURN(0, int);
}

int UVCCamera::setPreviewSize(int width, int height, int mode) {    //  mode 0 :YUYV  1 :MJPEG
	ENTER();
	int result = EXIT_FAILURE;
	if (preview.uvcPreview) {//mPreview) {
		result = preview.uvcPreview->setPreviewSize(width, height, mode);   // 设置 UVCCamera 分辨率 包括 帧率
	}
	RETURN(result, int);
}

int UVCCamera::setPreviewDisplay(ANativeWindow *preview_window) {
	ENTER();
	int result = EXIT_FAILURE;
	result = preview.setPreviewWindow(preview_window);
	//if (preview.uvcPreview) {//mPreview) {
	//	result = preview.uvcPreview->setPreviewDisplay(preview_window);
	//}
	RETURN(result, int);
}

//int UVCCamera::setFrameCallback(JNIEnv *env, jobject frame_callback_obj, int pixel_format) {
//	ENTER();
//	int result = EXIT_FAILURE;
//	if (mPreview) {
//		result = mPreview->setFrameCallback(env, frame_callback_obj, pixel_format);
//	}
//	RETURN(result, int);
//}

int UVCCamera::startPreview() {
	ENTER();

	int result = EXIT_FAILURE;
	if (preview.uvcPreview) {//mPreview) {
		result = preview.uvcPreview->startPreview();     //  开启 预览
	}
	RETURN(result, int);
}

int UVCCamera::stopPreview() {
	ENTER();
	LOGI("uvcPreview = %p", preview.uvcPreview);
	if (preview.uvcPreview) {//mPreview) {
		preview.uvcPreview->stopPreview();
	}
	RETURN(0, int);
}

int UVCCamera::setRotateType(bool rotate_enable, int rotate_type) {
	ENTER();
	preview.uvcPreview->setImageRotateType(rotate_enable, rotate_type);
	RETURN(0, int);
}

int UVCCamera::setFlip(bool filp_enable) {
	ENTER();
	preview.uvcPreview->setImageFlip(filp_enable);
	RETURN(0, int);
}

int UVCCamera::captureSnapshot(const char* path) {
	ENTER();
	preview.uvcPreview->captureSnapshot(path);
	RETURN(0, int);
}

int UVCCamera::setLight(int value) {
	ENTER();
	if (mDeviceHandle) {
		if (ASEE_DEVICE_SUCCESS == brightness(mDeviceHandle, value)) {
			RETURN(0, int);
		}
	}
	RETURN(1, int);
}

int UVCCamera::setExposure(int value) {
	ENTER();
	if (mDeviceHandle) {
		if (ASEE_DEVICE_SUCCESS == exposure(mDeviceHandle, value)) {
			RETURN(0, int);
		}
	}
	RETURN(1, int);
}

//int UVCCamera::setCaptureDisplay(ANativeWindow *capture_window) {
//	ENTER();
//	int result = EXIT_FAILURE;
//	if (mPreview) {
//		result = mPreview->setCaptureDisplay(capture_window);
//	}
//	RETURN(result, int);
//}

void UVCCamera::setPreviewCallback(IPreviewCallback *cb) {
	ENTER();
	preview.setPreviewCallback(cb);
	EXIT();
}

// /****************************************************************/   //  by  flyyang
//int UVCCamera::setRegisterValue() {
//	/*ENTER();
//	uvc_device_handle_t *mdev;
//		uvc_error_t result = UVC_ERROR_BUSY;
//		if (!mDeviceHandle && fd) {
//			if (mUsbFs)
//				free(mUsbFs);
//			mUsbFs = strdup(usbfs);
//			if (!mContext) {
//				result = uvc_init2(&mContext, NULL, mUsbFs);
//				if (UNLIKELY(result < 0)) {
//					LOGD("failed to init libuvc");
//					RETURN(result, int);
//				}
//			}
//			fd = dup(fd);
//			result = uvc_find_device2(mContext, &mDevice, vid, pid, NULL, fd);
//			if (LIKELY(!result)) {
//				result = uvc_open(mDevice, &mDeviceHandle);    //  打开 设备
//				if (LIKELY(!result)) {
//	#if LOCAL_DEBUG
//					uvc_print_diag(mDeviceHandle, stderr);
//	#endif
//					mFd = fd;
//					mPreview = new UVCPreview(mDeviceHandle);   //  UVC  Camera
//				} else {
//					LOGE("could not open camera:err=%d", result);
//					uvc_unref_device(mDevice);
//					mDevice = NULL;		//  SAFE_DELETE(mDevice);
//					mDeviceHandle = NULL;
//					close(fd);
//				}
//
//			} else {
//				LOGE("could not find camera:err=%d", result);
//				close(fd);
//			}
//		} else {
//			LOGW("camera is already opened. you should release first");
//		}
//		RETURN(result, int);
//*/
//	/*ENTER();
//	int result = EXIT_FAILURE;
//	if (mDeviceHandle) {
//
//
//		LOGE("setRegisterValue\n");
//		//LOGE("readRegisterValue------------- starting");
//		//readRegisterValue(mDeviceHandle);              //  读取寄存器数据
//
//		//LOGE("setRegisterValue------------- starting");
//		mySetRegisterValue(mDeviceHandle,32,13576,0);      //  写寄存器 数据
//		//   可能 需要  在这里完成 写寄存器
//
//		return 0;
//	}
//	RETURN(result, int);*/
//}
// /****************************************************************/
