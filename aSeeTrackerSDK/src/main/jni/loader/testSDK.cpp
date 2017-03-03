//
// Created by 7Invensun on 2017/1/18.
//

#include <stdlib.h>  
#include <string.h>  
#include <stdio.h>  
#include <jni.h>  
#include <assert.h>
#include  "Log.h"
#include "JNILoader.h"
#include "GearVrApi.h"

extern "C" JNIEXPORT void JNICALL testSDK_init(JNIEnv *env,jobject o, jint num) {
    LOGI("testSDK_init");
}

extern "C" JNIEXPORT jstring JNICALL testSDK_getErrorMsg(JNIEnv *env,jobject o) {
    LOGI("testSDK_getErrorMsg");
//    char* str = new char[128];
//    memcpy(str,"testSDK_getErrorMsg!",128);
//    jstring ret = env->NewStringUTF(str);
    char* str = "testSDK_getErrorMsg!";
    jstring ret = env->NewStringUTF(str);
    return ret;
}

extern "C" JNIEXPORT jint JNICALL testSDK_printInfo(JNIEnv *env,jobject o,jstring str) {
    LOGI("testSDK_init");
//    FDG_Interface::FD_Gaze_API* p = FDG_Interface::createFDG();
//    p->StopTracking();
    return 1;
}

const NativeClass &testSDK_Register() {
    static const char *className = "com/seveninvensun/sdk/ASeeTracker";//修改JNI包名
    static JNINativeMethod methods[] = {
            {"init","(I)V",(void *) &testSDK_init},
            {"getErrorMsg","()Ljava/lang/String;",(void*)&testSDK_getErrorMsg},
            {"printInfo","(Ljava/lang/String;)I",(void*)&testSDK_printInfo},
//            {"getErrorMsg","(I;)Ljava/lang/String;",(void*)&testSDK_getErrorMsg},
//            {"initSDK",				"(Ljava/lang/String;)V", (void *) &aSeeTracker_initSDK},
//            {"create",              "()J",      (void *) &aSeeTracker_create},
//            {"release",             "(J)I",     (void *) &aSeeTracker_release},
//            {"connect",             "(JJ)V",    (void *) &aSeeTracker_connect},
//            {"disconnect",          "(J)V",     (void *) &aSeeTracker_disconnect},
//            {"setImageCallback",    "(JLcom/seveninvensun/aseemobilesdk/ImageCallback;)V", (void *) &aSeeTracker_setImageCallback},
//            {"setEyeDetectionCallback", "(JLcom/seveninvensun/aseemobilesdk/EyeDetectionCallback;)V", (void *) &aSeeTracker_setEyeDetectionCallback},
//
//            {"setCalibCoe",         "(J[B)V",   (void *) &aSeeTracker_setCalibCoe},
//            {"startTracking" ,       "(JLcom/seveninvensun/aseemobilesdk/TrackingCallback;I)V", (void *) &aSeeTracker_startTracking},
//            {"stopTracking",        "(J)V",     (void *) &aSeeTracker_stopTracking},
//
//            {"startCalibration",    "(JII)V",   (void *) &aSeeTracker_startCalibration},
//            {"completeCalibration", "(J)[B",    (void *) &aSeeTracker_completeCalibration},
//            {"cancelCalibration",   "(J)V",     (void *) &aSeeTracker_cancelCalibration},
//            {"startPoint",          "(JILcom/seveninvensun/aseemobilesdk/PointF;)V", (void *) &aSeeTracker_startPoint},
//            {"completePoint",       "(JI)Z",    (void *) &aSeeTracker_completePoint},
    };
    static const NativeClass _cls = {className, (JNINativeMethod *) methods, sizeof(methods) / sizeof(methods[0])};

    return _cls;
}