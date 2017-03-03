/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014 saki t_saki@serenegiant.com
 *
 * File name: UVCPreview.cpp
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
#include "utilbase.h"
#include "UVCPreview.h"
#include "libuvc_internal.h"
#include "bmp.h"

#define	LOCAL_DEBUG 0

// 这个也是native_window.h中支持的格式,但未在其头文件中定义
#define WINDOW_FORMAT_RGB_888 3

#define MAX_FRAME 4
#define PREVIEW_PIXEL_BYTES 3	// 3: RGB, 4: RGBA/RGBX
#define DEFAULT_PREVIEW_FORMAT WINDOW_FORMAT_RGB_888
#define DEFAULT_PREVIEW_MODE 0      // YUV
//#if 0
//#define DEFAULT_PREVIEW_WIDTH 960     // 背夹相机的的图像,从相机上来时,其报告的图像宽度
//#define DEFAULT_PREVIEW_HEIGHT 1080
//#define DEFAULT_PREVIEW_FPS 60
//#endif

typedef void (*FCopyFrame)(uint8_t *dst, const uint8_t *src, int width, int height, int stride);
void beijia_copyFrame(uint8_t *dst, const uint8_t *src, int width, int height, int stride)
{
	// 背夹需要左右倒一下，否则图像不是镜像的
	for (int h = 0; h < height; h++) {
		const uint8_t *srcLine = src + stride * h;
		const uint8_t *srcLineEnd = srcLine + width;
		uint8_t       *dstIt = dst + stride * h + width - 1;
		//int left = 0, right = width - 1;
		for (const uint8_t *it = srcLine ; it < srcLineEnd; it++, dstIt--) {
			*dstIt = *it;
		}
	}
}

void yuy2_to_nv21(uint8_t *dst, const uint8_t *src, int width, int height, int stride)
{
    int lineWidth = stride * 2;
	for (int h = 0; h < height; h++) {
		const uint8_t *lineEnd = src + lineWidth;
		for (; src < lineEnd; ((src+=2), dst++)) {
			*dst = *src;
		}
        src = lineEnd;
        dst += (stride - width);
	}
}

// 背夹相机: vendorId: 0x5a9, productId: 0x581
struct CameraParam {
	int vendorId;
	int productId;
	int previewWidth;
	int previewHeight;
	int previewMinFps;
    int previewMaxFps;

    // 背夹相机的实际宽度,应为其返回宽度的2倍
    int frameWidthRate;

	FCopyFrame copyFrameData;
};
static const CameraParam normalCamera {    0,     0, 1920, 1080, 1, 30, 1, yuy2_to_nv21};
static const CameraParam beijiaCamera {0x5a9, 0x581,  960, 1080, 1, 60, 2, beijia_copyFrame};
static const CameraParam VRCamera {0x5a9, 0x4301,  200, 400, 1, 120, 2, beijia_copyFrame};

static const CameraParam &_default(int vendorId, int productId) {
	if (vendorId == beijiaCamera.vendorId && productId == beijiaCamera.productId) {
		LOGI("背夹");
		// 背夹摄像头
		return beijiaCamera;
	} else if(vendorId == VRCamera.vendorId && productId == VRCamera.productId){
		LOGI("Gear VR");
		return VRCamera;
	} else{
		LOGI("普通摄像头");
		return normalCamera;
	}
}

