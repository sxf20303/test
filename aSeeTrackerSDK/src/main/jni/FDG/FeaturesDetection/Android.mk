
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := FDetection
LOCAL_SRC_FILES := $(LOCAL_PATH)/libFeaturesDetection.so
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)

include $(PREBUILT_SHARED_LIBRARY)