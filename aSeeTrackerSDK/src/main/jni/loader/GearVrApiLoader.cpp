//
// Created by 7Invensun on 2017/2/13.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <assert.h>
#include "Log.h"
#include "JNILoader.h"
#include "GearVrApiJni.h"


extern "C" JNIEXPORT jlong JNICALL GearVrApi_createSDK(JNIEnv *env,jclass o) {
    LOGI("GearVrApi_createSDK");
    GearVrApiJni *p = new GearVrApiJni();
    p->init();
    jlong handle = reinterpret_cast <jlong>(p);
    LOGI("createSDK: %p, to jlong: 0x%llx", p, handle);
    return handle;
}
extern "C" JNIEXPORT jint JNICALL GearVrApi_releaseSDK(JNIEnv *env,jclass o,jlong handle) {
    LOGI("GearVrApi_releaseSDK");
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    if (pSDK) {
        delete pSDK;
    }
    return 0;
}
extern "C" JNIEXPORT jint JNICALL GearVrApi_initSDK(JNIEnv *env,jobject o,jlong handle,jstring config_str) {
    LOGI("GearVrApi_initSDK");
    int result = JNI_ERR;
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    const char *c_fs = env->GetStringUTFChars(config_str, JNI_FALSE);
    if (pSDK)
    {
        Gear_Vr_Interface::FDGParam param;
        memset(&param, 0, sizeof(param));
        strncpy(param.szPath, c_fs, std::min(sizeof(param.szPath), strlen(c_fs)));

        param.nwidth   = 400;
        param.nheight  = 400;
        param.nchannel = 1;

        param.devTypeEnum  = DEVICE_TYPE_GEAR_VR;
        param.trackingMode = TRACKING_MODE_RIGHTTRACK;
        result =  pSDK->m_pGearVrApi->InitGearVr(param);
    }
    env->ReleaseStringUTFChars(config_str, c_fs);
    return result;
}

extern "C" JNIEXPORT jint JNICALL GearVrApi_connectCamera(JNIEnv *env,jobject o,jlong handle,jint vid, jint pid, jint fd,  jstring usbfs_str) {
    LOGI("GearVrApi_connectCamera");
    int result = JNI_ERR;
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    const char *c_usbfs = env->GetStringUTFChars(usbfs_str, JNI_FALSE);
    if (pSDK && fd > 0) {
        result =  pSDK->m_pGearVrApi->ConnectCamera(vid, pid, fd, c_usbfs);
    }
    env->ReleaseStringUTFChars(usbfs_str, c_usbfs);
    return result;
}
extern "C" JNIEXPORT jint JNICALL GearVrApi_disconnectCamera(JNIEnv *env,jobject o,jlong handle) {
    LOGI("GearVrApi_disconnectCamera");
    int result = JNI_ERR;
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    if (pSDK) {
        result =  pSDK->m_pGearVrApi->DisconnectCamera();
    }
    return result;
}
//extern "C" JNIEXPORT jint JNICALL GearVrApi_setCameraPreviewSize(JNIEnv *env,jobject o,jlong handle,jint width, jint height, jint mode) {
//    LOGI("GearVrApi_connectCamera");
//    int result = JNI_ERR;
//    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
//    if (pSDK) {
//        result =  pSDK->m_pGearVrApi->SetCameraPreviewSize(width,height,mode);
//    }
//    return result;
//}
extern "C" JNIEXPORT jint JNICALL GearVrApi_startCamera(JNIEnv *env,jobject o,jlong handle, jobject callback) {
    LOGI("GearVrApi_startCamera");
    int result = JNI_ERR;
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    result = pSDK->startCamera(callback);
    return result;
}
extern "C" JNIEXPORT jint JNICALL GearVrApi_stopCamera(JNIEnv *env,jobject o,jlong handle) {
    LOGI("GearVrApi_stopCamera");
    int result = JNI_ERR;
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    if (pSDK) {
        result =  pSDK->m_pGearVrApi->StopCamera();
    }
    return result;
}
extern "C" JNIEXPORT void JNICALL GearVrApi_captureSnapshot(JNIEnv *env,jobject o,jlong handle,jstring path) {
    LOGI("GearVrApi_captureSnapshot");
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    const char *c_path = env->GetStringUTFChars(path, JNI_FALSE);
    if (pSDK > 0) {
        pSDK->m_pGearVrApi->CaptureSnapshot(c_path);
    }
    env->ReleaseStringUTFChars(path, c_path);
}
extern "C" JNIEXPORT jint JNICALL GearVrApi_startCalibration(JNIEnv *env,jobject o,jlong handle,jint totelnum, jobject callback) {
    LOGI("GearVrApi_startCalibration");
    int result = JNI_ERR;
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    result = pSDK->startCalibration(totelnum,callback);
    return result;
}

