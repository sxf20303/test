//
// Created by 7Invensun on 2017/2/8.
//

#include "GearVrApi.h"
#include "FD_Gaze_API.h"
#include "Camera_API.h"
#include "Config.h"
#include <memory>

using namespace FDG_Interface;
using namespace Camera_Interface;

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

void RotateAndFlip(uint8_t* pDstData, const uint8_t* pSrcData, int32_t nWidth, int32_t nHeight,
                   int32_t nRoType, bool bFlip, int32_t nChannel /*= 1*/)
{
    if (!bFlip)
    {
        switch (nRoType)
        {
            case 0://顺时针旋转90
            {
                for (int32_t i = 0; i < nWidth; i++)
                {
                    for (int32_t j = 0, n = nHeight - 1; j < nHeight; j++, n--)
                    {
                        pDstData[i*nHeight + j] = pSrcData[n*nWidth + i];
                    }
                }
                break;
            }
            case 1://顺时针旋转180
            {
                for (int32_t i = 0, m = nHeight - 1; i < nHeight; i++, m--)
                {
                    for (int32_t j = 0, n = nWidth - 1; j < nWidth; j++, n--)
                    {
                        pDstData[i*nWidth + j] = pSrcData[m*nWidth + n];
                    }
                }
                break;
            }
            case 2://顺时针旋转270、逆时针旋转90
            {
                for (int32_t i = 0, m = nWidth - 1; i < nWidth; i++, m--)
                {
                    for (int32_t j = 0; j < nHeight; j++)
                    {
                        pDstData[i*nHeight + j] = pSrcData[j*nWidth + m];
                    }
                }
                break;
            }
            case 3://不翻转（原图拷贝）
            {
                memcpy(pDstData, pSrcData, nChannel*nWidth*nHeight);
                break;
            }
            default:
                break;
        }
    }
    else//翻转
    {
        switch (nRoType)
        {
            case 0://1、顺时针旋转90+上下翻转
            {
                for (int32_t i = 0, m = nWidth - 1; i < nWidth; i++, m--)
                {
                    for (int32_t j = 0, n = nHeight - 1; j < nHeight; j++, n--)
                    {
                        pDstData[i*nHeight + j] = pSrcData[n*nWidth + m];
                    }
                }
                break;
            }
            case 1://顺时针旋转180+上下翻转
            {
                for (int32_t i = 0; i < nHeight; i++)
                {
                    for (int32_t j = 0, n = nWidth - 1; j < nWidth; j++, n--)
                    {
                        pDstData[(i*nWidth + j)] = pSrcData[(i*nWidth + n)];
                    }
                }
                break;
            }
            case 2://顺时针旋转270、逆时针旋转90+上下翻转
            {
                for (int32_t i = 0, m = 0; i < nWidth; i++, m++)
                {
                    for (int32_t j = 0, n = 0; j < nHeight; j++, n++)
                    {
                        pDstData[(i*nHeight + j)] = pSrcData[(n*nWidth + m)];
                    }
                }
                break;
            }
            case 3://上下翻转
            {
                for (int32_t i = 0, m = nHeight - 1; i < nHeight; i++, m--)
                {
                    memcpy(&pDstData[i*nWidth], &pSrcData[m*nWidth], nWidth);
                }
                break;
            }
            default:
                break;
        }
    }
}

namespace Gear_Vr_Interface
{
    class GearVr_Obj : public GearVr_API
    {
    public:
        virtual int InitGearVr(FDGParam& param);
        virtual int ConnectCamera(int vid, int pid, int fd, const char *usbfs);
        virtual int DisconnectCamera();
//        virtual int SetCameraPreviewSize(int width,int height,int mode);
        virtual int StartCamera(CameraFrameCallback call,void* data);
        virtual int StopCamera();
        virtual void CaptureSnapshot(const char* path);

        virtual int FDGStartCalibration(FDG_Interface::CalAllPointPos& points, FDG_Interface::FinishCalibrationCallback call);
        virtual int FDGStartCalibrationPoint(int pointIndex, FDG_Interface::FinishCalibrationPointCallback call,FDG_Interface::CalibrationFDInfoCallback FdCall);
        virtual int FDGStopCalibrationPoint();
        virtual int FDGStartTracking(FDG_Interface::GazeTrackingCallback call);
        virtual int FDGStartTrackingWithoutCal(FDG_Interface::CalCoeInfo& cal,FDG_Interface::GazeTrackingCallback call);
        virtual int FDGStopTracking();
        //线程开始和结束时的回调函数，给android下java用，c++不需要，调用在Init之前
        virtual int FDGSetThreadCallback(FDG_Interface::ThreadStartEndCallback start,FDG_Interface::ThreadStartEndCallback end);

