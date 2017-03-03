#ifndef __ICAMERA_H__
#define __ICAMERA_H__
#include "IPreviewCallback.h"

struct ICamera
{
	// 相机应保证该函数返回后,则在原有callback上的调用也已运行完并停止.
	virtual void setPreviewCallback(IPreviewCallback *cb) = 0;
	virtual int startPreview() = 0;
	virtual int stopPreview() = 0;
};

#endif //!__ICAMERA_H__

