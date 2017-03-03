#ifndef FD_GAZE_API_INCLUDE_H
#define FD_GAZE_API_INCLUDE_H

#include <functional>
#include "FDG_Common.h"


namespace FDG_Interface {

	class FD_Gaze_API
	{
	public:
		//初始化
		virtual int Init(const FDConfig &FDCfg,const GazeConfig &GazeCfg,int nImageWidth,int nImageHeight, int nChannel, bool bLeft) = 0;
		//图像处理输入接口
		virtual int DealImage(const unsigned char* pSrcImageData,int nImageSize,int frameNum) = 0;
		//预处理图像接口，暂时不用
		virtual int SetPreDealImage(PreDealImageCallback prepare,int multiple) = 0;
		//校准的接口
		//校准开始接口
		virtual int StartCalibration(CalAllPointPos& points, FinishCalibrationCallback call) = 0;
		//校准某个点开始接口
		virtual int StartCalibrationPoint(int pointIndex, FinishCalibrationPointCallback call, CalibrationFDInfoCallback FdCall) = 0;
		//校准某点停止接口，停止后就不会回调CalibrationFDInfoCallback，否则会一直回调直到成功
		virtual int StopCalibrationPoint() = 0;
		//跟踪接口
		virtual int StartTracking(GazeTrackingCallback call) = 0;
		//直接用校准系数开始跟踪接口
		virtual int StartTrackingWithoutCal(CalCoeInfo& cal,GazeTrackingCallback call) = 0;
		//停止跟踪接口
		virtual int StopTracking() = 0;
		//线程开始和结束时的回调函数，给android下java用，c++不需要，调用在Init之前
		virtual int SetThreadCallback(ThreadStartEndCallback start,ThreadStartEndCallback end) = 0;
	};
	FD_Gaze_API* createFDG();
	void releaseFDG(FD_Gaze_API* pFDG);

	char* getErrorMsg(int error);

}

#endif