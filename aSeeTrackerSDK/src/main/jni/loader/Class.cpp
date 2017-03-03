#include <memory>
#include <cassert>
#include "Class.h"

struct {
    EnumClass hashCode;
    ClassInfo _info;
} _classInfo[] = {//修改JNI包名
    {EnumClass::PointF, {"com/seveninvensun/sdk/PointF", nullptr}},
    {EnumClass::EyesInfo, {"com/seveninvensun/sdk/EyesInfo", nullptr}},
    {EnumClass::EyeGaze, {"com/seveninvensun/sdk/EyeGaze", nullptr}},

//    {EnumClass::IndexOutOfBoundsException, {"java/lang/IndexOutOfBoundsException", nullptr}},


};

const ClassInfo &getClassInfo(EnumClass hashCode) {
    //assert(0 <= hashCode && hashCode < sizeof(_classInfo)/sizeof(_classInfo[0]));
    return _classInfo[static_cast<int>(hashCode)]._info;
}

void initJClass(JNIEnv *env) {
    for (int i = 0; i < (int)EnumClass::MaxValue; i++) {
        assert(_classInfo[i].hashCode == static_cast<EnumClass>(i));
        jclass cls = env->FindClass(_classInfo[i]._info.name);
        assert(cls);
        _classInfo[i]._info.clazz = (jclass)env->NewGlobalRef(cls);
        env->DeleteLocalRef(cls);
    }
}

void releaseJClass(JNIEnv *env) {
    for (auto it: _classInfo) {
        env->DeleteGlobalRef(it._info.clazz);
        it._info.clazz = nullptr;
    }
    assert(_classInfo[0]._info.clazz == nullptr);
}
