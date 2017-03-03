
#ifdef _WIN32
#include "windows.h"
#else
#include<sys/time.h>
#endif

#include "FD_Gaze_API.h"
#include "CalibrationGaze.h"
#include "FeatureDetectionAPI.h"
#include "string.h"
#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>



#define USE_USB_KEY 0

#define EVERY_POINT_GATHER_NUM 10
#define EVERY_POINT_SELECT_NUM 10

int Get_Time()
{
#ifdef _WIN32
	auto time = GetTickCount();
	return (int)time;
#else
	struct timeval time;
	gettimeofday(&time,NULL); 
	return time.tv_sec*1000 + time.tv_usec/1000;
#endif
}

typedef struct devData{ 
	int imgH ;
	int imgW ;
	int Rmax ;
	int Rmin ;
	int Vmax ;
	int vestmethod;
	
	int rightorleft; //0:right, 1:left
	//double kappa[2]={0,0};

	double smult;
	double fc[2];
	double cc[2];
	double camckc[5];
	double lights[8][3];

	double lightsth[8];


	double lights2D[8][2];

	double lightsdis2D[8];
} * pDevData;


namespace FDG_Interface {

	static int FDG_STATE_NONE = 0;
	static int FDG_STATE_CALIBRATION = 1;
	static int FDG_STATE_CALIBRATION_DONE = 2;
	static int FDG_STATE_TRACKING = 3;
	class FD_Gaze_Obj : public FD_Gaze_API
	{
	public:
		FD_Gaze_Obj()
		{
			m_nImageWidth = 0;
			m_nImageHeight = 0;
			m_pGazePara = nullptr;
			m_pSmoothPara = nullptr;
			m_pFD = nullptr;
			m_state = 0;
			memset(&m_eyeCalCoe,0,sizeof(m_eyeCalCoe));
			m_callCalPoint = nullptr;
			m_callCal = nullptr;
			m_callCalFdInfo = nullptr;
			m_callGazeTrack = nullptr;
			m_bLeft = false;
			m_calPointTotel = 0;
			m_calCurPointIndex = 0;
			m_curPointCount = 0;
			m_bImageReady = false;
			m_imageBuff = nullptr;
			m_imgaeLength = 0;
			m_imgaeFrameNum = 0;
			m_isStopThread = 0;
			m_TimeMillisecond = 0;
			m_ThreadStart = nullptr;
			m_ThreadEnd = nullptr;
			m_nPreImageMultiple = 0;
			m_calCurPointCount = 0;
		}
	public:
		//初始化
		virtual int Init(const FDConfig &FDCfg,const GazeConfig &GazeCfg,int nImageWidth,int nImageHeight, int nChannel, bool bLeft);
		//图像处理输入接口
		virtual int DealImage(const unsigned char* pSrcImageData,int nImageSize,int frameNum);
		//预处理图像接口，暂时不用
		virtual int SetPreDealImage(PreDealImageCallback prepare,int multiple);
		//校准的接口
		//校准开始接口
		virtual int StartCalibration(CalAllPointPos& points, FinishCalibrationCallback call);
		//校准某个点开始接口
		virtual int StartCalibrationPoint(int pointIndex, FinishCalibrationPointCallback call, CalibrationFDInfoCallback FdCall);
		//校准某点停止接口，停止后就不会回调CalibrationFDInfoCallback，否则会一直回调直到成功
		virtual int StopCalibrationPoint();
		//跟踪接口
		virtual int StartTracking(GazeTrackingCallback call);
		//直接用校准系数开始跟踪接口
		virtual int StartTrackingWithoutCal(CalCoeInfo& cal,GazeTrackingCallback call);
		//停止跟踪接口
		virtual int StopTracking();
		//线程开始和结束时的回调函数，给android下java用，c++不需要，调用在Init之前
		virtual int SetThreadCallback(ThreadStartEndCallback start,ThreadStartEndCallback end);

