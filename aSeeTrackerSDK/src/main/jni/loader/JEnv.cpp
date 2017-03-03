//
// Created by oscar on 17/2/13.
//
#include <thread>
#include <cassert>
#include <sstream>
#include <sys/syscall.h>
#include <sys/types.h>
#include "estring.h"
#include <Log.h>

#include "JEnv.h"


template<typename T>
static std::string toStr(const T &r) {
    std::stringstream ss;
    ss << "0x" << std::hex << r;
    return ss.str();
}

int getTid() {
#ifdef SYS_gettid
    return syscall(SYS_gettid);
#elif defined(__NR_gettid)
    return syscall(__NR_gettid);
#else
    return getpid();
#endif
}

static thread_local estring threadId;
namespace std {
    namespace this_thread {
        const char *get_id_string() {
            if (threadId.size() == 0) {
                threadId = toStr(get_id());
            }

            return threadId;
        }
    }
}


namespace JEnv {
    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    JavaVM *vm;

    // 适用于单一线程内,请不要跨线程使用
    class JNIEnvProxy {
        JNIEnv *_env;
        // 虽然JNIEnvProxy运行于单一线程,但可能会被不同CPU调度, 因此需要保证变量的原子操作,防止被重复释放.
        bool _needDetach;

        // 线程局部存储的行为很怪异:
        // 1. 明明已经调用过detach了,但是在析构时,检查_needDetach仍为true.
        // 2. atomic操作在thread_local上失效,仍然会有1的问题.
        // 3. 当线程关闭时,jvm的的detach行为检测会早于_jniEnv的析构操作
        // 因此detach只能手动操作,且要在jvm检测是否已经detach前进行.
        static thread_local JNIEnvProxy _jniEnv;

        JNIEnvProxy() {
            assert(vm != nullptr);
            _needDetach = false;
            _env = _getEnv();
            if (nullptr == _env) {
                // 获取JNI版本
                int status = vm->AttachCurrentThread(&_env, 0);
                if (status < 0) {
                    LOGI("(%p): TID{%d,%s}, failed to attach current thread", this,
                         getTid(), toStr(std::this_thread::get_id()).c_str());

                    return;
                }

                _needDetach = true;
            }

            LOGI("[%p]: TID{%d,%s}, needDetach: %s", this,
                 getTid(), toStr(std::this_thread::get_id()).c_str(), (_needDetach ? "true" : "false"));
        }

    public:
        ~JNIEnvProxy() {
            LOGI("[%p): TID{%d,%s}, needDetach: %s", this,
                 getTid(), toStr(std::this_thread::get_id()).c_str(), (_needDetach ? "true" : "false"));
            //detach();
        }

        void detach() {
            LOGI("[%p): TID{%d,%s}, needDetach: %s", this,
                 getTid(), toStr(std::this_thread::get_id()).c_str(), (_needDetach ? "true" : "false"));
            if (_needDetach) {
                _needDetach = false;
                _env = nullptr;
                vm->DetachCurrentThread();
            }
            LOGI("(%p]: TID{%d,%s}, needDetach: %s", this,
                 getTid(), toStr(std::this_thread::get_id()).c_str(), (_needDetach ? "true" : "false"));
        }

        static JNIEnv *get() {
            return _jniEnv._needDetach ? _jniEnv._env : _getEnv();
        }

        static bool needDetach() {
            return _jniEnv._needDetach;
        }

        static void detachCurrentThread() {
            _jniEnv.detach();
        }
    private:
        static JNIEnv *_getEnv() {
            JNIEnv *env = nullptr;
            if (vm->GetEnv((void **) &env, JNI_VERSION_1_4) != JNI_OK) {
                return nullptr;
            }

            return env;
        }
    };
    thread_local JNIEnvProxy JNIEnvProxy::_jniEnv;

    JNIEnv *env() {
        return JNIEnvProxy::get();
    }

    void detachCurrentThread() {
        JNIEnvProxy::detachCurrentThread();
    }

    bool isJavaThread() {
        return !JNIEnvProxy::needDetach();
    }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////////////
    ExceptionGuard::~ExceptionGuard() {
        env()->ExceptionClear();
    }
}
