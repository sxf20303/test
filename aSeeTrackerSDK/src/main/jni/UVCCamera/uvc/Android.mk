LOCAL_PATH	:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_C_INCLUDES := \
		$(LOCAL_PATH) \
		$(MY_SDK_PATH)/Tracker/include \
		$(LOCAL_PATH)/../
LOCAL_CFLAGS += -DANDROID_NDK
LOCAL_CFLAGS += -DLOG_NDEBUG
LOCAL_CFLAGS += -DACCESS_RAW_DESCRIPTORS
LOCAL_CFLAGS += -DLOGEE=LOGI
LOCAL_CFLAGS += -std=c++11 -fexceptions -fpermissive -frtti
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_LDLIBS += -llog
LOCAL_LDLIBS += -landroid
LOCAL_STATIC_LIBRARIES += usb100_static uvc_static
LOCAL_SRC_FILES := \
		Camera_API.cpp \
		utilbase.cpp \
		aSeeDeviceCtrl.cpp \
		bmp.cpp

LOCAL_MODULE    := UVCCamera
include $(BUILD_SHARED_LIBRARY)
