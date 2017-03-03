#include "Camera_API.h"
#include <unistd.h>
#include "utilbase.h"
#include "libusb.h"
#include "libuvc.h"
#include <string>
#include "aSeeDeviceCtrl.h"
#include "common.h"
#include <memory>
#include "bmp.h"

namespace Camera_Interface
{
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

	class UVCCamera : public Camera_API
	{
	public:
		UVCCamera()
		{
			m_frameCount = 0;
			m_reservedFrameCallback = nullptr;
			m_pDeviceHandle = nullptr;
			m_captureSnapshot = 0;
		}
	public:
		virtual int Connect(int vid, int pid, int fd, const char *usbfs);
		virtual int Disconnect();
//		virtual int SetPreviewSize(int width,int height,int mode);
		virtual int StartCamera(FrameCallback call,void* data);
		virtual int StopCamera();
		virtual void CaptureSnapshot(const char* path);

	public:
		static void uvc_frame_callback(uvc_frame_t *frame, void *vptr_args);

		void DealFrameCallback(uvc_frame_t *frame);
		inline size_t frameBytes() const;
	private:
		int m_Fd;
		uvc_device_t *m_pDevice;
		uvc_device_handle_t *m_pDeviceHandle;
		uvc_stream_ctrl_t m_ctrl;
		UVCContext m_Context;
		const CameraParam* m_defaultCameraParam;
		CameraParam m_CameraParam;
		int m_vendorId;
		int m_productId;
		int m_requestMode;
		int m_frameMode;
		int m_previewFormat;
		int m_frameWidth;
		int m_frameWidthRate;
		int m_frameHeight;
		FrameCallback m_frame_call;
		int m_state;
		int m_frameCount;
		void* m_reservedFrameCallback;
		int m_captureSnapshot;
	};

