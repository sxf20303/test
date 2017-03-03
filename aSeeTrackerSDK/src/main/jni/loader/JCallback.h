//
// Created by 韩呈杰 on 16/1/29.
//

#ifndef ASEEMOBILESDK_JCALLBACK_H
#define ASEEMOBILESDK_JCALLBACK_H
#include <jni.h>
//#include "stdint.h"
#include "JEnv.h"
#include "GearVrApi.h"

std::shared_ptr<_jobject> toJava(JNIEnv *env, float x, float y);
void fromJava(JNIEnv *env, jobject pt,float& x, float& y);


struct IImageCallback
{
    virtual void onImage(const unsigned char *image, int imageSize, int width, int height, int seq) = 0;
};
struct IFinishCalCallback
{
    virtual void onFinishCal(int totelPoint, unsigned char * buff,int size,int error) = 0;
};
struct IFinishCalPointCallback
{
    virtual void onFinishCalPoint(int pointIndex,int error) = 0;
};
struct IEyesInfoCallback
{
    virtual void onEyesInfo(int pointIndex,int seq,FDG_Interface::FDInfo* eyes,int percent,int count,int error) = 0;
};
struct ITrackingCallback
{
    virtual void onGaze(int seq,float x, float y,FDG_Interface::FDInfo* eyes,int error) = 0;
};
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
class JCallback {
protected:
    jobject _callbackObj;
    jmethodID _methodID;
public:
    JCallback(jobject callbackObj, const char *method, const char *sig);
    virtual ~JCallback();

    virtual bool isValid() const {
        return _methodID != nullptr;
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
class JImageCallback : public JCallback, public IImageCallback {
public:
    JImageCallback(jobject cb);
    ~JImageCallback();

    virtual void onImage(const unsigned char *image, int imageSize, int width, int height, int seq);
};

class JFinishCalCallback : public JCallback, public IFinishCalCallback {
public:
    JFinishCalCallback(jobject cb);
    ~JFinishCalCallback();

    virtual void onFinishCal(int totelPoint, unsigned char * buff,int size,int error);

};
class JFinishCalPointCallback : public JCallback, public IFinishCalPointCallback {
public:
    JFinishCalPointCallback(jobject cb);
    ~JFinishCalPointCallback();

    virtual void onFinishCalPoint(int pointIndex,int error);

};
class JEyesInfoCallback : public JCallback, public IEyesInfoCallback {
    jmethodID _eyesInfoInitID;
public:
    JEyesInfoCallback(jobject cb);
    ~JEyesInfoCallback();

    virtual void onEyesInfo(int pointIndex,int seq,FDG_Interface::FDInfo* eyes,int percent,int count,int error);
};
class JTrackingCallback : public JCallback, public ITrackingCallback {
    jmethodID _eyesInfoInitID;
    jmethodID _eyeGazeInitID;
public:
    JTrackingCallback(jobject cb);
    ~JTrackingCallback();

    virtual void onGaze(int seq,float x, float y,FDG_Interface::FDInfo* eyes,int error);
};

#endif //ASEEMOBILESDK_JCALLBACK_H
