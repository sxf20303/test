#ifndef FD_GAZE_API_INCLUDE_H
#define FD_GAZE_API_INCLUDE_H

#include <functional>
#include "FDG_Common.h"


namespace FDG_Interface {

	class FD_Gaze_API
	{
	public:
		//��ʼ��
		virtual int Init(const FDConfig &FDCfg,const GazeConfig &GazeCfg,int nImageWidth,int nImageHeight, int nChannel, bool bLeft) = 0;
		//ͼ��������ӿ�
		virtual int DealImage(const unsigned char* pSrcImageData,int nImageSize,int frameNum) = 0;
		//Ԥ����ͼ��ӿڣ���ʱ����
		virtual int SetPreDealImage(PreDealImageCallback prepare,int multiple) = 0;
		//У׼�Ľӿ�
		//У׼��ʼ�ӿ�
		virtual int StartCalibration(CalAllPointPos& points, FinishCalibrationCallback call) = 0;
		//У׼ĳ���㿪ʼ�ӿ�
		virtual int StartCalibrationPoint(int pointIndex, FinishCalibrationPointCallback call, CalibrationFDInfoCallback FdCall) = 0;
		//У׼ĳ��ֹͣ�ӿڣ�ֹͣ��Ͳ���ص�CalibrationFDInfoCallback�������һֱ�ص�ֱ���ɹ�
		virtual int StopCalibrationPoint() = 0;
		//���ٽӿ�
		virtual int StartTracking(GazeTrackingCallback call) = 0;
		//ֱ����У׼ϵ����ʼ���ٽӿ�
		virtual int StartTrackingWithoutCal(CalCoeInfo& cal,GazeTrackingCallback call) = 0;
		//ֹͣ���ٽӿ�
		virtual int StopTracking() = 0;
		//�߳̿�ʼ�ͽ���ʱ�Ļص���������android��java�ã�c++����Ҫ��������Init֮ǰ
		virtual int SetThreadCallback(ThreadStartEndCallback start,ThreadStartEndCallback end) = 0;
	};
	FD_Gaze_API* createFDG();
	void releaseFDG(FD_Gaze_API* pFDG);

	char* getErrorMsg(int error);

}

#endif