	int UVCCamera::Connect(int vid, int pid, int fd, const char *usbfs)
	{
		ENTER();
		LOGE("=== vid = %d,pid=%d,fd=%d,usbfs=%s\n",vid,pid,fd,usbfs);
		uvc_error_t result = UVC_ERROR_BUSY;
		if (!m_pDeviceHandle && fd)
		{
			result = m_Context.reset(usbfs);
			if (UNLIKELY(result < 0))
			{
				LOGD("failed to init libuvc");
				RETURN(result, int);
			}
			fd = dup(fd);
			result = uvc_find_device2(m_Context, &m_pDevice, vid, pid, NULL, fd);
			if (LIKELY(!result))
			{
				result = uvc_open(m_pDevice, &m_pDeviceHandle);    //  打开 设备
				if (LIKELY(!result))
				{
#if LOCAL_DEBUG
					uvc_print_diag(m_pDeviceHandle, stderr);
#endif
					brightness(m_pDeviceHandle, 2);
					m_Fd = fd;
					const CameraParam &def = _default(vid, pid);
					m_defaultCameraParam = &def;
					m_CameraParam = def;
					m_frameWidth = def.previewWidth;
					m_frameWidthRate = def.frameWidthRate;
					m_frameHeight = def.previewHeight;
					m_vendorId = vid;
					m_productId = pid;
					m_requestMode = DEFAULT_PREVIEW_MODE;
					m_frameMode = DEFAULT_PREVIEW_MODE;
					m_previewFormat = DEFAULT_PREVIEW_FORMAT;
				}
				else
				{
					LOGE("could not open camera:err=%d", result);
					uvc_unref_device(m_pDevice);
					m_pDevice = NULL;		//  SAFE_DELETE(mDevice);
					m_pDeviceHandle = NULL;
					close(fd);
				}
			}
			else
			{
				LOGE("could not find camera:err=%d", result);
				close(fd);
			}
		}
		else
		{
			LOGW("camera is already opened. you should release first");
		}
		RETURN(result, int);
	}
	int UVCCamera::Disconnect()
	{
		ENTER();
//		stopPreview();
		if (LIKELY(m_pDeviceHandle)) {
			uvc_close(m_pDeviceHandle);
			m_pDeviceHandle = NULL;
		}
		if (LIKELY(m_pDevice)) {
			uvc_unref_device(m_pDevice);
			m_pDevice = NULL;
		}
		if (LIKELY(m_Fd)) {
			close(m_Fd);
			m_Fd = 0;
		}
		RETURN(0, int);
	}
//	int UVCCamera::SetPreviewSize(int width,int height,int mode)
//	{
//		ENTER();
//		LOGI("requestWidth = %d, requestHeight = %d, width = %d, height = %d",
//			 m_CameraParam.previewWidth, m_CameraParam.previewHeight, width, height);
//		int result = 0;
//		if ((m_CameraParam.previewWidth != (width/m_frameWidthRate)) || (m_CameraParam.previewHeight != height) || (m_requestMode != mode)) {
//			m_CameraParam.previewWidth = width / m_frameWidthRate;
//			m_CameraParam.previewHeight = height;
//			m_requestMode = mode;
//
//			LOGI("uvc_get_stream_ctrl_format_size_fps = [%d, %d]", m_CameraParam.previewMinFps, m_CameraParam.previewMaxFps);
//			result = uvc_get_stream_ctrl_format_size_fps(m_pDeviceHandle, &m_ctrl,
//														 !m_requestMode ? UVC_FRAME_FORMAT_YUYV : UVC_FRAME_FORMAT_MJPEG,
//														 m_CameraParam.previewWidth, m_CameraParam.previewHeight, m_CameraParam.previewMinFps, m_CameraParam.previewMaxFps);
//		}
//		RETURN(result, int);
//	}
	int UVCCamera::StartCamera(FrameCallback call,void* data)
	{
		uvc_error_t result;
		uvc_frame_desc_t *frame_desc;

		//LOGI("2 uvc_get_stream_ctrl_format_size_fps = 60");
		LOGI("2 requestWidth = %d:requestHeight=%d", m_CameraParam.previewWidth,m_CameraParam.previewHeight);
		LOGI("2 cframeSize=(%d,%d)@%s", m_frameWidth, m_frameHeight, (!m_requestMode ? "YUYV" : "MJPEG"));
		result = uvc_get_stream_ctrl_format_size_fps(m_pDeviceHandle, &m_ctrl,
													 !m_requestMode ? UVC_FRAME_FORMAT_YUYV : UVC_FRAME_FORMAT_MJPEG,
													 m_CameraParam.previewWidth, m_CameraParam.previewHeight, m_CameraParam.previewMinFps, m_CameraParam.previewMaxFps);

		/*result = uvc_get_stream_ctrl_format_size(mDeviceHandle, ctrl,
                !requestMode ? UVC_FRAME_FORMAT_YUYV : UVC_FRAME_FORMAT_MJPEG,
                requestWidth, requestHeight, 60);*/

		result = uvc_get_frame_desc(m_pDeviceHandle, &m_ctrl, &frame_desc);
		if (LIKELY(!result))
		{
			m_frameWidth = frame_desc->wWidth;
			m_frameHeight = frame_desc->wHeight;
		}
		else
		{
			m_frameWidth = m_CameraParam.previewWidth;
			m_frameHeight = m_CameraParam.previewHeight;
		}
		m_frameMode = m_requestMode;

		uvc_frame_t *frame_mjpeg = NULL;
		result = uvc_start_iso_streaming(
				m_pDeviceHandle, &m_ctrl, uvc_frame_callback, (void *)this);
		if (LIKELY(!result))
		{
			m_frame_call = call;
			m_state = 1;
		}
		else
		{
			uvc_perror(result, "failed start_streaming");
			return -1;
		}
		m_reservedFrameCallback = data;
		return 0;
	}
	int UVCCamera::StopCamera()
	{
		m_state = 0;
		LOGI("uvc_stop_streaming...");
		uvc_stop_streaming(m_pDeviceHandle);
		LOGI("uvc_stop_streaming... OK");
		return 0;
	}
	void UVCCamera::uvc_frame_callback(uvc_frame_t *frame, void *vptr_args)
	{
		UVCCamera* camera = (UVCCamera*)vptr_args;
		camera->DealFrameCallback(frame);
	}
	void UVCCamera::DealFrameCallback(uvc_frame_t *frame)
	{
		if (UNLIKELY(!frame || !frame->frame_format || !frame->data || !frame->data_bytes))
		{
			return;
		}
		if (UNLIKELY(
				((frame->frame_format != UVC_FRAME_FORMAT_MJPEG) && (frame->actual_bytes < frameBytes()))
				|| (frame->width != m_frameWidth) || (frame->height != m_frameHeight) )) {

#if LOCAL_DEBUG
			LOGD("broken frame!:format=%d,actual_bytes=%d/%d(%d,%d/%d,%d)",
			frame->frame_format, frame->actual_bytes, preview->frameBytes,
			frame->width, frame->height, preview->frameWidth, preview->frameHeight);
#endif
			return;
		}
		m_frameCount++;
		if(m_frame_call == nullptr)
		{
			return;
		}
		if(m_state != 1)
		{
			return;
		}
		int data_bytes = frame->width * m_frameWidthRate * frame->height * 3 / 2;
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
		int width = copy->width * m_frameWidthRate;
		m_defaultCameraParam->copyFrameData((uint8_t *)copy->data, (const uint8_t *)frame->data, width, copy->height, width);
//		memcpy(copy->data,frame->data,width * copy->height);
		copy->actual_bytes = width * copy->height;
		//LOGI("%d/%d x %d, format = %d, %d/%d", (int)copy->width, width, (int)copy->height, (int)copy->frame_format, (int)copy->actual_bytes, (int)copy->data_bytes);
		copy->frame_format = UVC_FRAME_FORMAT_RAW8;
		// 未使用部分清零
		if (copy->data_bytes > copy->actual_bytes) {
			memset((char *)copy->data + copy->actual_bytes, 128, copy->data_bytes - copy->actual_bytes);
		}
		//LOGI("frame[1] {expect_data_bytes:%d, copy->data_bytes:%d, copy->actual_bytes:%d}", data_bytes, (int)copy->data_bytes, (int)copy->actual_bytes);
		auto image = std::shared_ptr<uvc_frame_t>(copy, [](uvc_frame_t *p) { if (p) uvc_free_frame(p); });
		image->width *= m_frameWidthRate;
		image->frame_format = adjustFrameFormat(m_defaultCameraParam, image->frame_format);
		std::shared_ptr<uint8_t> buf((uint8_t *)image->data, [image](uint8_t *){   //   YYYYYYYYYYYYYYYYY
			const_cast<std::shared_ptr<uvc_frame_t> &>(image).reset();
		});
		m_frame_call(buf.get(), frame->data_bytes, image->width, image->height,m_frameCount,0,m_reservedFrameCallback);
		if(m_captureSnapshot == 1)
		{
			SnapshotBmp("", (unsigned char*)image->data, image->width, image->height, 8);
			m_captureSnapshot = 0;
		}

	}
	size_t UVCCamera::frameBytes() const {
		return m_frameWidth * m_frameHeight * (!m_requestMode ? 2 : 4);
	}
	void UVCCamera::CaptureSnapshot(const char* path)
	{
		m_captureSnapshot = 1;
	}

	Camera_API* createCamera()
	{
		return new UVCCamera();
	}
	void releaseCamera(Camera_API* pCamera)
	{
		if(pCamera)
		{
			delete pCamera;
			pCamera = nullptr;
		}
	}

	char* getErrorMsg(int error)
	{
		return nullptr;
	}

}