	private:
		void ThreadDealImage();
		int TryLockGetImage(std::shared_ptr<unsigned char> &image,int& imageSize,int& frameNum);
		int TryLockDealImage(float& x, float& y,float &re,FDInfo& fd,std::shared_ptr<unsigned char>& ptr,int& nImageLength,int& frameNum,int& calPoint,int& isFinishPoint,int& isFinishCal,int& calPointCount);
		int DoPreDealImage(std::shared_ptr<unsigned char>& pDst,int& dstSize,std::shared_ptr<unsigned char> pSrc,int srcSize);
		void SetEyeCalCoe(CalCoeInfo& cal);
		void GetEyeCalCoe(CalCoeInfo& cal);
		void GetFDInfo(FDInfo& fd);
		void GetCalPointInfo(int calpoint,CalPointInfo& info);
		void SetState(int state);
		CalibrationGaze::EyeCalCoe getEyeCalCoe();// there is multithread problem,so you need use this function to get variables.
		int IsCalibrationPoint(int &pointIndex,int& calPointCount);
		int SetCalPointFdInfo(int pointIndex,int& isFinishPoint,int& isFinishCal);
		int GetLeftRightEyeFd(ASEE_FeatureDetectionData& fd,CalibrationGaze::EyeFeature& eyeFeature);

	private:
		int m_nImageWidth;
		int m_nImageHeight;
		void* m_pGazePara;
		void* m_pSmoothPara;
		bool m_bLeft;
		IFeaturesDetection* m_pFD;
		//current data, every frame is different, don't need to lock,only one thread use them.
		ASEE_FeatureDetectionData m_curFd;
		CalibrationGaze::gazeEstData m_curGazeEstData;

		//variables that is needed to lock in own thread
		CalibrationGaze::EyeCalCoe m_eyeCalCoe;
		int m_state;
		int m_calPointTotel;
		int m_calCurPointIndex;
		int m_calCurPointCount;
		std::vector<CalibrationGaze::PointF> m_allpoints;
		std::vector<int> m_vtCalPoints;
		CalibrationGaze::gazeSelectData m_pointSelectData;//select calibration input, need every frame fd
		int m_curPointCount;
		CalibrationGaze::gazeCalData m_gazeCalData;

		FinishCalibrationPointCallback m_callCalPoint;
		FinishCalibrationCallback m_callCal;
		CalibrationFDInfoCallback m_callCalFdInfo;
		GazeTrackingCallback m_callGazeTrack;
		PreDealImageCallback m_preDealImageCall;
		int m_nPreImageMultiple;

		//multi thread variable
		std::mutex m_mtx;//data mutex
		std::mutex m_mutex;//image mutex
		std::condition_variable cv;//condition
		bool m_bImageReady;//false not ready, ture ready
		std::shared_ptr<unsigned char> m_imageBuff;
		int m_imgaeLength;
		int m_imgaeFrameNum;

		int m_isStopThread;
		int m_TimeMillisecond;

		//void* m_reservedFinishCalibrationPoint;
		//void* m_reservedFinishCalibration;
		//void* m_reservedCalibrationFDInfoCall;
		//void* m_reservedGazeTracking;
		//void* m_reservedPreDealImage;


		ThreadStartEndCallback m_ThreadStart;
		ThreadStartEndCallback m_ThreadEnd;
	};