    public:
        GearVr_Obj()
        {
            m_pFDG = nullptr;
            m_pCamera = nullptr;
            m_pFDG = std::shared_ptr<FDG_Interface::FD_Gaze_API>(FDG_Interface::createFDG(),FDG_Interface::releaseFDG);
            m_pCamera = std::shared_ptr<Camera_Interface::Camera_API>(Camera_Interface::createCamera(),Camera_Interface::releaseCamera);
            m_CameraFrameCallback = nullptr;
            m_reservedCameraFrameCallback = nullptr;
            m_rotate_enable = true;
            m_rotate_type = 2;
            m_flip_enable = true;

        }

        static void CameraFrameCall(unsigned char* pImage,int imageSize, int width, int height, int frameNum,int error,void* data);
//        static void FDGFinishCalibrationCallback(int totelPoint, CalCoeInfo cal, int error,void* data);
//        static void FDGFinishCalibrationPointCallback(int pointIndex, int error,void* data);
//        static void FDGCalibrationFDInfoCallback(int pointIndex, int frameNum, FDInfo fd,int error,void* data);
//        static void FDGGazeTrackingCallback(int frameNum, float x, float y,float re,FDInfo fd,int error,void* data);

        void DealFrameCallback(unsigned char* pImage,int imageSize, int width, int height, int frameNum,int error);
        void DealFDGFinishCalibrationCallback(int totelPoint, CalCoeInfo* cal, int error);
        void DealFDGFinishCalibrationPointCallback(int pointIndex, int error);
        void DealFDGCalibrationFDInfoCallback(int pointIndex,CalPointInfo info,int error);
        void DealFDGGazeTrackingCallback(int seq, GazePoint point,int error);

        int PreDealImage(unsigned char* pDstImage,int& nDstImageSize,const unsigned char* pSrcImage,int nSrcImageSize,int width,int height,void* data);

    private:
        std::shared_ptr<FDG_Interface::FD_Gaze_API> m_pFDG;
        std::shared_ptr<Camera_Interface::Camera_API> m_pCamera;
        Config m_cfg;

        CameraFrameCallback m_CameraFrameCallback;
        void* m_reservedCameraFrameCallback;
        FinishCalibrationCallback m_FDGFinishCalibration;
//        void* m_reservedFDGFinishCalibration;
        FinishCalibrationPointCallback m_FDGFinishCalibrationPoint;
//        void* m_reservedFDGFinishCalibrationPoint;
        CalibrationFDInfoCallback m_FDGCalibrationFDInfoCall;
//        void* m_reservedFDGCalibrationFDInfoCall;
        GazeTrackingCallback m_FDGGazeTracking;
//        void* m_reservedFDGGazeTracking;