extern "C" JNIEXPORT jint JNICALL GearVrApi_startCalibrationPoint(JNIEnv *env,jobject o,jlong handle,jint pointIndex, jobject callback1,jobject callback2) {
    LOGI("GearVrApi_startCalibrationPoint");
    int result = JNI_ERR;
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    result = pSDK->startCalibrationPoint(pointIndex,callback1,callback2);
    return result;
}
extern "C" JNIEXPORT jint JNICALL GearVrApi_stopCalibrationPoint(JNIEnv *env,jobject o,jlong handle) {
    LOGI("GearVrApi_stopCalibrationPoint");
    int result = JNI_ERR;
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    if (pSDK) {
        result =  pSDK->m_pGearVrApi->FDGStopCalibrationPoint();
    }
    return result;
}
extern "C" JNIEXPORT jint JNICALL GearVrApi_startTracking(JNIEnv *env,jobject o,jlong handle,jobject callback) {
    LOGI("GearVrApi_startTracking");
    int result = JNI_ERR;
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    result = pSDK->startTracking(callback);
    return result;
}
extern "C" JNIEXPORT jint JNICALL GearVrApi_startTrackingWithoutCal(JNIEnv *env,jobject o,jlong handle,jbyteArray coe,jobject callback) {
    LOGI("GearVrApi_startTrackingWithoutCal");
    int result = JNI_ERR;
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    // 验尺寸
    jsize size = env->GetArrayLength(coe);
    if (size <= 0) {
        LOGI("calibCoe size(%d) not valid!", (int)size);
        return -1;
    }
    // 获取标定参数（该数据由Java）
    std::shared_ptr<jbyte> buf(env->GetByteArrayElements(coe, nullptr), [=](jbyte *p) {
        env->ReleaseByteArrayElements(coe, p, JNI_ABORT);
    });

    result = pSDK->startTrackingWithoutCal(buf.get(),size,callback);
    return result;
}
extern "C" JNIEXPORT jint JNICALL GearVrApi_stopTracking(JNIEnv *env,jobject o,jlong handle) {
    LOGI("GearVrApi_stopTracking");
    int result = JNI_ERR;
    GearVrApiJni *pSDK = reinterpret_cast<GearVrApiJni *>(handle);
    if (pSDK) {
        result =  pSDK->m_pGearVrApi->FDGStopTracking();
    }
    return result;
}

const NativeClass &GearVrApi_Register() {
    static const char *className = "com/seveninvensun/sdk/ASeeTracker";//修改JNI包名
    static JNINativeMethod methods[] = {
            {"createSDK","()J",(void*)&GearVrApi_createSDK},
            {"releaseSDK","(J)I",(void*)&GearVrApi_releaseSDK},
            {"initSDK","(JLjava/lang/String;)I", (void *)&GearVrApi_initSDK},
            {"connectCamera","(JIIILjava/lang/String;)I",(void*)&GearVrApi_connectCamera},
            {"disconnectCamera","(J)I",(void*)&GearVrApi_disconnectCamera},
//            {"setCameraPreviewSize","(JIII)I",(void*)&GearVrApi_setCameraPreviewSize},
            {"startCamera","(JLcom/seveninvensun/sdk/ImageCallback;)I",(void*)&GearVrApi_startCamera},
            {"stopCamera","(J)I",(void*)&GearVrApi_stopCamera},
            {"captureSnapshot","(JLjava/lang/String;)V",(void*)&GearVrApi_captureSnapshot},
            {"startCalibration","(JILcom/seveninvensun/sdk/FinishCalCallback;)I",(void*)&GearVrApi_startCalibration},
            {"startCalibrationPoint","(JILcom/seveninvensun/sdk/FinishCalPointCallback;Lcom/seveninvensun/sdk/EyesInfoCallback;)I",(void*)&GearVrApi_startCalibrationPoint},
            {"stopCalibrationPoint","(J)I",(void*)&GearVrApi_stopCalibrationPoint},
            {"startTracking","(JLcom/seveninvensun/sdk/TrackingCallback;)I",(void*)&GearVrApi_startTracking},
            {"startTrackingWithoutCal","(J[BLcom/seveninvensun/sdk/TrackingCallback;)I",(void*)&GearVrApi_startTrackingWithoutCal},
            {"stopTracking","(J)I",(void*)&GearVrApi_stopTracking},


    };
    static const NativeClass _cls = {className, (JNINativeMethod *) methods, sizeof(methods) / sizeof(methods[0])};

    return _cls;
}