/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014 saki t_saki@serenegiant.com
 *
 * File name: UVCPreview.h
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

#ifndef UVCPREVIEW_H_
#define UVCPREVIEW_H_
#include <mutex>
#include <vector>
#include "libUVCCamera.h"
#include <pthread.h>
#include <android/native_window.h>
#include "objectarray.h"
#include <camera/IPreviewCallback.h>

//#pragma interface

typedef uvc_error_t (*convFunc_t)(uvc_frame_t *in, uvc_frame_t *out);

#define PIXEL_FORMAT_RAW 0		// same as PIXEL_FORMAT_YUV
#define PIXEL_FORMAT_YUV 1
#define PIXEL_FORMAT_RGB565 2
#define PIXEL_FORMAT_RGBX 3
#define PIXEL_FORMAT_YUV20SP 4
#define PIXEL_FORMAT_NV21 5		// YVU420SemiPlanar

class UVCCamera;
struct CameraParam;
//// for callback to Java object
//typedef struct {
//	jmethodID onFrame;
//} Fields_iframecallback;

class UVCPreview {
public:
	class Data {
		friend class UVCPreview;
		std::mutex mutex;
		ANativeWindow * mPreviewWindow;
		IPreviewCallback *previewCallback;

	public:
		UVCPreview *uvcPreview;
	public:
		Data();
		~Data();

		int setPreviewCallback(IPreviewCallback *previewCallback);

		int setPreviewWindow(ANativeWindow *preview_window);
		void addPreviewCallbackFrame(std::shared_ptr<uvc_frame_t> frame, int width, int height);
		void preparePreview();
		void drawPreview(uvc_frame_t *frame, convFunc_t convert_func, int pixelBytes);
		void clearPreviewWindow();
	};

private:
	uvc_device_handle_t *mDeviceHandle;
	volatile bool mIsRunning;

	Data &mData;

	int vendorId, productId;
	int requestWidth, requestHeight, requestMode, requestMinFps, requestMaxFps;
	int frameWidth, frameWidthRate, frameHeight;
	int frameMode;
	//size_t frameBytes;
	int previewFormat;
	//size_t previewBytes;
	const CameraParam *defaultCameraParam;

	pthread_t preview_thread;
	pthread_mutex_t preview_mutex;
	pthread_cond_t preview_sync;
	ObjectArray<uvc_frame_t *> previewFrames;

	//std::mutex save_mutex;
	//pthread_cond_t save_sync;
	bool mIsSaveSnapshot;

	bool _flip_enable = false;
	bool _rotate_enable = false;
	int _rotate_type = 0;

	unsigned char* _rotateData = nullptr;
	unsigned char* _flipData = nullptr;

    //// 相机直接支持的图像回调（抓图）,回调过程为:
    //// 1. 预览图像送图像队列(实时队列)(captureQueu + capture_mutex + capture_sync),队列中只保存最后一幅图像
    //// 2. 抓图线程(capture_thread)从队列(captureQueu)中取到图像
    //// 3. 将抓到的图像渲染到mCaptureWindow上(如果用户指定了mCaptureWindow)
    //// 4. 调用mFrameCallbackFunc对图像进行转换
    //// 5. 将转后的图像通过mFrameCallbackObj回调到Java App空间
    //volatile bool mIsCapturing;
    //// 有CaptureWindow和mFrameCallbackObj是当前支持的两种回调方式:
    ////  1. mCaptureWindow将抓到的图像画到其上面, mCaptureWindows固定接受RGBX格式的图像数据
    ////  2. mFrameCallbackObj 将抓到的图像调到用户空间
    //// 两者可并存
    //ANativeWindow *mCaptureWindow;  // only RGBX
    //pthread_t capture_thread;
    //// 图像队列(实时队列),预览图送入该队列后,才可以被capture_thread线程取到,然后进行mFrameCallbackFunc进行格式转换
    //pthread_mutex_t capture_mutex;
    //pthread_cond_t capture_sync;
    //uvc_frame_t *captureQueu;			// keep latest frame
    //// 格式转换:预览图转回调图像(抓图)
    //convFunc_t mFrameCallbackFunc;
    //// 回调时图像的格式(setFrameCallback中的pixel_format参数), 取值范围见callbackPixelFormatChanged函数
    //int mPixelFormat;
    //// 回调时图像尺寸
    //size_t callbackPixelBytes;
    //// Java回调对象
    //jobject mFrameCallbackObj;
    //Fields_iframecallback iframecallback_fields;
//
	void clearDisplay();
	static void uvc_preview_frame_callback(uvc_frame_t *frame, void *vptr_args);
	void addPreviewFrame(uvc_frame_t *frame);
	uvc_frame_t *waitPreviewFrame();
	void clearPreviewFrame();
	static void *preview_thread_func(void *vptr_args);
	int prepare_preview(uvc_stream_ctrl_t *ctrl);
	void do_preview(uvc_stream_ctrl_t *ctrl);
	uvc_frame_t *draw_preview_one(uvc_frame_t *frame, ANativeWindow **window, convFunc_t func, int pixelBytes);
//
//
    //void addCaptureFrame(uvc_frame_t *frame);
    //uvc_frame_t *waitCaptureFrame();
    //void clearCaptureFrame();
    //static void *capture_thread_func(void *vptr_args);
    //void do_capture(JNIEnv *env);
    //void do_capture_surface(JNIEnv *env);
    //void do_capture_idle_loop(JNIEnv *env);
    //void do_capture_callback(JNIEnv *env, uvc_frame_t *frame);
    //void callbackPixelFormatChanged();

    inline size_t frameBytes() const;
public:
	UVCPreview(Data &data, uvc_device_handle_t *devh, int vendorId, int productId);
	~UVCPreview();

	inline const bool isRunning() const;
	int setPreviewSize(int width, int height, int mode);
	//int setPreviewDisplay(ANativeWindow *preview_window);
	//int setFrameCallback(JNIEnv *env, jobject frame_callback_obj, int pixel_format);
	int startPreview();
	int stopPreview();
	//inline bool isCapturing() const;
	//int setCaptureDisplay(ANativeWindow *capture_window);

	void setImageRotateType(bool rotate_enable, int rotate_type);
	void setImageFlip(bool flip_enable);
	void captureSnapshot(const char* path);
};

#endif /* UVCPREVIEW_H_ */