        bool m_rotate_enable;
        int m_rotate_type;
        bool m_flip_enable;
    };
    int GearVr_Obj::InitGearVr(FDGParam& param)
    {

        m_cfg.Load(param.szPath);
        FDConfig fdCfg;
        fdCfg.c_center_x = m_cfg.m_nCenterX;
        fdCfg.c_center_y = m_cfg.m_nCenterY;
        fdCfg.c_radius = m_cfg.m_nRadius;
        fdCfg.c_rowsMask = m_cfg.m_nROIRectW;
        fdCfg.c_colsMask = m_cfg.m_nROTRectH;
        fdCfg.c_cutBellow = m_cfg.m_nCutBellowHeight;
        fdCfg.c_ruRadius = m_cfg.m_nRightUpRadius;
        fdCfg.c_leftOrRight = m_cfg.m_nLeftOrRight;
        fdCfg.c_glass = false;
        fdCfg.has_glass = m_cfg.m_nHasClass;
        GazeConfig gazeCfg;
        gazeCfg.device = m_cfg.m_nDeviceType;
        gazeCfg.method = m_cfg.m_nMethodType;
        gazeCfg.ex = m_cfg.m_fSmoothEx;
        gazeCfg.th = m_cfg.m_fSmoothTh;
        gazeCfg.tl = m_cfg.m_nTimeLimit;

        m_pFDG->Init(fdCfg,gazeCfg,param.nwidth,param.nheight,param.nchannel,param.trackingMode != TRACKING_MODE_RIGHTTRACK ? true : false);

        return 0;
    }
    int GearVr_Obj::ConnectCamera(int vid, int pid, int fd, const char *usbfs)
    {
        return m_pCamera->Connect(vid,pid,fd,usbfs);
    }
    int GearVr_Obj::DisconnectCamera()
    {
        return m_pCamera->Disconnect();
    }
//    int GearVr_Obj::SetCameraPreviewSize(int width,int height,int mode)
//    {
//        return m_pCamera->SetPreviewSize(width,height,mode);
//    }
    int GearVr_Obj::StartCamera(CameraFrameCallback call,void* data)
    {
        m_CameraFrameCallback = call;
        m_reservedCameraFrameCallback = data;
        return m_pCamera->StartCamera(CameraFrameCall, this);
    }
    void GearVr_Obj::CameraFrameCall(unsigned char* pImage,int imageSize, int width, int height, int frameNum,int error,void* data)
    {
        if(data)
        {
            GearVr_Obj* pGearVr = (GearVr_Obj*)data;
            pGearVr->DealFrameCallback(pImage,imageSize,width,height,frameNum,error);
        }
    }
    void GearVr_Obj::DealFrameCallback(unsigned char* pImage,int imageSize, int width, int height, int frameNum,int error)
    {
        //deal image
        std::shared_ptr<uint8_t> imageCopy = std::shared_ptr<uint8_t>(new uint8_t[imageSize],[](uint8_t *p){if(p) delete [] p; p = nullptr;});
        memcpy( imageCopy.get(), pImage, imageSize);
        int imgWidth = width;
        int imgHeight = height;
        int bitCount = 8;

        RotateAndFlip(imageCopy.get(),pImage,width,height,m_rotate_type,m_flip_enable,1);

//        if (m_rotate_enable) {
//            // 利用pDealBuffer做临时buffer进行图像旋转
//            switch (m_rotate_type) {
//                case 0:    //顺时针90度旋转
//                    RotateImage(imageCopy.get(), pImage, width, height,
//                                bitCount >> 3);
//                    imgWidth = height;
//                    imgHeight = width;
//                    break;
//
//                case 1:    //顺时针180度旋转
//                    RotateImage_180(imageCopy.get(), pImage, width, height,
//                                    bitCount >> 3);
//                    break;
//
//                case 2:    //顺时针270度（逆时针90）旋转
//                    RotateImage(imageCopy.get(), pImage, width, height,
//                                bitCount >> 3, false);
//                    imgWidth = height;
//                    imgHeight = width;
//                    break;
//
//                default:
//                    break;
//            }
//        }
//
//        std::shared_ptr<uint8_t> imageCopy2 = std::shared_ptr<uint8_t>(new uint8_t[imageSize]);
//        memcpy( imageCopy2.get(), imageCopy.get(), imageSize);
//        if(m_flip_enable){
//            FlipImage_Vertical(imageCopy.get(), imageCopy2.get(), imgWidth, imgHeight, bitCount >> 3);
//        }

        m_pFDG->DealImage(imageCopy.get(),imageSize,frameNum);
        if(m_CameraFrameCallback)
        {
            m_CameraFrameCallback(imageCopy.get(),imageSize,width,height,frameNum,error,m_reservedCameraFrameCallback);
        }

    }
    int GearVr_Obj::PreDealImage(unsigned char* pDstImage,int& nDstImageSize,const unsigned char* pSrcImage,int nSrcImageSize,int width,int height,void* data)
    {

        return 0;
    }

    int GearVr_Obj::StopCamera()
    {
        m_pCamera->StopCamera();
        return 0;
    }
    void GearVr_Obj::CaptureSnapshot(const char* path)
    {
        m_pCamera->CaptureSnapshot(path);
    }