	//初始化
	int FD_Gaze_Obj::Init(const FDConfig &FDCfg,const GazeConfig &GazeCfg,int nImageWidth,int nImageHeight, int nChannel, bool bLeft)
	{
		int ret = -1;
		if(USE_USB_KEY == 0)
		{
			ret = CalibrationGaze::InitGazeEstimation(GazeCfg.device,GazeCfg.method,m_pGazePara);
			if(ret != 0)
				return -1;
		}
		else
		{
			//use usbkey to init gaze
		}
		ret = CalibrationGaze::InitSmooth(GazeCfg.th,GazeCfg.ex,GazeCfg.tl,m_pSmoothPara);
		if(ret != 0)
			return -2;
		m_pFD = CreateFeaturesDetection();
		CONFIG_LIST conf;
		conf.c_center_x = FDCfg.c_center_x;
		conf.c_center_y = FDCfg.c_center_y;
		conf.c_rowsMask = FDCfg.c_rowsMask;
		conf.c_colsMask = FDCfg.c_colsMask;
		conf.c_radius = FDCfg.c_radius;
		conf.c_cutBellow = FDCfg.c_cutBellow;
		conf.c_ruRadius = FDCfg.c_ruRadius;
		conf.c_leftOrRight = FDCfg.c_leftOrRight;
		conf.c_glass = FDCfg.c_glass;
		
		if(!m_pFD->SefCfg(&conf))
			return -3;
		if(FDCfg.has_glass == 1)
			m_pFD->SetGlassStatus(1);
		m_pFD->Init(nImageWidth,nImageHeight,nChannel,bLeft);
		m_bLeft = bLeft;
		std::thread t1(&FD_Gaze_Obj::ThreadDealImage,this);
		m_isStopThread = 0;
		t1.detach();
		return 0;
	}
	void FD_Gaze_Obj::ThreadDealImage()
	{
		if(m_ThreadStart)
			m_ThreadStart();
		while(!m_isStopThread)
		{
			float x,y,re;
			FDInfo fd;
			memset(&fd,0,sizeof(fd));
			std::shared_ptr<unsigned char> ptr;
			int frameNum,nImageLength,calPoint,isFinishPoint,isFinishCal,calPointCount;
			int ret = TryLockDealImage(x,y,re,fd,ptr,nImageLength,frameNum,calPoint,isFinishPoint,isFinishCal,calPointCount);
			if(ret == 1)
			{
				if(m_callGazeTrack)
				{
					fd.imageData = ptr.get();
					fd.nImageSize = nImageLength;
					fd.nImageWidth = m_nImageWidth;
					fd.nImageHeight = m_nImageHeight;
					GazePoint point;
					point.x = x;
					point.y = y;
					point.re = re;
					point.fd = &fd;
					m_callGazeTrack(frameNum,point,0);
				}
			}
			else if(ret == 2)
			{
				if(m_callCalFdInfo)
				{
					fd.imageData = ptr.get();
					fd.nImageSize = nImageLength;
					fd.nImageWidth = m_nImageWidth;
					fd.nImageHeight = m_nImageHeight;
					CalPointInfo info;
					GetCalPointInfo(calPoint,info);
					info.count = calPointCount;
					info.fd = &fd;
					info.seq = frameNum;
					m_callCalFdInfo(calPoint,info,0);
				}
				if(isFinishPoint == 1 && m_callCalPoint)
				{
					m_callCalPoint(calPoint,0);
				}
				else if(isFinishPoint == -1 && m_callCalPoint)
				{
					m_callCalPoint(calPoint,-1);
				}
				if(isFinishCal == 1 && m_callCal)
				{
					CalCoeInfo calInfo;
					GetEyeCalCoe(calInfo);
					m_callCal(m_calPointTotel,&calInfo,0);
				}
				else if(isFinishCal == -1 && m_callCal)
				{
					m_callCal(m_calPointTotel,nullptr,-1);
				}
			}
			else if(ret == -3)
			{
				if(m_callCalFdInfo)
				{
					CalPointInfo info;
					GetCalPointInfo(calPoint,info);
					info.count = calPointCount;
					info.seq = frameNum;
					info.fd = nullptr;
					m_callCalFdInfo(calPoint,info,1);
				}
			}
		}
		if(m_ThreadEnd)
			m_ThreadEnd();
	}
	int FD_Gaze_Obj::TryLockGetImage(std::shared_ptr<unsigned char> &image,int& imageSize,int& frameNum)
	{
		std::unique_lock<std::mutex> lck(m_mutex);
		while(!m_bImageReady)
		{
			cv.wait(lck);
		}
		int ret = 0;
		image = nullptr;
		if(m_imageBuff)
		{
			image = m_imageBuff;
			m_imageBuff = nullptr;
			imageSize = m_imgaeLength;
			frameNum = m_imgaeFrameNum;
			m_bImageReady = false;
		}
		return ret;
	}

