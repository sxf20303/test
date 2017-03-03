MY_LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
include $(MY_LOCAL_PATH)/libjpeg/Android.mk
include $(MY_LOCAL_PATH)/libusb/Android.mk
include $(MY_LOCAL_PATH)/libuvc/Android.mk
include $(MY_LOCAL_PATH)/uvc/Android.mk