    int GearVr_Obj::FDGStartCalibration(FDG_Interface::CalAllPointPos& points, FDG_Interface::FinishCalibrationCallback call)
    {
        CalAllPointPos pos;
        memcpy(&pos,&points,sizeof(pos));
        m_FDGFinishCalibration = call;
        return m_pFDG->StartCalibration(pos,FDG_CALLBACK_FC(GearVr_Obj::DealFDGFinishCalibrationCallback,this));
    }
//    void GearVr_Obj::FDGFinishCalibrationCallback(int totelPoint, CalCoeInfo cal, int error,void* data)
//    {
//        if(data)
//        {
//            GearVr_Obj* pGearVr = (GearVr_Obj*)data;
//            pGearVr->DealFDGFinishCalibrationCallback(totelPoint,cal,error);
//        }
//    }
    void GearVr_Obj::DealFDGFinishCalibrationCallback(int totelPoint, CalCoeInfo* cal, int error)
    {
        if(m_FDGFinishCalibration)
        {
//            FDG_CalCoeInfo coe;
//            memcpy(&coe,&cal,sizeof(coe));
            m_FDGFinishCalibration(totelPoint,cal,error);
        }
    }
    int GearVr_Obj::FDGStartCalibrationPoint(int pointIndex, FDG_Interface::FinishCalibrationPointCallback call,FDG_Interface::CalibrationFDInfoCallback FdCall)
    {
        m_FDGFinishCalibrationPoint = call;
        m_FDGCalibrationFDInfoCall = FdCall;
        return m_pFDG->StartCalibrationPoint(pointIndex,FDG_CALLBACK_FCP(GearVr_Obj::DealFDGFinishCalibrationPointCallback,this),FDG_CALLBACK_CFDI(GearVr_Obj::DealFDGCalibrationFDInfoCallback,this));
    }
//    void GearVr_Obj::FDGFinishCalibrationPointCallback(int pointIndex, int error,void* data)
//    {
//        if(data)
//        {
//            GearVr_Obj* pGearVr = (GearVr_Obj*)data;
//            pGearVr->DealFDGFinishCalibrationPointCallback(pointIndex,error);
//        }
//    }
    void GearVr_Obj::DealFDGFinishCalibrationPointCallback(int pointIndex, int error)
    {
        if(m_FDGFinishCalibrationPoint)
        {
            m_FDGFinishCalibrationPoint(pointIndex,error);
        }
    }
//    void GearVr_Obj::FDGCalibrationFDInfoCallback(int pointIndex, int frameNum, FDInfo fd,int error,void* data)
//    {
//        if(data)
//        {
//            GearVr_Obj* pGearVr = (GearVr_Obj*)data;
//            pGearVr->DealFDGCalibrationFDInfoCallback(pointIndex,frameNum,fd,error);
//        }
//    }
    void GearVr_Obj::DealFDGCalibrationFDInfoCallback(int pointIndex,CalPointInfo info,int error)
    {
        if(m_FDGCalibrationFDInfoCall)
        {
//            FDG_FDInfo fdd;
//            memcpy(&fdd,&fd,sizeof(fdd));
            m_FDGCalibrationFDInfoCall(pointIndex,info,error);
        }
    }
    int GearVr_Obj::FDGStopCalibrationPoint()
    {
        return m_pFDG->StopCalibrationPoint();
    }

    int GearVr_Obj::FDGStartTracking(FDG_Interface::GazeTrackingCallback call)
    {
        m_FDGGazeTracking = call;
        return m_pFDG->StartTracking(FDG_CALLBACK_GT(GearVr_Obj::DealFDGGazeTrackingCallback,this));
    }
//    void GearVr_Obj::FDGGazeTrackingCallback(int frameNum, float x, float y,float re,FDInfo fd,int error,void* data)
//    {
//        if(data)
//        {
//            GearVr_Obj* pGearVr = (GearVr_Obj*)data;
//            pGearVr->DealFDGGazeTrackingCallback(frameNum,x,y,re,fd,error);
//        }
//    }
    void GearVr_Obj::DealFDGGazeTrackingCallback(int seq, GazePoint point,int error)
    {
        if(m_FDGGazeTracking)
        {
            m_FDGGazeTracking(seq,point,error);
        }
    }
    int GearVr_Obj::FDGStartTrackingWithoutCal(FDG_Interface::CalCoeInfo& cal,FDG_Interface::GazeTrackingCallback call)
    {
        m_FDGGazeTracking = call;
        return m_pFDG->StartTrackingWithoutCal(cal,FDG_CALLBACK_GT(GearVr_Obj::DealFDGGazeTrackingCallback,this));
    }
    int GearVr_Obj::FDGStopTracking()
    {
        return 0;
    }
    int GearVr_Obj::FDGSetThreadCallback(FDG_Interface::ThreadStartEndCallback start,FDG_Interface::ThreadStartEndCallback end)
    {
        return m_pFDG->SetThreadCallback(start,end);
    }

    GearVr_API* createGearVrApi()
    {
        return new GearVr_Obj();
    }
    void releaseGearVrApi(GearVr_API* pGearVr)
    {
        if(pGearVr)
        {
            delete pGearVr;
            pGearVr = nullptr;
        }
    }

}