	int FD_Gaze_Obj::TryLockDealImage(float& x, float& y,float &re,FDInfo& fd,std::shared_ptr<unsigned char>& ptr,int& nImageLength,int& frameNum,int& calPoint,int& isFinishPoint,int& isFinishCal,int& calPointCount)
	{
		//std::unique_lock<std::mutex> lck(m_mutex);
		//while(!m_bImageReady)
		//{
		//	cv.wait(lck);
		//}
		std::shared_ptr<unsigned char> imageBuff;
		int imageBuffSize;
		int imageFrameNum;
		TryLockGetImage(imageBuff,imageBuffSize,imageFrameNum);
		int ret = 0;
		if(imageBuff)
		{
			unsigned char* pSrcImageData;
			int nImageSize;
			std::shared_ptr<unsigned char> preImage;
			int preSize;
			int isPre = DoPreDealImage(preImage,preSize,imageBuff,imageBuffSize);
			if(isPre == 0)
			{
				pSrcImageData = preImage.get();
				nImageSize = preSize;
				frameNum = imageFrameNum;
			}
			else
			{
				pSrcImageData = imageBuff.get();
				nImageSize = imageBuffSize;
				frameNum = imageFrameNum;
			}
			if(m_state == FDG_STATE_TRACKING)
			{
				memset(&m_curFd,0,sizeof(m_curFd));
				memset(&m_curGazeEstData,0,sizeof(m_curGazeEstData));
				std::shared_ptr<unsigned char> p(new unsigned char[nImageSize*3],[](unsigned char*p){if(p) delete [] p; p = nullptr;});
				if(m_pFD->DealImage(pSrcImageData,p.get(),&m_curFd))
				{
					m_curGazeEstData.coeff = getEyeCalCoe();
					GetLeftRightEyeFd(m_curFd,m_curGazeEstData.eye);
					devData* pdata = (devData*)m_pGazePara;
					if(CalibrationGaze::GazeEstimation(m_curGazeEstData,m_pGazePara) > 0)
					{
						int time = 10000;
						if(m_TimeMillisecond != 0)
						{
							time = Get_Time() - m_TimeMillisecond;
						}
						if(CalibrationGaze::SmoothGaze(m_curGazeEstData.eye.gaze,re,time,m_pSmoothPara) == 0)
						{
							m_TimeMillisecond = Get_Time();
							GetFDInfo(fd);
							ptr = p;
							nImageLength = nImageSize*3;
							x = m_curGazeEstData.eye.gaze.x;
							y = m_curGazeEstData.eye.gaze.y;
							ret = 1;
						}
						else
						{
							ret = -5;
						}
					}
					else
					{
						ret = -2;
					}
				}
				else
				{
					ret = -1;
				}
			}
			else if(m_state == FDG_STATE_CALIBRATION)
			{
				calPoint = 0;
				if(IsCalibrationPoint(calPoint,calPointCount) == 0)
				{
					memset(&m_curFd,0,sizeof(m_curFd));
					std::shared_ptr<unsigned char> p(new unsigned char[nImageSize*3],[](unsigned char*p){if(p) delete [] p; p = nullptr;});
					if(m_pFD->DealImage(pSrcImageData,p.get(),&m_curFd))
					{
						GetFDInfo(fd);
						ptr = p;
						nImageLength = nImageSize*3;
						if(SetCalPointFdInfo(calPoint,isFinishPoint,isFinishCal) == 0)
						{
							ret = 2;
						}
						else
						{
							ret = -4;
						}
					}
					else
					{
						ret = -3;
					}
				}
			}
		}
		return ret;
	}
	int FD_Gaze_Obj::DealImage(const unsigned char* pSrcImageData,int nImageSize,int frameNum)
	{
		//if(m_mutex.try_lock() == false)
		//{
		//	return 0;
		//}
		//memset(&m_curFd,0,sizeof(m_curFd));
		//std::shared_ptr<unsigned char> p(new unsigned char[nImageSize*3],[](unsigned char*p){if(p) delete [] p; p = nullptr;});
		//if(m_pFD->DealImage(pSrcImageData,p.get(),&m_curFd))
		//{
		//	printf("\n");
		//}
		std::unique_lock<std::mutex> lck(m_mutex);
		std::shared_ptr<unsigned char> buff(new unsigned char[nImageSize],[](unsigned char*p){if(p) delete [] p; p = nullptr;});
		memcpy(buff.get(),pSrcImageData,nImageSize);
		m_imageBuff = buff;
		m_imgaeLength = nImageSize;
		m_imgaeFrameNum = frameNum;
		//if(m_pFD->DealImage(m_imageBuff.get(),p.get(),&m_curFd))
		//{
		//	printf("\n");
		//}
		m_bImageReady = true;
		cv.notify_all();
		//m_mutex.unlock();
		return 0;
	}
	int FD_Gaze_Obj::SetPreDealImage(PreDealImageCallback prepare,int multiple)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		m_preDealImageCall = prepare;
		m_nPreImageMultiple = multiple;
		return 0;
	}
	int FD_Gaze_Obj::DoPreDealImage(std::shared_ptr<unsigned char>& pDst,int& dstSize,std::shared_ptr<unsigned char> pSrc,int srcSize)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		if(m_preDealImageCall == nullptr)
			return -1;
		std::shared_ptr<unsigned char> buff(new unsigned char[srcSize*m_nPreImageMultiple],[](unsigned char*p){if(p) delete [] p; p = nullptr;});
		if(m_preDealImageCall(buff.get(),dstSize,pSrc.get(),m_nImageWidth,m_nImageHeight) == 0)
		{
			pDst = buff;
		}
		else
		{
			return -2;
		}
		return 0;
	}

	//校准的接口
	int FD_Gaze_Obj::StartCalibration(CalAllPointPos& points, FinishCalibrationCallback call)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		SetState(FDG_STATE_CALIBRATION);
		m_calPointTotel = points.pointNum;
		m_calCurPointIndex = 0;
		m_calCurPointCount = 0;
		m_curPointCount = 0;
		m_vtCalPoints.clear();
		m_vtCalPoints.resize(m_calPointTotel);
		m_allpoints.clear();
		for(int i = 0; i < m_calPointTotel ; i++)
		{
			CalibrationGaze::PointF pos;
			pos.x = points.point[i][0];
			pos.y = points.point[i][1];
			m_allpoints.push_back(pos);
		}
		memset(&m_pointSelectData,0,sizeof(m_pointSelectData));
		memset(&m_gazeCalData,0,sizeof(m_gazeCalData));
		
		m_gazeCalData.frameNum = EVERY_POINT_SELECT_NUM;
		m_gazeCalData.calNum = m_calPointTotel;
		m_gazeCalData.finalCal = true;
		m_callCal = call;
		return 0;
	}
	int FD_Gaze_Obj::StartCalibrationPoint(int pointIndex, FinishCalibrationPointCallback call, CalibrationFDInfoCallback FdCall)
	{
		if(pointIndex > m_calPointTotel || pointIndex <= 0)
			return -1;
		std::unique_lock<std::mutex> lock(m_mtx);
		m_calCurPointIndex = pointIndex;
		m_calCurPointCount = 0;
		m_callCalPoint = call;
		m_callCalFdInfo = FdCall;
		memset(&m_pointSelectData,0,sizeof(m_pointSelectData));
		m_vtCalPoints[pointIndex-1] = 0;
		m_curPointCount = 0;
		m_pointSelectData.totalNum = EVERY_POINT_GATHER_NUM;
		m_pointSelectData.selectNum = EVERY_POINT_SELECT_NUM;
		return 0;
	}
	int FD_Gaze_Obj::StopCalibrationPoint()
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		m_vtCalPoints[m_calCurPointIndex-1] = 0;
		m_calCurPointIndex = 0;
		m_calCurPointCount = 0;
		memset(&m_pointSelectData,0,sizeof(m_pointSelectData));
		m_curPointCount = 0;
		m_pointSelectData.totalNum = EVERY_POINT_GATHER_NUM;
		m_pointSelectData.selectNum = EVERY_POINT_SELECT_NUM;
		return 0;
	}

	//跟踪接口
	int FD_Gaze_Obj::StartTracking(GazeTrackingCallback call)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		if(m_state != FDG_STATE_CALIBRATION_DONE)
			return -1;
		SetState(FDG_STATE_TRACKING);
		m_callGazeTrack = call;
		return 0;
	}
	void FD_Gaze_Obj::SetEyeCalCoe(CalCoeInfo& cal)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		int index = 0;
		for(int i = 0 ; i < 12; i++,index++)
			m_eyeCalCoe.calCoeff[i] = cal.value[index];
		for(int i = 0 ; i < 12; i++,index++)
			m_eyeCalCoe.auxCoeff[i] = cal.value[index];
		m_eyeCalCoe.vp.kappa[0] = cal.value[index++];
		m_eyeCalCoe.vp.kappa[1] = cal.value[index++];
		m_eyeCalCoe.vp.R = cal.value[index++];
		m_eyeCalCoe.vp.K = cal.value[index++];
		m_eyeCalCoe.vp.backoff = cal.value[index++];
		for(int i = 0 ; i < 12; i++,index++)
			m_eyeCalCoe.dc[i] = cal.value[index];
		for(int i = 0; i < 32; i++)
			m_eyeCalCoe.ver[i] = cal.ver[i];
	}
	void FD_Gaze_Obj::GetEyeCalCoe(CalCoeInfo& cal)
	{
		int index = 0;
		for(int i = 0 ; i < 12; i++,index++)
			cal.value[index] = m_eyeCalCoe.calCoeff[i];
		for(int i = 0 ; i < 12; i++,index++)
			cal.value[index] = m_eyeCalCoe.auxCoeff[i];
		cal.value[index++] = m_eyeCalCoe.vp.kappa[0];
		cal.value[index++] = m_eyeCalCoe.vp.kappa[1];
		cal.value[index++] = m_eyeCalCoe.vp.R;
		cal.value[index++] = m_eyeCalCoe.vp.K;
		cal.value[index++] = m_eyeCalCoe.vp.backoff;
		for(int i = 0 ; i < 12; i++,index++)
			cal.value[index] = m_eyeCalCoe.dc[i];
		for(int i = 0; i < 32; i++)
			cal.ver[i] = m_eyeCalCoe.ver[i];
	}
	void FD_Gaze_Obj::GetFDInfo(FDInfo& fd)
	{
		fd.processOK = m_curFd.processOK;
		fd.realLeftEyeFound = m_curFd.realLeftEyeFound;
		fd.leftROI[0] = m_curFd.leftROI.x;
		fd.leftROI[1] = m_curFd.leftROI.y;
		fd.leftROI[2] = m_curFd.leftROI.w;
		fd.leftROI[3] = m_curFd.leftROI.h;
		fd.leftPupil[0] = m_curFd.leftPupilDiameter;
		fd.leftPupil[1] = m_curFd.leftPupilx;
		fd.leftPupil[2] = m_curFd.leftPupily;
		fd.leftGlintNum = m_curFd.leftGlintNum;
		for(int i = 0; i < 8; i++)
		{
			fd.leftGlints[i][0] = m_curFd.leftGlints[i].x;
			fd.leftGlints[i][1] = m_curFd.leftGlints[i].y;
			fd.leftGlints[i][2] = m_curFd.leftGlints[i].re;
		}
		fd.realRightEyeFound = m_curFd.realRightEyeFound;
		fd.rightROI[0] = m_curFd.rightROI.x;
		fd.rightROI[1] = m_curFd.rightROI.y;
		fd.rightROI[2] = m_curFd.rightROI.w;
		fd.rightROI[3] = m_curFd.rightROI.h;
		fd.rightPupil[0] = m_curFd.rightPupilDiameter;
		fd.rightPupil[1] = m_curFd.rightPupilx;
		fd.rightPupil[2] = m_curFd.rightPupily;
		fd.rightGlintNum = m_curFd.rightGlintNum;
		for(int i = 0; i < 8; i++)
		{
			fd.rightGlints[i][0] = m_curFd.rightGlints[i].x;
			fd.rightGlints[i][1] = m_curFd.rightGlints[i].y;
			fd.rightGlints[i][2] = m_curFd.rightGlints[i].re;
		}
	}
	void FD_Gaze_Obj::GetCalPointInfo(int calpoint,CalPointInfo& info)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		memset(&info,0,sizeof(info));
		if(calpoint != m_calCurPointIndex)
		{
			if(m_calCurPointIndex == 0)
			{
				info.percent = 100;
				info.finish = EVERY_POINT_GATHER_NUM;
				info.totel = EVERY_POINT_GATHER_NUM;
			}
			return;
		}
		info.count = m_calCurPointCount;
		info.finish = m_curPointCount;
		info.percent = (int)(100*m_curPointCount/EVERY_POINT_GATHER_NUM);
		info.totel = EVERY_POINT_GATHER_NUM;
	}

	int FD_Gaze_Obj::StartTrackingWithoutCal(CalCoeInfo& cal,GazeTrackingCallback call)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		SetEyeCalCoe(cal);
		SetState(FDG_STATE_TRACKING);
		m_callGazeTrack = call;
		return 0;
	}
	int FD_Gaze_Obj::StopTracking()
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		if(m_state != FDG_STATE_TRACKING)
			return -1;
		SetState(FDG_STATE_CALIBRATION_DONE);
		m_TimeMillisecond = 0;
		return 0;
	}
	void FD_Gaze_Obj::SetState(int state)
	{
		m_state = state;
		m_callCal = nullptr;
		m_callCalPoint = nullptr;
		m_callCalFdInfo = nullptr;
		m_callGazeTrack = nullptr;
	}
	CalibrationGaze::EyeCalCoe FD_Gaze_Obj::getEyeCalCoe()
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		return m_eyeCalCoe;
	}
	int FD_Gaze_Obj::IsCalibrationPoint(int &pointIndex,int& calPointCount)
	{
		std::unique_lock<std::mutex> lock(m_mtx);
		if(m_state != FDG_STATE_CALIBRATION)
			return -1;
		if(m_calCurPointIndex == 0)
			return -2;
		pointIndex = m_calCurPointIndex;
		m_calCurPointCount++;
		calPointCount = m_calCurPointCount;
		return 0;
	}
	int FD_Gaze_Obj::SetCalPointFdInfo(int pointIndex,int& isFinishPoint,int& isFinishCal)
	{
		isFinishPoint = 0;
		isFinishCal = 0;
		std::unique_lock<std::mutex> lock(m_mtx);
		if(m_state != FDG_STATE_CALIBRATION)
			return -1;
		if(m_calCurPointIndex == 0 || m_calCurPointIndex != pointIndex)
			return -2;
		if(m_curPointCount >= EVERY_POINT_GATHER_NUM)
			return -3;
		GetLeftRightEyeFd(m_curFd,m_pointSelectData.eyes[m_curPointCount]);
		m_pointSelectData.eyes[m_curPointCount].gaze = m_allpoints[m_calCurPointIndex-1];
		m_curPointCount++;
		if(m_curPointCount >= EVERY_POINT_GATHER_NUM)
		{
			if(CalibrationGaze::Selection(m_pointSelectData,m_pGazePara) >= 0)
			{
				for(int i = 0; i < EVERY_POINT_SELECT_NUM; i++)
				{
					memcpy(&m_gazeCalData.eyes[(m_calCurPointIndex - 1)*EVERY_POINT_SELECT_NUM + i],&m_pointSelectData.eyes[i],sizeof(m_pointSelectData.eyes[i]));
				}
				m_vtCalPoints[m_calCurPointIndex - 1] = 1;
				m_calCurPointIndex = 0;
				m_calCurPointCount = 0;
				m_curPointCount = 0;
				isFinishPoint = 1;
				int isAllFinish = 1;
				for(auto &i:m_vtCalPoints)
				{
					if(i == 0)
					{
						isAllFinish = 0;
						break;
					}
				}
				if(isAllFinish == 1)
				{
					if(CalibrationGaze::Calibration(m_gazeCalData,m_pGazePara) == 1)
					{
						m_eyeCalCoe = m_gazeCalData.coeff;
						m_state = FDG_STATE_CALIBRATION_DONE;
						m_calPointTotel = 0;
						m_calCurPointIndex = 0;
						m_calCurPointCount = 0;
						m_vtCalPoints.clear();
						isFinishCal = 1;
					}
					else
					{
						m_state = FDG_STATE_CALIBRATION;
						m_calCurPointIndex = 0;
						m_calCurPointCount = 0;
						m_curPointCount = 0;
						m_vtCalPoints.clear();
						m_vtCalPoints.resize(m_calPointTotel);
						memset(&m_pointSelectData,0,sizeof(m_pointSelectData));
						memset(&m_gazeCalData,0,sizeof(m_gazeCalData));
						m_gazeCalData.frameNum = EVERY_POINT_SELECT_NUM;
						m_gazeCalData.calNum = m_calPointTotel;
						isFinishCal = -1;
					}
				}
			}
			else
			{
				isFinishPoint = -1;
				m_vtCalPoints[m_calCurPointIndex-1] = 0;
				m_calCurPointIndex = 0;
				m_calCurPointCount = 0;
				memset(&m_pointSelectData,0,sizeof(m_pointSelectData));
				m_curPointCount = 0;
				m_pointSelectData.totalNum = EVERY_POINT_GATHER_NUM;
				m_pointSelectData.selectNum = EVERY_POINT_SELECT_NUM;
			}
		}
		return 0;
	}
	int FD_Gaze_Obj::GetLeftRightEyeFd(ASEE_FeatureDetectionData& fd,CalibrationGaze::EyeFeature& eyeFeature)
	{
		if(m_bLeft == false)
		{
			eyeFeature.pupilx = fd.rightPupilx;
			eyeFeature.pupily = fd.rightPupily;
			memcpy(eyeFeature.glints,fd.rightGlints,sizeof(fd.rightGlints));
		}
		else
		{
			eyeFeature.pupilx = fd.leftPupilx;
			eyeFeature.pupily = fd.leftPupily;
			memcpy(eyeFeature.glints,fd.leftGlints,sizeof(fd.leftGlints));
		}
		return 0;
	}
	int FD_Gaze_Obj::SetThreadCallback(ThreadStartEndCallback start,ThreadStartEndCallback end)
	{
		m_ThreadStart = start;
		m_ThreadEnd = end;
		return 0;
	}

	FD_Gaze_API* createFDG()
	{
		return new FD_Gaze_Obj();
	}
	void releaseFDG(FD_Gaze_API* pFDG)
	{
		if(pFDG)
		{
			delete pFDG;
			pFDG = nullptr;
		}
	}

	char* getErrorMsg(int error)
	{
		return nullptr;
	}

}