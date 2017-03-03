//
// Created by 韩呈杰 on 16/1/27.
//

#ifndef ASEEMOBILESDK_JNILOADER_H
#define ASEEMOBILESDK_JNILOADER_H
//#ifdef __ANDROID__
#include <atomic>
#include <jni.h>

struct NativeClass {
    const char *className;
    JNINativeMethod *methods;
    int methodCount;
};

#endif //ASEEMOBILESDK_JNILOADER_H
