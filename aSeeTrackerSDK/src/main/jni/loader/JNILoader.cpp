//
// Created by 韩呈杰 on 16/1/27.
//

//#ifdef __ANDROID__
#include <sstream>
#include <cassert>
#include <thread>
#include "Log.h"
#include "JNILoader.h"

void initJClass(JNIEnv *env);
void releaseJClass(JNIEnv *env);

const NativeClass &GearVrApi_Register();
//const NativeClass &com_aSeeMobileSDK_aSeeTracker();
//const NativeClass &com_aSeeMobileSDK_Debug_aSeeTracker();
//
////const NativeClass &com_aSeeMobileSDK_UVCCamera();
//const NativeClass &com_aSeeMobileSDK_Debug_VirtualCamera();

typedef const NativeClass &(*NativeClassGetter)();
static const NativeClassGetter getJNIClass[] = {
        GearVrApi_Register,
//        com_aSeeMobileSDK_aSeeTracker,
//        com_aSeeMobileSDK_Debug_aSeeTracker,
//
//        //com_aSeeMobileSDK_UVCCamera,
//        com_aSeeMobileSDK_Debug_VirtualCamera,
};

static int registerNativeClass(JNIEnv *env, const char *className, JNINativeMethod *methods, int numMethods)
{
    jclass clazz = env->FindClass(className);
    if (clazz == nullptr) {
        return JNI_FALSE;
    }

    if (env->RegisterNatives(clazz, methods, numMethods) < 0)
    {
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

namespace JEnv {
    extern JavaVM *vm;
}
extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {

    JEnv::vm = vm;

    LOGI(__FUNCTION__);

    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK)
    {
        return -1;
    }
    assert(env != nullptr);

    // 初始化需要在异步线程中使用的Java的类
    initJClass(env);

    for (const NativeClassGetter f: getJNIClass)
    {
        const NativeClass &cls = f();
        if (!registerNativeClass(env, cls.className, cls.methods, cls.methodCount))
        {
            return -1;
        }
    }

    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) {
    LOGI(__FUNCTION__);

    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **)&env, JNI_VERSION_1_6) != JNI_OK)
    {
        return ;
    }

    releaseJClass(env);
}
