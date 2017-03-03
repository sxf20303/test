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
LOCAL_CFLAGS += -O3 -std=c++11 -fexceptions -fpermissive -frtti
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -ldl
LOCAL_LDLIBS += -llog
LOCAL_LDLIBS += -landroid
LOCAL_STATIC_LIBRARIES += usb100_static uvc_static
LOCAL_SRC_FILES := \
		_onload.cpp \
		utilbase.cpp \
		UVCCamera.cpp \
		UVCPreview.cpp \
		Extension.cpp \
		bmp.cpp \
		com_aSeeMobileSDK_UVCCamera.cpp \
		aSeeDeviceCtrl.cpp
LOCAL_MODULE    := UVCCamera
include $(BUILD_SHARED_LIBRARY)
