//
// Created by 韩呈杰 on 16/1/29.
//
#include <memory>
#include <cassert>
#include "JCallback.h"
#include "JEnv.h"
#include "Log.h"
#include "Class.h"
#include "../GVR_API/GearVrApi.h"


static inline JNIEnv *env() {
    return JEnv::env();
}


thread_local jmethodID _methodID = nullptr;
thread_local jfieldID  _fieldIDX = nullptr;
thread_local jfieldID  _fieldIDY = nullptr;

std::shared_ptr<_jobject> toJava(JNIEnv *env, float x, float y)
{
    if (_methodID == nullptr) {
        _methodID = env->GetMethodID(getClassInfo(EnumClass::PointF).clazz, "<init>", "(FF)V");

        //LOGI("TID[%s][JPointF::toJava] Get PointF Construct Method ID: %p", std::this_thread::get_id_string(), _methodID);
    }

    return std::shared_ptr<_jobject>(env->NewObject(getClassInfo(EnumClass::PointF).clazz, _methodID, x, y),
                                         [=](jobject o){env->DeleteLocalRef(o); });

}
void fromJava(JNIEnv *env, jobject pt,float& x, float& y)
{
    if (_fieldIDX == nullptr) {
        _fieldIDX = env->GetFieldID(getClassInfo(EnumClass::PointF).clazz, "x", "F");
        _fieldIDY = env->GetFieldID(getClassInfo(EnumClass::PointF).clazz, "y", "F");
    }
    x = env->GetFloatField(pt, _fieldIDX);
    y = env->GetFloatField(pt, _fieldIDY);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JCallback::JCallback(jobject callbackObj, const char *method, const char *sig) {

    _callbackObj = env()->NewGlobalRef(callbackObj);
    std::shared_ptr<_jclass> clazz(env()->GetObjectClass(_callbackObj), [=](jclass cls) {
        env()->DeleteLocalRef(cls);
    });
    _methodID = env()->GetMethodID(clazz.get(), method, sig);
    LOGI("[%p]%s%s => %p", callbackObj, method, sig, _methodID);
}

JCallback::~JCallback() {
    env()->DeleteGlobalRef(_callbackObj);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
JImageCallback::JImageCallback(jobject callbackObj) :
    JCallback(callbackObj, "onImage", "([BIII)V") {
}

JImageCallback::~JImageCallback() {
}


void JImageCallback::onImage(const unsigned char *image, int imageSize, int width, int height, int seq) {
    JEnv::ExceptionGuard _guard;
	//LOGI("%s(): image: %dx%d [%d %d %d %d %d ...]", __FUNCTION__, width, height, image[0], image[1], image[2], image[3],
	//	 image[4]);
	//LOGI("%s(): env:%p", __FUNCTION__, env());

	//int imageSize = width * height;
    assert(imageSize >= width * height);
    std::shared_ptr<_jbyteArray> arr = std::shared_ptr<_jbyteArray>(env()->NewByteArray(imageSize),
        [this](jbyteArray p) { env()->DeleteLocalRef(p); });
    if (arr.get()) {
        env()->SetByteArrayRegion(arr.get(), 0, imageSize, (const jbyte *)image);
        env()->CallVoidMethod(_callbackObj, _methodID, arr.get(), width, height, seq);
    }
}

JFinishCalCallback::JFinishCalCallback(jobject callbackObj) :
        JCallback(callbackObj, "onFinishCal", "(II[B)V")
{
}

JFinishCalCallback::~JFinishCalCallback()
{
}
void JFinishCalCallback::onFinishCal(int totelPoint, unsigned char * buff,int size,int error)
{
    JEnv::ExceptionGuard _guard;
    std::shared_ptr<_jbyteArray> arr = std::shared_ptr<_jbyteArray>(env()->NewByteArray(size),
                                                                    [this](jbyteArray p) { env()->DeleteLocalRef(p); });
    if (arr.get())
    {
        env()->SetByteArrayRegion(arr.get(), 0, size, (const jbyte *)buff);
        env()->CallVoidMethod(_callbackObj, _methodID, totelPoint,error,arr.get());
    }
}

JFinishCalPointCallback::JFinishCalPointCallback(jobject callbackObj) :
        JCallback(callbackObj, "onFinishCalPoint", "(II)V")
{
}
JFinishCalPointCallback::~JFinishCalPointCallback()
{
}
void JFinishCalPointCallback::onFinishCalPoint(int pointIndex,int error)
{
    JEnv::ExceptionGuard _guard;
    env()->CallVoidMethod(_callbackObj, _methodID, pointIndex,error);
}

JEyesInfoCallback::JEyesInfoCallback(jobject callbackObj) :
        JCallback(callbackObj, "onEyesInfo", "(IILcom/seveninvensun/sdk/EyesInfo;III)V")
{
    _eyesInfoInitID = env()->GetMethodID(getClassInfo(EnumClass::EyesInfo).clazz, "<init>", "(Lcom/seveninvensun/sdk/PointF;Lcom/seveninvensun/sdk/PointF;)V");
    assert(_eyesInfoInitID != 0);
}
JEyesInfoCallback::~JEyesInfoCallback()
{
}
void JEyesInfoCallback::onEyesInfo(int pointIndex,int seq,FDG_Interface::FDInfo* eyes,int percent,int count,int error)
{
    JEnv::ExceptionGuard _guard;
    if(eyes == nullptr || eyes->processOK == 0)
    {
//        LOGI("onEyesInfo  fd is failed");
        jclass clsEyes = getClassInfo(EnumClass::EyesInfo).clazz;
        auto leftEye  = std::shared_ptr<_jobject>();
        auto rightEye = std::shared_ptr<_jobject>();
        jobject jEyeInfo = env()->NewObject(clsEyes, _eyesInfoInitID, leftEye.get(), rightEye.get());
        auto item = std::shared_ptr<_jobject>(jEyeInfo, [](jobject p) {if (p) env()->DeleteLocalRef(p);});
        env()->CallVoidMethod(_callbackObj, _methodID,pointIndex,seq,item.get(),percent,count,error);
        return;
    }
    jclass clsEyes = getClassInfo(EnumClass::EyesInfo).clazz;
    auto leftEye  = eyes->realLeftEyeFound ? toJava(env(),eyes->leftPupil[1],eyes->leftPupil[2]): (std::shared_ptr<_jobject>());
    auto rightEye = eyes->realRightEyeFound ? toJava(env(),eyes->rightPupil[1],eyes->rightPupil[2]):std::shared_ptr<_jobject>();
    jobject jEyeInfo = env()->NewObject(clsEyes, _eyesInfoInitID, leftEye.get(), rightEye.get());
    auto item = std::shared_ptr<_jobject>(jEyeInfo, [](jobject p) {if (p) env()->DeleteLocalRef(p);});
    env()->CallVoidMethod(_callbackObj, _methodID,pointIndex,seq,item.get(),percent,count,error);
}
JTrackingCallback::JTrackingCallback(jobject callbackObj) :
        JCallback(callbackObj, "onGaze", "(Lcom/seveninvensun/sdk/EyeGaze;Lcom/seveninvensun/sdk/EyesInfo;II)V")
{
    _eyesInfoInitID = env()->GetMethodID(getClassInfo(EnumClass::EyesInfo).clazz, "<init>", "(Lcom/seveninvensun/sdk/PointF;Lcom/seveninvensun/sdk/PointF;)V");
    assert(_eyesInfoInitID != 0);
    _eyeGazeInitID = env()->GetMethodID(getClassInfo(EnumClass::EyeGaze).clazz, "<init>", "(Lcom/seveninvensun/sdk/PointF;Lcom/seveninvensun/sdk/PointF;Lcom/seveninvensun/sdk/PointF;Lcom/seveninvensun/sdk/PointF;)V");
    assert(_eyeGazeInitID != 0);
}
JTrackingCallback::~JTrackingCallback()
{
}
void JTrackingCallback::onGaze(int seq,float x, float y,FDG_Interface::FDInfo* eyes,int error)
{
    JEnv::ExceptionGuard _guard;

//    if(eyes.processOK == 0)
//    {
//        LOGI("onEyesInfo  fd is failed");
//        return;
//    }
    jclass clsGaze = getClassInfo(EnumClass::EyeGaze).clazz;
    auto gazeleft  = eyes->realLeftEyeFound ? toJava(env(),x,y):std::shared_ptr<_jobject>();
    auto gazeRight  = eyes->realRightEyeFound ? toJava(env(),x,y):std::shared_ptr<_jobject>();
    auto gazeleftSmooth  = eyes->realLeftEyeFound ? toJava(env(),x,y):std::shared_ptr<_jobject>();
    auto gazeRightSmooth  = eyes->realRightEyeFound ? toJava(env(),x,y):std::shared_ptr<_jobject>();
    jobject jEyeGaze = env()->NewObject(clsGaze, _eyeGazeInitID, gazeleft.get(), gazeRight.get(),gazeleftSmooth.get(),gazeRightSmooth.get());
    auto eyegaze = std::shared_ptr<_jobject>(jEyeGaze, [](jobject p) {if (p) env()->DeleteLocalRef(p);});

    jclass clsEyes = getClassInfo(EnumClass::EyesInfo).clazz;
    auto leftEye  = eyes->realLeftEyeFound ? toJava(env(),eyes->leftPupil[1],eyes->leftPupil[2]):std::shared_ptr<_jobject>();
    auto rightEye = eyes->realRightEyeFound ? toJava(env(),eyes->rightPupil[1],eyes->rightPupil[2]):std::shared_ptr<_jobject>();
    jobject jEyeInfo = env()->NewObject(clsEyes, _eyesInfoInitID, leftEye.get(), rightEye.get());
    auto item = std::shared_ptr<_jobject>(jEyeInfo, [](jobject p) {if (p) env()->DeleteLocalRef(p);});

    env()->CallVoidMethod(_callbackObj, _methodID,eyegaze.get(),item.get(),seq,error);
}
