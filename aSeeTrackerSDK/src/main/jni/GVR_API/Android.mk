LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := GVRAPI
LOCAL_CFLAGS := -std=c++11 -fexceptions -fpermissive -frtti
LOCAL_LDFLAGS := -Wl,--build-id
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_LDLIBS += -llog
LOCAL_LDLIBS += -landroid
LOCAL_C_INCLUDES := $(LOCAL_PATH)/ \
        $(LOCAL_PATH)/../FDG/FDGAPI/ \
        $(LOCAL_PATH)/../UVCCamera/uvc/
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/
LOCAL_SHARED_LIBRARIES += FDGAPI
LOCAL_SHARED_LIBRARIES += UVCCamera
LOCAL_SRC_FILES := \
	GearVrApi.cpp \
	Config.cpp

include $(BUILD_SHARED_LIBRARY)
#$(call import-add-path, $(LOCAL_PATH)/..)
