//
// Created by 7Invensun on 2017/2/13.
//

#ifndef GEARVR_V1_GEARVRAPIJNI_H
#define GEARVR_V1_GEARVRAPIJNI_H

#include "GearVrApi.h"
#include <memory>
#include <jni.h>
#include "JCallback.h"

class GearVrApiJni
{
public:
    GearVrApiJni();
    ~GearVrApiJni(){}

public:
    void init();
    int startCamera(jobject callback);
    int startCalibration(int totelnum,jobject callback);
    int startCalibrationPoint(int pointIndex,jobject callback1,jobject callback2);
    int startTracking(jobject callback);
    int startTrackingWithoutCal(signed char* buff,int size,jobject callback);

    static void ThreadStop();
    static void StaticImageCallback(unsigned char* pImage,int imageSize, int width, int height, int frameNum,int error,void* data);
    void DealJImageCallback(unsigned char* pImage,int imageSize, int width, int height, int frameNum,int error);
//    static void FinishCalCallback(int totelPoint, FDG_Interface::CalCoeInfo* cal, int error,void* data);
    void DealFinishCalCallback(int totelPoint, FDG_Interface::CalCoeInfo* cal, int error);
//    static void FinishCalPointCallback(int pointIndex, int error,void* data);
    void DealFinishCalPointCallback(int pointIndex, int error);
//    static void CalibrationFDInfoCall(int pointIndex, int frameNum, Gear_Vr_Interface::FDG_FDInfo fd,int error,void* data);
    void DealCalibrationFDInfoCall(int pointIndex,FDG_Interface::CalPointInfo info,int error);
//    static void GazeTracking(int frameNum, float x, float y,float re,Gear_Vr_Interface::FDG_FDInfo fd,int error,void* data);
    void DealGazeTracking(int seq, FDG_Interface::GazePoint point,int error);

public:
    std::shared_ptr<Gear_Vr_Interface::GearVr_API> m_pGearVrApi;
    std::shared_ptr<IImageCallback> m_pImageCallback;
    std::shared_ptr<IFinishCalCallback> m_pFinishCalCallback;
    std::shared_ptr<IFinishCalPointCallback> m_pFinishCalPointCallback;
    std::shared_ptr<IEyesInfoCallback> m_pEyesInfoCallback;
    std::shared_ptr<ITrackingCallback> m_pTrackingCallback;
};


#endif //GEARVR_V1_GEARVRAPIJNI_H