static uvc_frame_format adjustFrameFormat(const CameraParam *defaultCameraParam, uvc_frame_format frameFormat) {
    return (defaultCameraParam == &beijiaCamera ? UVC_FRAME_FORMAT_RAW8 : frameFormat);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
UVCPreview::Data::Data()
{
	uvcPreview = nullptr;
	mPreviewWindow = nullptr;
	previewCallback = nullptr;
}
UVCPreview::Data::~Data() {
	delete uvcPreview;
	uvcPreview = nullptr;

	if (mPreviewWindow) {
		ANativeWindow_release(mPreviewWindow);
		mPreviewWindow = nullptr;
	}
}

int UVCPreview::Data::setPreviewWindow(ANativeWindow *preview_window) {
	std::unique_lock<std::mutex> lock(mutex);
	if (mPreviewWindow != preview_window) {
		if (mPreviewWindow)
			ANativeWindow_release(mPreviewWindow);
		mPreviewWindow = preview_window;
		if (LIKELY(mPreviewWindow) && uvcPreview) {
			ANativeWindow_setBuffersGeometry(mPreviewWindow,
				uvcPreview->frameWidth*uvcPreview->frameWidthRate, uvcPreview->frameHeight, uvcPreview->previewFormat);
		}
	}
}
int UVCPreview::Data::setPreviewCallback(IPreviewCallback *previewCallback) {
	std::unique_lock<std::mutex> lock(mutex);
	this->previewCallback = previewCallback;
}

void UVCPreview::Data::addPreviewCallbackFrame(std::shared_ptr<uvc_frame_t> frame, int width, int height) {
	//LOGI("%d x %d => previewCallback(%p)", width, height, previewCallback);
	std::unique_lock<std::mutex> lock(mutex);
	if (previewCallback != nullptr) {
		std::shared_ptr<uint8_t> buf((uint8_t *)frame->data, [frame](uint8_t *){   //   YYYYYYYYYYYYYYYYY
			const_cast<std::shared_ptr<uvc_frame_t> &>(frame).reset();
		});

		// begin modify by dapeng.wang 跳帧，解决资源占用率问题
		static uint32_t skip = 0;
		//if(skip++ % 2 == 0)
		{
			previewCallback->onPreviewImage(buf, frame->data_bytes, width, height);
		}

		// end modify by dapeng.wang
	}
}

void UVCPreview::Data::preparePreview() {
	std::unique_lock<std::mutex> lock(mutex);
	assert(uvcPreview);
	if (LIKELY(mPreviewWindow)) {
        LOGI("ANativeWindow_BufferGeometry: width = %d, height = %d", uvcPreview->frameWidth*uvcPreview->frameWidthRate, uvcPreview->frameHeight);
		ANativeWindow_setBuffersGeometry(mPreviewWindow,
			uvcPreview->frameWidth*uvcPreview->frameWidthRate, uvcPreview->frameHeight, uvcPreview->previewFormat);
	}
}

void UVCPreview::Data::clearPreviewWindow() {
	std::unique_lock<std::mutex> lock(mutex);

	if (LIKELY(mPreviewWindow)) {
		ANativeWindow_Buffer buffer;
		if (LIKELY(ANativeWindow_lock(mPreviewWindow, &buffer, NULL) == 0)) {
			uint8_t *dest = (uint8_t *)buffer.bits;
			const size_t bytes = buffer.width * PREVIEW_PIXEL_BYTES;
			const int stride = buffer.stride * PREVIEW_PIXEL_BYTES;
            LOGI("line bytes: %d, stride: %d", (int)bytes, (int)stride);
			for (int i = 0; i < buffer.height; i++) {
				memset(dest, 0, bytes);
				dest += stride;
			}
			ANativeWindow_unlockAndPost(mPreviewWindow);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
UVCPreview::UVCPreview(Data &data, uvc_device_handle_t *devh, int vendorId, int productId):
		mDeviceHandle(devh), mIsRunning(false), mData(data)
	//, mCaptureWindow(NULL)
	//requestWidth(DEFAULT_PREVIEW_WIDTH),
	//requestHeight(DEFAULT_PREVIEW_HEIGHT),
	//requestFps(DEFAULT_PREVIEW_FPS),
	//requestMode(DEFAULT_PREVIEW_MODE),
	//frameWidth(DEFAULT_PREVIEW_WIDTH),
	//frameHeight(DEFAULT_PREVIEW_HEIGHT),
	//frameBytes(DEFAULT_PREVIEW_WIDTH * DEFAULT_PREVIEW_HEIGHT * 2),	// YUYV
	//frameMode(0),
	//previewBytes(DEFAULT_PREVIEW_WIDTH * 2 * DEFAULT_PREVIEW_HEIGHT * PREVIEW_PIXEL_BYTES),  //   //  应该 这个地方也要改   by flyyang  *2 标示宽增加一倍
	//previewFormat(WINDOW_FORMAT_RGBA_8888),
	//mIsRunning(false),
	//mIsCapturing(false),
	//captureQueu(NULL),
	//mFrameCallbackObj(NULL),
	//mFrameCallbackFunc(NULL),
	//callbackPixelBytes(2)
{
	ENTER();
	mData.uvcPreview = this;
	pthread_cond_init(&preview_sync, NULL);
	pthread_mutex_init(&preview_mutex, NULL);
    //
    //pthread_cond_init(&capture_sync, NULL);
    //pthread_mutex_init(&capture_mutex, NULL);

	preview_thread = 0;

	const CameraParam &def = _default(vendorId, productId);
    defaultCameraParam = &def;
	this->vendorId = vendorId;
	this->productId = productId;
	requestWidth = def.previewWidth;
	requestHeight = def.previewHeight;
	requestMinFps = def.previewMinFps;
    requestMaxFps = def.previewMaxFps;
	requestMode = DEFAULT_PREVIEW_MODE;
	frameWidth = def.previewWidth;
    frameWidthRate = def.frameWidthRate;
	frameHeight = def.previewHeight;
	//frameBytes = def.previewWidth * def.previewHeight * 2;  // YUYV
	frameMode = DEFAULT_PREVIEW_MODE;
	//previewBytes(DEFAULT_PREVIEW_WIDTH * 2 * DEFAULT_PREVIEW_HEIGHT * PREVIEW_PIXEL_BYTES),  //   //  应该 这个地方也要改   by flyyang  *2 标示宽增加一倍
	previewFormat = DEFAULT_PREVIEW_FORMAT;

	EXIT();
}

UVCPreview::~UVCPreview() {
	ENTER();
	mData.uvcPreview = nullptr;
	//if (mPreviewWindow) ANativeWindow_release(mPreviewWindow);
	//mPreviewWindow = NULL;
	//if (mCaptureWindow) ANativeWindow_release(mCaptureWindow);
	//mCaptureWindow = NULL;
	clearPreviewFrame();
	//clearCaptureFrame();
	pthread_mutex_destroy(&preview_mutex);
	pthread_cond_destroy(&preview_sync);
	//pthread_mutex_destroy(&capture_mutex);
	//pthread_cond_destroy(&capture_sync);
	EXIT();
}

inline const bool UVCPreview::isRunning() const {
	return mIsRunning;
}

int UVCPreview::setPreviewSize(int width, int height, int mode) {
	ENTER();
	
	LOGI("requestWidth = %d, requestHeight = %d, width = %d, height = %d",
         requestWidth, requestHeight, width, height);
	int result = 0;
	if ((requestWidth != (width/frameWidthRate)) || (requestHeight != height) || (requestMode != mode)) {
        requestWidth = width / frameWidthRate;
        requestHeight = height;
        requestMode = mode;

        uvc_stream_ctrl_t ctrl;

        LOGI("uvc_get_stream_ctrl_format_size_fps = [%d, %d]", requestMinFps, requestMaxFps);
        result = uvc_get_stream_ctrl_format_size_fps(mDeviceHandle, &ctrl,
             !requestMode ? UVC_FRAME_FORMAT_YUYV : UVC_FRAME_FORMAT_MJPEG,
             requestWidth, requestHeight, requestMinFps, requestMaxFps);
    }
	
	RETURN(result, int);
}

//int UVCPreview::setPreviewDisplay(ANativeWindow *preview_window) {
//	ENTER();
//	pthread_mutex_lock(&preview_mutex);
//	{
//		if (mPreviewWindow != preview_window) {
//			if (mPreviewWindow)
//				ANativeWindow_release(mPreviewWindow);
//			mPreviewWindow = preview_window;
//			if (LIKELY(mPreviewWindow)) {
//				ANativeWindow_setBuffersGeometry(mPreviewWindow,
//					frameWidth*frameWidthRate, frameHeight, previewFormat);             //   这里      by flyyang
//			}
//		}
//	}
//	pthread_mutex_unlock(&preview_mutex);
//	RETURN(0, int);
//}

void UVCPreview::clearDisplay() {
	ENTER();

	//ANativeWindow_Buffer buffer;
	//pthread_mutex_lock(&capture_mutex);
	//{
	//	if (LIKELY(mCaptureWindow)) {
	//		if (LIKELY(ANativeWindow_lock(mCaptureWindow, &buffer, NULL) == 0)) {
	//			uint8_t *dest = (uint8_t *)buffer.bits;
	//			const size_t bytes = buffer.width * PREVIEW_PIXEL_BYTES;
	//			const int stride = buffer.stride * PREVIEW_PIXEL_BYTES;
	//			for (int i = 0; i < buffer.height; i++) {
	//				memset(dest, 0, bytes);
	//				dest += stride;
	//			}
	//			ANativeWindow_unlockAndPost(mCaptureWindow);
	//		}
	//	}
	//}
	//pthread_mutex_unlock(&capture_mutex);

	//pthread_mutex_lock(&preview_mutex);
	//{
	//	if (LIKELY(mPreviewWindow)) {
	//		if (LIKELY(ANativeWindow_lock(mPreviewWindow, &buffer, NULL) == 0)) {
	//			uint8_t *dest = (uint8_t *)buffer.bits;
	//			const size_t bytes = buffer.width * PREVIEW_PIXEL_BYTES;
	//			const int stride = buffer.stride * PREVIEW_PIXEL_BYTES;
	//			for (int i = 0; i < buffer.height; i++) {
	//				memset(dest, 0, bytes);
	//				dest += stride;
	//			}
	//			ANativeWindow_unlockAndPost(mPreviewWindow);
	//		}
	//	}
	//}
	//pthread_mutex_unlock(&preview_mutex);
	mData.clearPreviewWindow();
	EXIT();
}

// 预览线程
void *UVCPreview::preview_thread_func(void *vptr_args) {
	int result;

	ENTER();
	UVCPreview *preview = reinterpret_cast<UVCPreview *>(vptr_args);
	if (LIKELY(preview)) {
		uvc_stream_ctrl_t ctrl;
		result = preview->prepare_preview(&ctrl);
		if (LIKELY(!result)) {
			preview->do_preview(&ctrl);
		}
	}
	PRE_EXIT();
	pthread_exit(NULL);
}

int UVCPreview::startPreview() {
	ENTER();

	//int result = EXIT_FAILURE;
	if (!isRunning()) {
		pthread_mutex_lock(&preview_mutex);
		{
			//if (LIKELY(mData.mPreviewWindow) || LIKELY(mData.previewCallback)) {
			int	result = pthread_create(&preview_thread, NULL, preview_thread_func, (void *)this);    //   开启 预览  线程
			mIsRunning = (result == 0);
			LOGI("preview_thread (%ld)", preview_thread);
			//}
				if (UNLIKELY(!mIsRunning)) {
					preview_thread = 0;
					//LOGW("UVCCamera::window does not exist/already running/could not create thread etc.");
					LOGE("pthread_cond_signal");
					// 不知道失败的话，发送信号做什么
					pthread_cond_signal(&preview_sync);     //  发信号
				}
		}
		pthread_mutex_unlock(&preview_mutex);
		//if (UNLIKELY(result != EXIT_SUCCESS)) {
		//	LOGW("UVCCamera::window does not exist/already running/could not create thread etc.");
		//	mIsRunning = false;
		//	pthread_mutex_lock(&preview_mutex);
		//	{
		//		LOGE("pthread_cond_signal");
		//		pthread_cond_signal(&preview_sync);     //  发信号
		//	}
		//	pthread_mutex_unlock(&preview_mutex);
		//}
	}
	int result = (isRunning() ? EXIT_SUCCESS : EXIT_FAILURE);
	LOGI("UVCPreview isRunning? %s, result: %d(EXIT_SUCCESS:%d, EXIT_FAILURE:%d)",
		 (isRunning() ? "true" : "false"), result, EXIT_SUCCESS, EXIT_FAILURE);

	RETURN(result, int);
}

int UVCPreview::stopPreview() {
	ENTER();
	bool b = isRunning();
	LOGI("isRunning = %s", (b ? "true" : "false"));
	if (LIKELY(b)) {
		mIsRunning = false;
		pthread_cond_signal(&preview_sync);
		//pthread_cond_signal(&capture_sync);
		//if (pthread_join(capture_thread, NULL) != EXIT_SUCCESS) {
		//	LOGW("UVCPreview::terminate capture thread: pthread_join failed");
		//}
		LOGI("stop preview thread(%ld)", preview_thread);
		if (pthread_join(preview_thread, NULL) != EXIT_SUCCESS) {
			LOGW("UVCPreview::terminate preview thread: pthread_join failed");
		}
		preview_thread = 0;
		LOGI("clearDisplay");
		clearDisplay();
	}
	LOGI("clearPreviewFrame");
	clearPreviewFrame();
	//pthread_mutex_lock(&preview_mutex);
	//if (mPreviewWindow) {
	//	ANativeWindow_release(mPreviewWindow);
	//	mPreviewWindow = NULL;
	//}
	//pthread_mutex_unlock(&preview_mutex);
	LOGI("clearPreviewWindow");
	mData.clearPreviewWindow();
    //clearCaptureFrame();
	//pthread_mutex_lock(&capture_mutex);
	//if (mCaptureWindow) {
	//	ANativeWindow_release(mCaptureWindow);
	//	mCaptureWindow = NULL;
	//}
	//pthread_mutex_unlock(&capture_mutex);
	RETURN(0, int);
}

void UVCPreview::setImageRotateType(bool rotate_enable, int rotate_type) {
	_rotate_enable = rotate_enable;
	_rotate_type = rotate_type;
}

void UVCPreview::setImageFlip(bool flip_enable) {
	_flip_enable = flip_enable;
}

void UVCPreview::captureSnapshot(const char* path) {
	mIsSaveSnapshot = true;
}

//**********************************************************************
// uvc读取到的数据回调给UVCPreview
//**********************************************************************
void UVCPreview::uvc_preview_frame_callback(uvc_frame_t *frame, void *vptr_args) {
	UVCPreview *preview = reinterpret_cast<UVCPreview *>(vptr_args);
	if (UNLIKELY(!preview->isRunning() || !frame || !frame->frame_format || !frame->data || !frame->data_bytes)) {
		return;
	}
	if (UNLIKELY(
		((frame->frame_format != UVC_FRAME_FORMAT_MJPEG) && (frame->actual_bytes < preview->frameBytes()))
		|| (frame->width != preview->frameWidth) || (frame->height != preview->frameHeight) )) {

#if LOCAL_DEBUG
		LOGD("broken frame!:format=%d,actual_bytes=%d/%d(%d,%d/%d,%d)",
			frame->frame_format, frame->actual_bytes, preview->frameBytes,
			frame->width, frame->height, preview->frameWidth, preview->frameHeight);
#endif
		return;
	}
	if (LIKELY(preview->isRunning())) {
		int data_bytes = frame->width * preview->frameWidthRate * frame->height * 3 / 2;
		if (data_bytes < frame->data_bytes) {
			data_bytes = frame->data_bytes;
		}
		uvc_frame_t *copy = uvc_allocate_frame(data_bytes);
		if (UNLIKELY(!copy)) {
#if LOCAL_DEBUG
			LOGE("uvc_callback:unable to allocate duplicate frame!");
#endif
			return;
		}
		//LOGI("frame[0] {expect_data_bytes:%d, copy->data_bytes:%d, copy->actual_bytes:%d}", data_bytes, (int)copy->data_bytes, (int)copy->actual_bytes);
		uvc_error_t ret = uvc_duplicate_frame_header(frame, copy);
		if (UNLIKELY(ret)) {
			uvc_free_frame(copy);
			return;
		}
        int width = copy->width * preview->frameWidthRate;
        preview->defaultCameraParam->copyFrameData((uint8_t *)copy->data, (const uint8_t *)frame->data, width, copy->height, width);
		copy->actual_bytes = width * copy->height;
        //LOGI("%d/%d x %d, format = %d, %d/%d", (int)copy->width, width, (int)copy->height, (int)copy->frame_format, (int)copy->actual_bytes, (int)copy->data_bytes);
		copy->frame_format = UVC_FRAME_FORMAT_RAW8;
		// 未使用部分清零
		if (copy->data_bytes > copy->actual_bytes) {
			memset((char *)copy->data + copy->actual_bytes, 128, copy->data_bytes - copy->actual_bytes);
		}
		//LOGI("frame[1] {expect_data_bytes:%d, copy->data_bytes:%d, copy->actual_bytes:%d}", data_bytes, (int)copy->data_bytes, (int)copy->actual_bytes);
		preview->addPreviewFrame(copy);
	}
}

void UVCPreview::addPreviewFrame(uvc_frame_t *frame) {

	pthread_mutex_lock(&preview_mutex);
	if (isRunning() && (previewFrames.size() < MAX_FRAME)) {
		previewFrames.put(frame);
		frame = NULL;
		pthread_cond_signal(&preview_sync);
	}
	pthread_mutex_unlock(&preview_mutex);
	if (frame) {
		uvc_free_frame(frame);
	}
}

uvc_frame_t *UVCPreview::waitPreviewFrame() {
	uvc_frame_t *frame = NULL;
	pthread_mutex_lock(&preview_mutex);
	{
		if (!previewFrames.size()) {
			pthread_cond_wait(&preview_sync, &preview_mutex);    //  阻塞
		}
		if (LIKELY(isRunning() && previewFrames.size() > 0)) {
			frame = previewFrames.remove(0);
		}
	}
	pthread_mutex_unlock(&preview_mutex);
	return frame;
}

void UVCPreview::clearPreviewFrame() {
	pthread_mutex_lock(&preview_mutex);
	{
		for (int i = 0; i < previewFrames.size(); i++)
			uvc_free_frame(previewFrames[i]);
		previewFrames.clear();
	}
	pthread_mutex_unlock(&preview_mutex);
}

size_t UVCPreview::frameBytes() const {
    return frameWidth * frameHeight * (!requestMode ? 2 : 4);
}

int UVCPreview::prepare_preview(uvc_stream_ctrl_t *ctrl) {  //   -------   //     417
	uvc_error_t result;

	ENTER();
	//LOGI("2 uvc_get_stream_ctrl_format_size_fps = 60");
	LOGI("2 requestWidth = %d:requestHeight=%d", requestWidth,requestHeight);
	LOGI("2 cframeSize=(%d,%d)@%s", frameWidth, frameHeight, (!requestMode ? "YUYV" : "MJPEG"));
	result = uvc_get_stream_ctrl_format_size_fps(mDeviceHandle, ctrl,
		!requestMode ? UVC_FRAME_FORMAT_YUYV : UVC_FRAME_FORMAT_MJPEG,
		requestWidth, requestHeight, requestMinFps, requestMaxFps);

	/*result = uvc_get_stream_ctrl_format_size(mDeviceHandle, ctrl,
			!requestMode ? UVC_FRAME_FORMAT_YUYV : UVC_FRAME_FORMAT_MJPEG,
			requestWidth, requestHeight, 60);*/

	LOGI("[EYEDEA] --   result_1:[%d]", result);
	if (LIKELY(!result)) {
#if LOCAL_DEBUG
		uvc_print_stream_ctrl(ctrl, stderr);
#endif
		uvc_frame_desc_t *frame_desc;
		result = uvc_get_frame_desc(mDeviceHandle, ctrl, &frame_desc);
		if (LIKELY(!result)) {
			frameWidth = frame_desc->wWidth;
			frameHeight = frame_desc->wHeight;
			mData.preparePreview();
			//LOGI("frameSize=(%d,%d)@%s, mPreviewWindow: %p", frameWidth, frameHeight, (!requestMode ? "YUYV" : "MJPEG"), mPreviewWindow);
			//pthread_mutex_lock(&preview_mutex);
			//if (LIKELY(mPreviewWindow)) {
			//	ANativeWindow_setBuffersGeometry(mPreviewWindow,
			//		frameWidth*frameWidthRate, frameHeight, previewFormat);     //  by flyyang
			//}
			//pthread_mutex_unlock(&preview_mutex);
		} else {
			frameWidth = requestWidth;
			frameHeight = requestHeight;
		}
		frameMode = requestMode;
		//frameBytes = frameWidth * frameHeight * (!requestMode ? 2 : 4);
		//previewBytes = frameWidth* 2 * frameHeight * PREVIEW_PIXEL_BYTES;
	} else {
		LOGI("could not negotiate with camera:err=%d", result);
	}
	RETURN(result, int);
}


void RotateImage(unsigned char* pDstData, const unsigned char* pSrcData,
		int nWidth, int nHeight, int nChannel, bool bClockWise = true) {
	// 旋转90度
	int nLineSize = nWidth * nChannel;
	int n = 0;

	if (bClockWise) {
		// 顺时针旋转
		for (int j = 0; j < nWidth; j++) {
			for (int i = nHeight; i > 0; i--) {
				memcpy( &pDstData[n],
						&pSrcData[nLineSize * (i - 1) + j * nChannel - nChannel],
						nChannel);
				n += nChannel;
			}
		}
	} else {
		// 逆时针的旋转的算法
		for (int j = nWidth; j > 0; j--) {
			for (int i = 0; i < nHeight; i++) {
				memcpy( &pDstData[n],
						&pSrcData[nLineSize * i + j * nChannel - nChannel],
						nChannel);
				n += nChannel;
			}
		}
	}
}

void RotateImage_180(unsigned char* pDstData, const unsigned char* pSrcData,
		int nWidth, int nHeight, int nChannel) {
	// 旋转180度
	int nLineSize = nWidth * nChannel;
	int n = 0;

	for (int i = 0; i < nHeight; ++i) {
		for (int j = 0; j < nWidth; j++) {
			memcpy( &pDstData[n],
					&pSrcData[(nHeight - i) * nLineSize - (j + 1) * nChannel],
					nChannel);
			n += nChannel;
		}
	}
}

void FlipImage_Vertical(unsigned char* pDstData, const unsigned char* pSrcData,
		int nWidth, int nHeight, int nChannel) {
	// 上下翻转
	int nLineSize = nWidth * nChannel;

	for (int i = 0; i < nHeight / 2; ++i) {
		memcpy(&pDstData[i * nLineSize], &pSrcData[(nHeight - i - 1) * nLineSize], nLineSize);
		memcpy(&pDstData[(nHeight - i - 1) * nLineSize], &pSrcData[i * nLineSize], nLineSize);
	}
}

void UVCPreview::do_preview(uvc_stream_ctrl_t *ctrl) {
	ENTER();

	//uvc_frame_t *frame = NULL;
	auto deleter = [](uvc_frame_t *p) { if (p) uvc_free_frame(p); };
	uvc_frame_t *frame_mjpeg = NULL;
	uvc_error_t result = uvc_start_iso_streaming(
		mDeviceHandle, ctrl, uvc_preview_frame_callback, (void *)this);

	if (LIKELY(!result)) {
		clearPreviewFrame();
		//pthread_create(&capture_thread, NULL, capture_thread_func, (void *)this);

		LOGI("Streaming...");
		if (frameMode) {    //  当 获取的是mjpeg图像格式时 会先将     mjpeg图像格式转换成yuyv的格式 然后再将  yuyv的转换成 rgbx
			LOGI("Streaming...");
			// MJPEG mode
			while (LIKELY(isRunning())) {
				auto frame_mjpeg = std::shared_ptr<uvc_frame_t>(waitPreviewFrame(), deleter);
				if (LIKELY(frame_mjpeg.get())) {
					auto frame = std::shared_ptr<uvc_frame_t>(uvc_allocate_frame(frame_mjpeg->width * frame_mjpeg->height * 2), deleter);
					result = uvc_mjpeg2yuyv(frame_mjpeg.get(), frame.get());   // MJPEG => yuyv
					//uvc_free_frame(frame_mjpeg);
					frame_mjpeg.reset();
					if (LIKELY(!result)) {
						mData.drawPreview(frame.get(), uvc_any2rgbx, 4);
                        //addCaptureFrame(frame);
					}// else {
					//	uvc_free_frame(frame);
					//}
				}
			}
		} else {
			LOGI("Streaming YUYV");
			// yuvyv mode
			while (LIKELY(isRunning())) {    //  YUYV 帧      //   循环 读取帧数据
				//LOGE("geting waitPreviewFrame ! ");
				//   获取 当前 数据 帧  等待 捕获 一帧 数据
				//LOGI("waitPreviewFrame...");
				auto frame = std::shared_ptr<uvc_frame_t>(waitPreviewFrame(), deleter);

				if (LIKELY(frame.get())) {
                    frame->width *= frameWidthRate;
                    frame->frame_format = adjustFrameFormat(defaultCameraParam, frame->frame_format);
					mData.addPreviewCallbackFrame(frame, frame->width, frame->height);
					//frame = draw_preview_one(frame, &mPreviewWindow, uvc_any2rgb, PREVIEW_PIXEL_BYTES);    //  在这里 切换转换 格式   * 2 宽度扩大一倍
					//mData.drawPreview(frame.get(), uvc_any2rgb, PREVIEW_PIXEL_BYTES);

                    //uvc_free_frame(frame); //addCaptureFrame(frame);

					if(mIsSaveSnapshot)
					{
//						if(nullptr == _rotateData)
//						{
//							_rotateData = new unsigned char[frame->width * frame->height * 3];
//						}
//
//						if(nullptr == _flipData)
//						{
//							_flipData = new unsigned char[frame->width * frame->height * 3];
//						}
//						RotateImage_180(_rotateData, (unsigned char*)frame->data, frame->width, frame->height, 8);
//						FlipImage_Vertical(_flipData, _rotateData, frame->width, frame->height, 8);

						//LOGE("SnapshotBmp-----------------1!");
						//SnapshotBmp("", _flipData, frame->width, frame->height, 8);
						SnapshotBmp("", (unsigned char*)frame->data, frame->width, frame->height, 8);
						mIsSaveSnapshot = false;
					}
				}
			}
			LOGI("EXIT WHILE");
		}
		//pthread_cond_signal(&capture_sync);
		LOGI("uvc_stop_streaming...");
		uvc_stop_streaming(mDeviceHandle);
		LOGI("uvc_stop_streaming... OK");
	} else {
		uvc_perror(result, "failed start_streaming");
	}

	EXIT();
}


static void copyFrame(const uint8_t *src, uint8_t *dest, const int width, int height, const int stride_src, const int stride_dest) {
	const int h8 = height % 8;
	for (int i = 0; i < h8; i++) {
		memcpy(dest, src, width);
		dest += stride_dest; src += stride_src;
	}
	for (int i = 0; i < height; i += 8) {
		memcpy(dest, src, width);
		dest += stride_dest; src += stride_src;
		memcpy(dest, src, width);
		dest += stride_dest; src += stride_src;
		memcpy(dest, src, width);
		dest += stride_dest; src += stride_src;
		memcpy(dest, src, width);
		dest += stride_dest; src += stride_src;
		memcpy(dest, src, width);
		dest += stride_dest; src += stride_src;
		memcpy(dest, src, width);
		dest += stride_dest; src += stride_src;
		memcpy(dest, src, width);
		dest += stride_dest; src += stride_src;
		memcpy(dest, src, width);
		dest += stride_dest; src += stride_src;
	}
}


// transfer specific frame data to the Surface(ANativeWindow)
int copyToSurface(uvc_frame_t *frame, ANativeWindow *window) {
	// ENTER();
	int result = 0;
	if (LIKELY(window)) {
		ANativeWindow_Buffer buffer;
		if (LIKELY(ANativeWindow_lock(window, &buffer, NULL) == 0)) {
			// source = frame data
			const uint8_t *src = (uint8_t *)frame->data;
			const int src_w = frame->width * PREVIEW_PIXEL_BYTES;
			const int src_step = frame->width * PREVIEW_PIXEL_BYTES;
			// destination = Surface(ANativeWindow)
			uint8_t *dest = (uint8_t *)buffer.bits;
			const int dest_w = buffer.width * PREVIEW_PIXEL_BYTES;
			const int dest_step = buffer.stride * PREVIEW_PIXEL_BYTES;
			// use lower transfer bytes
			const int w = src_w < dest_w ? src_w : dest_w;
			// use lower height
			const int h = frame->height < buffer.height ? frame->height : buffer.height;
			// transfer from frame data to the Surface
			copyFrame(src, dest, w, h, src_step, dest_step);
			ANativeWindow_unlockAndPost(window);
		} else {
			result = -1;
		}
	} else {
		result = -1;
	}
	return result; //RETURN(result, int);
}

// changed to return original frame instead of returning converted frame even if convert_func is not null.
void UVCPreview::Data::drawPreview(uvc_frame_t *frame, convFunc_t convert_func, int pixelBytes) {
	// ENTER();
	{
		std::unique_lock<std::mutex> lock(this->mutex);
		if (this->mPreviewWindow == nullptr) {
			return ;
		}
		if (!convert_func) {
			// 无须进行格式转换，直接绘制
			copyToSurface(frame, mPreviewWindow);
			return ;
		}
	}

	// 进行格式需转换
	auto converted = std::shared_ptr<uvc_frame_t>(uvc_allocate_frame(frame->width * frame->height * pixelBytes),
			[](uvc_frame_t *p){if (p) uvc_free_frame(p);});
	if (LIKELY(converted.get())) {
		//    对图像数据 进行转换  转换后结果放 到 converted 内
		uvc_error_t b = convert_func(frame, converted.get());
		if (!b) {
			std::unique_lock<std::mutex> lock(this->mutex);
			if (mPreviewWindow) {
				copyToSurface(converted.get(), mPreviewWindow);
			}
		} else {
			LOGE("failed converting");
		}
	}
}

////======================================================================
//// Capture
////======================================================================
//inline bool UVCPreview::isCapturing() const {
//	return mIsCapturing;
//}
//
//int UVCPreview::setFrameCallback(JNIEnv *env, jobject frame_callback_obj, int pixel_format) {
//
//    LOGE("pixel_format = %d\n",pixel_format);
//    ENTER();
//    pthread_mutex_lock(&capture_mutex);
//    {
//        if (isRunning() && isCapturing()) {
//            mIsCapturing = false;
//            if (mFrameCallbackObj) {
//                pthread_cond_signal(&capture_sync);
//                pthread_cond_wait(&capture_sync, &capture_mutex);	// wait finishing capturing
//            }
//        }
//        if (!env->IsSameObject(mFrameCallbackObj, frame_callback_obj))	{
//            iframecallback_fields.onFrame = NULL;
//            if (mFrameCallbackObj) {
//                env->DeleteGlobalRef(mFrameCallbackObj);
//            }
//            mFrameCallbackObj = frame_callback_obj;
//            if (frame_callback_obj) {
//                // get method IDs of Java object for callback
//                jclass clazz = env->GetObjectClass(frame_callback_obj);
//                if (LIKELY(clazz)) {
//                    iframecallback_fields.onFrame = env->GetMethodID(clazz,
//                                                                     "onFrame",	"(Ljava/nio/ByteBuffer;)V");
//                } else {
//                    LOGW("failed to get object class");
//                }
//                env->ExceptionClear();
//                if (!iframecallback_fields.onFrame) {
//                    LOGE("Can't find IFrameCallback#onFrame");
//                    env->DeleteGlobalRef(frame_callback_obj);
//                    mFrameCallbackObj = frame_callback_obj = NULL;
//                }
//            }
//        }
//        if (frame_callback_obj) {
//            mPixelFormat = pixel_format;
//            callbackPixelFormatChanged();
//        }
//    }
//    pthread_mutex_unlock(&capture_mutex);
//    RETURN(0, int);
//}
//
//void UVCPreview::callbackPixelFormatChanged() {        //             --------------------------------------------             //    CASE-1
//    LOGD("callbackPixelFormatChanged = Starting\n");
//    LOGI("pixel_format = %d\n",mPixelFormat);
//    mFrameCallbackFunc = NULL;
//    const size_t sz = requestWidth * requestHeight;
//    switch (mPixelFormat) {
//    case PIXEL_FORMAT_RAW:                              //   0
//        LOGI("PIXEL_FORMAT_RAW:");
//        callbackPixelBytes = sz * 2;
//        break;
//    case PIXEL_FORMAT_YUV:
//        LOGI("PIXEL_FORMAT_YUV:");
//        callbackPixelBytes = sz * 2;
//        break;
//    case PIXEL_FORMAT_RGB565:
//        LOGI("PIXEL_FORMAT_RGB565:");
//        mFrameCallbackFunc = uvc_any2rgb565;
//        callbackPixelBytes = sz * 2;
//        break;
//    case PIXEL_FORMAT_RGBX:
//        LOGI("PIXEL_FORMAT_RGBX:");
//        mFrameCallbackFunc = uvc_any2rgbx;
//        callbackPixelBytes = sz * 4;
//        break;
//    case PIXEL_FORMAT_YUV20SP:
//        LOGI("PIXEL_FORMAT_YUV20SP:");
//        mFrameCallbackFunc = uvc_yuyv2yuv420SP;
//        callbackPixelBytes = (sz * 3) / 2;
//        break;
//    case PIXEL_FORMAT_NV21:
//        LOGI("PIXEL_FORMAT_NV21:");
//        mFrameCallbackFunc = uvc_yuyv2iyuv420SP;
//        callbackPixelBytes = (sz * 3) / 2;
//        break;
//    }
//}
//
//
//int UVCPreview::setCaptureDisplay(ANativeWindow *capture_window) {
//	ENTER();
//	pthread_mutex_lock(&capture_mutex);
//	{
//		if (isRunning() && isCapturing()) {
//			mIsCapturing = false;
//			if (mCaptureWindow) {
//				pthread_cond_signal(&capture_sync);
//				pthread_cond_wait(&capture_sync, &capture_mutex);	// wait finishing capturing
//			}
//		}
//		if (mCaptureWindow != capture_window) {
//			// release current Surface if already assigned.
//			if (UNLIKELY(mCaptureWindow)) {
//				ANativeWindow_release(mCaptureWindow);
//			}
//			mCaptureWindow = capture_window;
//			// if you use Surface came from MediaCodec#createInputSurface
//			// you could not change window format at least when you use
//			// ANativeWindow_lock / ANativeWindow_unlockAndPost
//			// to write frame data to the Surface...
//			// So we need check here.
//			if (mCaptureWindow) {
//				int32_t window_format = ANativeWindow_getFormat(mCaptureWindow);
//				if ((window_format != WINDOW_FORMAT_RGB_565)
//					&& (previewFormat == WINDOW_FORMAT_RGB_565)) {
//					LOGE("window format mismatch, cancelled movie capturing.");
//					ANativeWindow_release(mCaptureWindow);
//					mCaptureWindow = NULL;
//				}
//			}
//		}
//	}
//	pthread_mutex_unlock(&capture_mutex);
//	RETURN(0, int);
//}
//
//void UVCPreview::addCaptureFrame(uvc_frame_t *frame) {    //  增加 一帧 数据  这个函数在 获取帧中调用   504 hang
//	pthread_mutex_lock(&capture_mutex);
//	if (LIKELY(isRunning())) {
//		// keep only latest one
//		if (captureQueu) {
//			uvc_free_frame(captureQueu);
//		}
//		captureQueu = frame;
//		pthread_cond_broadcast(&capture_sync);
//	}
//	pthread_mutex_unlock(&capture_mutex);
//}
//
///**
// * get frame data for capturing, if not exist, block and wait
// */
//uvc_frame_t *UVCPreview::waitCaptureFrame() {       //   返回 一帧 图像 数据
//	uvc_frame_t *frame = NULL;
//	pthread_mutex_lock(&capture_mutex);
//	{
//		if (!captureQueu) {
//			pthread_cond_wait(&capture_sync, &capture_mutex);
//		}
//		if (LIKELY(isRunning() && captureQueu)) {
//			frame = captureQueu;
//			captureQueu = NULL;
//		}
//	}
//	pthread_mutex_unlock(&capture_mutex);
//	return frame;
//}
//
///**
// * clear drame data for capturing
// */
//void UVCPreview::clearCaptureFrame() {
//	pthread_mutex_unlock(&capture_mutex);
//	{
//		if (captureQueu)
//			uvc_free_frame(captureQueu);
//		captureQueu = NULL;
//	}
//	pthread_mutex_unlock(&capture_mutex);
//}
//
////======================================================================
///*
// * thread function
// * @param vptr_args pointer to UVCPreview instance
// */
//// static
//void *UVCPreview::capture_thread_func(void *vptr_args) {
//	int result;
//	LOGE("capture_thread_func\n");
//	ENTER();
//	UVCPreview *preview = reinterpret_cast<UVCPreview *>(vptr_args);
//	if (LIKELY(preview)) {
//		JavaVM *vm = getVM();
//		JNIEnv *env;
//		// attach to JavaVM
//		vm->AttachCurrentThread(&env, NULL);
//		preview->do_capture(env);	// never return until finish previewing
//		// detach from JavaVM
//		vm->DetachCurrentThread();
//		MARK("DetachCurrentThread");
//	}
//	PRE_EXIT();
//	pthread_exit(NULL);
//}
//
///**
// * the actual function for capturing
// */
//void UVCPreview::do_capture(JNIEnv *env) {
//
//	ENTER();
//
//	LOGE("do_capture\n");
//	clearCaptureFrame();
//	callbackPixelFormatChanged();
//	for (; isRunning() ;) {
//		mIsCapturing = true;
//		if (mCaptureWindow) {
//			do_capture_surface(env);
//		} else {
//			do_capture_idle_loop(env);
//		}
//		pthread_cond_broadcast(&capture_sync);
//	}	// end of for (; isRunning() ;)
//	EXIT();
//}
//
//void UVCPreview::do_capture_idle_loop(JNIEnv *env) {
//	ENTER();
//
//	for (; isRunning() && isCapturing() ;) {
//		do_capture_callback(env, waitCaptureFrame());
//	}
//
//	EXIT();
//}
//
///**
// * write frame data to Surface for capturing
// */
//void UVCPreview::do_capture_surface(JNIEnv *env) {   //  TODO     frame data is always YUYV format.
//	ENTER();
//
//	uvc_frame_t *frame = NULL;   //   uvc_frame_t  一帧数据的   结构体
//	uvc_frame_t *converted = NULL;
//	char *local_picture_path;
//
//	for (; isRunning() && isCapturing() ;) {
//		frame = waitCaptureFrame();
//		if (LIKELY(frame)) {
//			// frame data is always YUYV format.
//			if (LIKELY(isCapturing())) {
//				if (UNLIKELY(!converted)) {
//					converted = uvc_allocate_frame(previewBytes);
//				}
//				if (LIKELY(converted)) {
//					int b = uvc_any2rgbx(frame, converted);
//					if (!b) {
//                        pthread_mutex_lock(&capture_mutex); // 保护mCaptureWindow
//						if (LIKELY(mCaptureWindow)) {
//							copyToSurface(converted, &mCaptureWindow);
//						}
//                        pthread_mutex_unlock(&capture_mutex);
//					}
//				}
//			}
//			do_capture_callback(env, frame);   //  这里 应该是 一帧数据的   YUV  图像 数据   在这里 对YUV数据进行处理一下 测试
//		}
//	}
//	if (converted) {
//		uvc_free_frame(converted);
//	}
//	if (mCaptureWindow) {
//		ANativeWindow_release(mCaptureWindow);
//		mCaptureWindow = NULL;
//	}
//
//	EXIT();
//}
//
///**
//* call IFrameCallback#onFrame if needs
// */
//void UVCPreview::do_capture_callback(JNIEnv *env, uvc_frame_t *frame) {
//	ENTER();
//
//	if (LIKELY(frame)) {
//		uvc_frame_t *callback_frame = frame;
//		if (mFrameCallbackObj) {
//			if (mFrameCallbackFunc) {
//				callback_frame = uvc_allocate_frame(callbackPixelBytes);
//				if (LIKELY(callback_frame)) {
//					int b = mFrameCallbackFunc(frame, callback_frame);
//					uvc_free_frame(frame);
//					if (UNLIKELY(b)) {
//						LOGW("failed to convert for callback frame");
//						goto SKIP;
//					}
//				} else {
//					LOGW("failed to allocate for callback frame");
//					callback_frame = frame;
//					goto SKIP;
//				}
//			}
//			jobject buf = env->NewDirectByteBuffer(callback_frame->data, callbackPixelBytes);
//			env->CallVoidMethod(mFrameCallbackObj, iframecallback_fields.onFrame, buf);
//			env->ExceptionClear();
//			env->DeleteLocalRef(buf);
//		}
// SKIP:
//		uvc_free_frame(callback_frame);
//	}
//	EXIT();
//}
