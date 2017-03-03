#ifndef _CAMERA_API_INCLUDE_H_
#define _CAMERA_API_INCLUDE_H_


namespace Camera_Interface {


	typedef void (*FrameCallback)(unsigned char* pImage,int imageSize, int width, int height, int frameNum,int error,void* data);

	class Camera_API
	{
	public:
		virtual int Connect(int vid, int pid, int fd, const char *usbfs) = 0;
		virtual int Disconnect() = 0;
		virtual int StartCamera(FrameCallback call,void* data) = 0;
		virtual int StopCamera() = 0;
		virtual void CaptureSnapshot(const char* path) = 0;
	};

	Camera_API* createCamera();
	void releaseCamera(Camera_API* pCamera);

	char* getErrorMsg(int error);

}


#endif