//
// Created by 7Invensun on 2017/2/8.
//

#ifndef GEARVR_V1_GEARVRAPI_H
#define GEARVR_V1_GEARVRAPI_H

#include "FDG_Common.h"

namespace Gear_Vr_Interface
{

//TrackingMode
#define TRACKING_MODE_LEFTTRACK 1//左眼
#define TRACKING_MODE_RIGHTTRACK  2//右眼
#define TRACKING_MODE_BINOCULARTRACK  3//双眼
#define TRACKING_MODE_ELSETRACK  4//双眼找不到时，单眼

//DeviceType
#define DEVICE_TYPE_A2_SDK  0
#define DEVICE_TYPE_A5_SDK  1
#define DEVICE_TYPE_ZTE_PHONE  2
#define DEVICE_TYPE_HTC_MS  3
#define DEVICE_TYPE_GEAR_VR  4

/**
 * @brief The FDGParam struct FDG初始化参数
 */
    struct FDGParam
    {
        char szPath[512];                   //配置文件路径
        unsigned int nwidth;               //图像宽度
        unsigned int nheight;              //图像高度
        unsigned int nchannel;				 //图像通道数
        unsigned int devTypeEnum;          //设备枚举类型参数
        unsigned int trackingMode;         //眼睛追踪模式
    };
    typedef void (*CameraFrameCallback)(unsigned char* pImage,int imageSize, int width, int height, int frameNum,int error,void* data);

//    #define FDG_CAL_COE_INFO_DOUBLE_NUM 41 // result double num
//    #define FDG_CAL_COE_INFO_CHAR_NUM 32 // result char num
//#pragma pack(1)
//    struct FDG_CalCoeInfo
//    {
//        double value[FDG_CAL_COE_INFO_DOUBLE_NUM];
//        char ver[FDG_CAL_COE_INFO_CHAR_NUM];
//    };
//#pragma pack()
//    //eyes feature detected info
//    struct FDG_FDInfo
//    {
//        int				processOK;			///< 眼睛特征数据是否处理成功, 1 true; 0 false
//
//        int				realLeftEyeFound;	///< 左眼是否检测到
//        int				leftROI[4];			///< 左眼区域 [0]x [1]y [2]w [3]h
//        float			leftPupil[3];		///< 左眼瞳孔直径[0] 左眼瞳孔位置x[1] 左眼瞳孔位置y[2]
//        int				leftGlintNum;		///< 左眼光斑数量
//        float			leftGlints[8][3];	///< 左眼光斑数组
//
//        int				realRightEyeFound;	///< 右眼是否检测到
//        int				rightROI[4];		///< 右眼区域 [0]x [1]y [2]w [3]h
//        float			rightPupil[3];		///< 右眼瞳孔直径[0] 右眼瞳孔位置x[1] 右眼瞳孔位置y[2]
//        int				rightGlintNum;		///< 右眼光斑数量
//        float			rightGlints[8][3];	///< 右眼光斑数组
//
//        unsigned char*  imageData;			///< 图像数据指针
//        int				nImageSize;			///< 图像大小
//        int				nImageWidth;		///< 图像宽
//        int				nImageHeight;		///< 图像高
//    };
//    struct FDG_CalAllPointPos
//    {
//        int pointNum;//标定点个数
//        float point[9][2];//最多9个点的坐标值,point[][0]为x，point[][1]为y
//    };
//
//    typedef void (*FDGFinishCalibrationPoint)(int pointIndex, int error,void* data);
//    typedef void (*FDGFinishCalibration)(int totelPoint, FDG_CalCoeInfo cal, int error,void* data);
//    typedef void (*FDGCalibrationFDInfoCall)(int pointIndex, int frameNum, FDG_FDInfo fd,int error,void* data);
//    typedef void (*FDGGazeTracking)(int frameNum, float x, float y,float re,FDG_FDInfo fd,int error,void* data);
//    typedef void (*FDGThreadStartEndCall)();

    class GearVr_API
    {
    public:
        virtual int InitGearVr(FDGParam& param) = 0;
        virtual int ConnectCamera(int vid, int pid, int fd, const char *usbfs) = 0;
        virtual int DisconnectCamera() = 0;
//        virtual int SetCameraPreviewSize(int width,int height,int mode) = 0;
        virtual int StartCamera(CameraFrameCallback call,void* data) = 0;
        virtual int StopCamera() = 0;
        virtual void CaptureSnapshot(const char* path) = 0;

        virtual int FDGStartCalibration(FDG_Interface::CalAllPointPos& points, FDG_Interface::FinishCalibrationCallback call) = 0;
        virtual int FDGStartCalibrationPoint(int pointIndex, FDG_Interface::FinishCalibrationPointCallback call,FDG_Interface::CalibrationFDInfoCallback FdCall) = 0;
        virtual int FDGStopCalibrationPoint() = 0;
        virtual int FDGStartTracking(FDG_Interface::GazeTrackingCallback call) = 0;
        virtual int FDGStartTrackingWithoutCal(FDG_Interface::CalCoeInfo& cal,FDG_Interface::GazeTrackingCallback call) = 0;
        virtual int FDGStopTracking() = 0;
        //线程开始和结束时的回调函数，给android下java用，c++不需要，调用在Init之前
        virtual int FDGSetThreadCallback(FDG_Interface::ThreadStartEndCallback start,FDG_Interface::ThreadStartEndCallback end) = 0;
    };

    GearVr_API* createGearVrApi();
    void releaseGearVrApi(GearVr_API* pGearVr);
}

#endif //GEARVR_V1_GEARVRAPI_H
