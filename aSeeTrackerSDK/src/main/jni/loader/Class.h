#ifndef __CLASS_CLASS_H__
#define __CLASS_CLASS_H__

#include <jni.h>


struct ClassInfo {
    const char *name;   // 类名
    //const char *sig;    // 完全限定类名
    jclass clazz;
};

enum class EnumClass {
    PointF,
    EyesInfo,
    EyeGaze,


    MaxValue,  // 请确保该项是枚举值的最后一项,用于计算最大值,及枚举数量
};
const ClassInfo &getClassInfo(EnumClass hashCode);


//template<typename T>
//class Class {
//    // 禁止未特化时实例化:没有实际意义
//    Class(){}
//};
//
//#define CLASS(CLS) template<> \
//class Class< CLS > { \
//public: \
//    enum {hashCode = (int)EnumClass:: CLS, }; \
//    static const char *name() { \
//        return getClassInfo(hashCode).name; \
//    } \
//    static jclass clazz() { \
//        return getClassInfo(hashCode).clazz; \
//    } \
//}

template<EnumClass EC>
class Class {
public:
    static const EnumClass hashCode = EC;
    static const char *name() {
        return getClassInfo(EC).name;
    }
    static jclass clazz() {
        return getClassInfo(EC).clazz;
    }
};

//// 请注意: EnumClass中的枚举项(除MaxValue外),需要与下面的CLASS一一对应.
//CLASS(ASeeTrackerException);
//CLASS(IndexOutOfBoundsException);
//CLASS(RecursiveConnectException);
//CLASS(CalibrationException);
//CLASS(CalibrationAlreadyStartedException);
//CLASS(CalibrationNotRunningException);
//CLASS(CalibrationLackOfPointException);
//
//CLASS(PointF);
//
//CLASS(IImageCallback);
//CLASS(tagEyePos);
//CLASS(IEyeDetectionCallback);
//CLASS(ITrackingCallback);
//
//CLASS(VirtualCamera);
//
//CLASS(StatInfo);


#endif
