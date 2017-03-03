//
// Created by 韩呈杰 on 16/1/30.
//

#ifndef GEARVR_V1_JENV_H
#define GEARVR_V1_JENV_H


#include <jni.h>

namespace JEnv {
    JNIEnv *env();
    void detachCurrentThread();
    // 判断当前线程是否Java线程(在Java空间创建的线程)
    bool isJavaThread();

    // 确保析构时清除掉所有的Java异常
    class ExceptionGuard {
    public:
        ~ExceptionGuard();
    };
};

namespace std {
    namespace this_thread {
        const char *get_id_string();
    }
}


#endif //GEARVR_V1_JENV_H
