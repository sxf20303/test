
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := FDGAPI
LOCAL_C_INCLUDES := $(LOCAL_PATH) \
        $(LOCAL_PATH)/../CalibrationGaze \
        $(LOCAL_PATH)/../FeaturesDetection \

LOCAL_SRC_FILES := FD_Gaze_API.cpp

LOCAL_STATIC_LIBRARIES += cgaze

LOCAL_SHARED_LIBRARIES += FDetection

LOCAL_CFLAGS := -std=c++11
LOCAL_LDLIBS += -ldl -llog -lz

include $(BUILD_SHARED_LIBRARY)