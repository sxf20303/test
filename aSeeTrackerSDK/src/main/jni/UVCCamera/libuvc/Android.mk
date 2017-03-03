LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/include/libuvc \
	$(LOCAL_PATH)/../
LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)/include \
	$(LOCAL_PATH)/include/libuvc
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_CFLAGS += -DANDROID_NDK
LOCAL_CFLAGS += -DLOG_NDEBUG
LOCAL_CFLAGS += -O3 -fstrict-aliasing #-fprefetch-loop-arrays
LOCAL_EXPORT_LDLIBS := -llog
LOCAL_STATIC_LIBRARIES += jpeg_static
LOCAL_STATIC_LIBRARIES += usb100_static
LOCAL_SRC_FILES := \
	src/ctrl.c \
	src/device.c \
	src/diag.c \
	src/frame.c \
	src/frame-mjpeg.c \
	src/init.c \
	src/stream.c
LOCAL_MODULE := libuvc_static
include $(BUILD_STATIC_LIBRARY)