//
// Created by 7Invensun on 2017/2/13.
//

#include "Log.h"
#include "GearVrApiJni.h"

using namespace FDG_Interface;
using namespace Gear_Vr_Interface;


int initCalPointPos(CalAllPointPos& points)
{
    if(points.pointNum != 3 && points.pointNum != 9)
        return -1;
    if(points.pointNum == 9)
    {
        points.point[0][0] = 1;
        points.point[0][1] = 1;
        points.point[1][0] = 1;
        points.point[1][1] = 1;
        points.point[2][0] = 1;
        points.point[2][1] = 1;
        points.point[3][0] = 1;
        points.point[3][1] = 1;
        points.point[4][0] = 1;
        points.point[4][1] = 1;
        points.point[5][0] = 1;
        points.point[5][1] = 1;
        points.point[6][0] = 1;
        points.point[6][1] = 1;
        points.point[7][0] = 1;
        points.point[7][1] = 1;
        points.point[8][0] = 1;
        points.point[8][1] = 1;
    }
    return 0;
}

GearVrApiJni::GearVrApiJni()
{
    m_pGearVrApi = std::shared_ptr<GearVr_API>(createGearVrApi(),releaseGearVrApi);
}
void GearVrApiJni::init()
{
    if(m_pGearVrApi)
        m_pGearVrApi->FDGSetThreadCallback(nullptr,ThreadStop);
    else
        LOGI("init failed");
}
void GearVrApiJni::ThreadStop()
{
    JEnv::detachCurrentThread();
}

int GearVrApiJni::startCamera(jobject callback)
{
    LOGI("ITracker: %p, callback: %p", this, callback);
    int result = 0;
    if (callback) {
        JImageCallback *jimageCallback = new JImageCallback(callback);
        m_pImageCallback = std::shared_ptr<IImageCallback>(jimageCallback, [=](IImageCallback *) {
            LOGI("Remove Old ImageCallback(%p)", callback);
            delete jimageCallback;
        });
        result = m_pGearVrApi->StartCamera(StaticImageCallback,this);
    } else {
        m_pImageCallback = nullptr;
        result = m_pGearVrApi->StartCamera(nullptr,this);
    }
    return result;
}
void GearVrApiJni::StaticImageCallback(unsigned char* pImage,int imageSize, int width, int height, int frameNum,int error,void* data)
{
    GearVrApiJni *pSDK = (GearVrApiJni *)data;
    pSDK->DealJImageCallback(pImage,imageSize,width,height,frameNum,error);

}
void GearVrApiJni::DealJImageCallback(unsigned char* pImage,int imageSize, int width, int height, int frameNum,int error)
{
    if(m_pImageCallback)
    {
        m_pImageCallback->onImage(pImage,imageSize,width,height,frameNum);
    }
}
int GearVrApiJni::startCalibration(int totelnum,jobject callback)
{
    LOGI("ITracker: %p, callback: %p", this, callback);
    int result = 0;
    CalAllPointPos points;
    points.pointNum = totelnum;
    initCalPointPos(points);
    if (callback) {
        JFinishCalCallback *jfinishCalCallback = new JFinishCalCallback(callback);
        m_pFinishCalCallback = std::shared_ptr<IFinishCalCallback>(jfinishCalCallback, [=](IFinishCalCallback *) {
            LOGI("Remove Old JFinishCalCallback(%p)", callback);
            delete jfinishCalCallback;
        });

    } else {
        m_pFinishCalCallback = nullptr;
    }
    result = m_pGearVrApi->FDGStartCalibration(points,FDG_CALLBACK_FC(GearVrApiJni::DealFinishCalCallback,this));
    return result;
}
//void GearVrApiJni::FinishCalCallback(int totelPoint, FDG_CalCoeInfo cal, int error,void* data)
//{
//    GearVrApiJni *pSDK = (GearVrApiJni *)data;
//    pSDK->DealFinishCalCallback(totelPoint,cal,error);
//}
void GearVrApiJni::DealFinishCalCallback(int totelPoint, CalCoeInfo* cal, int error)
{
    if(m_pFinishCalCallback)
    {
        m_pFinishCalCallback->onFinishCal(totelPoint,(unsigned char*)cal,sizeof(CalCoeInfo),error);
    }
}
int GearVrApiJni::startCalibrationPoint(int pointIndex,jobject callback1,jobject callback2)
{
    int result = 0;
    if (callback1) {
        JFinishCalPointCallback *jfinishCalPointCallback = new JFinishCalPointCallback(callback1);
        m_pFinishCalPointCallback = std::shared_ptr<IFinishCalPointCallback>(jfinishCalPointCallback, [=](IFinishCalPointCallback *) {
            LOGI("Remove Old JFinishCalPointCallback(%p)", callback1);
            delete jfinishCalPointCallback;
        });
    } else {
        m_pFinishCalPointCallback = nullptr;
    }
    if (callback2) {
        JEyesInfoCallback *jeyesInfoCallback = new JEyesInfoCallback(callback2);
        m_pEyesInfoCallback = std::shared_ptr<IEyesInfoCallback>(jeyesInfoCallback, [=](IEyesInfoCallback *) {
            LOGI("Remove Old JEyesInfoCallback(%p)", callback2);
            delete jeyesInfoCallback;
        });
    } else {
        m_pEyesInfoCallback = nullptr;
    }
    result = m_pGearVrApi->FDGStartCalibrationPoint(pointIndex,FDG_CALLBACK_FCP(GearVrApiJni::DealFinishCalPointCallback,this),FDG_CALLBACK_CFDI(GearVrApiJni::DealCalibrationFDInfoCall,this));
    return result;
}
//void GearVrApiJni::FinishCalPointCallback(int pointIndex, int error,void* data)
//{
//    GearVrApiJni *pSDK = (GearVrApiJni *)data;
//    pSDK->DealFinishCalPointCallback(pointIndex,error);
//}
void GearVrApiJni::DealFinishCalPointCallback(int pointIndex, int error)
{
    if(m_pFinishCalPointCallback)
    {
        m_pFinishCalPointCallback->onFinishCalPoint(pointIndex,error);
    }
}
//void GearVrApiJni::CalibrationFDInfoCall(int pointIndex, int frameNum, Gear_Vr_Interface::FDG_FDInfo fd,int error,void* data)
//{
//    GearVrApiJni *pSDK = (GearVrApiJni *)data;
//    pSDK->DealCalibrationFDInfoCall(pointIndex,frameNum,fd,error);
//}
void GearVrApiJni::DealCalibrationFDInfoCall(int pointIndex,FDG_Interface::CalPointInfo info,int error)
{
    if(m_pEyesInfoCallback)
    {
        m_pEyesInfoCallback->onEyesInfo(pointIndex,info.seq,info.fd,info.percent,info.count,error);
    }
}

