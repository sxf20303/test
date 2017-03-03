LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES := \
	core.c \
	descriptor.c \
	hotplug.c \
	io.c \
	sync.c \
	strerror.c \
	os/android_usbfs.c \
	os/poll_posix.c \
	os/threads_posix.c \
	os/android_netlink.c
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)\
	$(LOCAL_PATH)/os \
    $(LOCAL_PATH)/../
LOCAL_EXPORT_C_INCLUDES := \
	$(LOCAL_PATH)
LOCAL_CFLAGS := $(LOCAL_C_INCLUDES:%=-I%)
LOCAL_CFLAGS += -DANDROID_NDK
LOCAL_CFLAGS += -DLOG_NDEBUG
LOCAL_CFLAGS += -DACCESS_RAW_DESCRIPTORS
LOCAL_CFLAGS += -O3 -fstrict-aliasing #-fprefetch-loop-arrays
LOCAL_EXPORT_LDLIBS += -llog
LOCAL_MODULE := usb100_static
include $(BUILD_STATIC_LIBRARY)