int GearVrApiJni::startTracking(jobject callback)
{
    int result = 0;
    if(callback)
    {
        JTrackingCallback *jtrackingCallback = new JTrackingCallback(callback);
        m_pTrackingCallback = std::shared_ptr<ITrackingCallback>(jtrackingCallback, [=](ITrackingCallback *) {
            LOGI("Remove Old JTrackingCallback(%p)", callback);
            delete jtrackingCallback;
        });
    }
    else
    {
        m_pTrackingCallback = nullptr;
    }
    result = m_pGearVrApi->FDGStartTracking(FDG_CALLBACK_GT(GearVrApiJni::DealGazeTracking,this));
    return result;
}
int GearVrApiJni::startTrackingWithoutCal(signed char* buff,int size,jobject callback)
{
    int result = 0;


    if(callback)
    {
        JTrackingCallback *jtrackingCallback = new JTrackingCallback(callback);
        m_pTrackingCallback = std::shared_ptr<ITrackingCallback>(jtrackingCallback, [=](ITrackingCallback *) {
            LOGI("Remove Old JTrackingCallback(%p)", callback);
            delete jtrackingCallback;
        });
    }
    else
    {
        m_pTrackingCallback = nullptr;
    }
    CalCoeInfo cal;
    memcpy(&cal,buff,size);

    result = m_pGearVrApi->FDGStartTrackingWithoutCal(cal,FDG_CALLBACK_GT(GearVrApiJni::DealGazeTracking,this));
    return result;
}
//void GearVrApiJni::GazeTracking(int frameNum, float x, float y,float re,Gear_Vr_Interface::FDG_FDInfo fd,int error,void* data)
//{
//    GearVrApiJni *pSDK = (GearVrApiJni *)data;
//    pSDK->DealGazeTracking(frameNum,x,y,re,fd,error);
//}
void GearVrApiJni::DealGazeTracking(int seq, FDG_Interface::GazePoint point,int error)
{
    if(m_pTrackingCallback)
    {
        m_pTrackingCallback->onGaze(seq,point.x,point.y,point.fd,error);
    }
}