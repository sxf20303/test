/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014 saki t_saki@serenegiant.com
 *
 * File name: serenegiant_usb_UVCCamera.cpp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * All files in the folder are under this Apache License, Version 2.0.
 * Files in the jni/libjpeg, jni/libusb and jin/libuvc folder may have a different license, see the respective files.
*/

#include <jni.h>
#include <android/native_window_jni.h>

#include "libUVCCamera.h"
#include "UVCCamera.h"

///**
// * set the value into the long field
// * @param env: this param should not be null
// * @param bullet_obj: this param should not be null
// * @param field_name
// * @params val
// */
//static jlong setField_long(JNIEnv *env, jobject java_obj, const char *field_name, jlong val) {
//#if LOCAL_DEBUG
//	LOGV("setField_long:");
//#endif
//
//	jclass clazz = env->GetObjectClass(java_obj);
//	jfieldID field = env->GetFieldID(clazz, field_name, "J");
//	if (LIKELY(field))
//		env->SetLongField(java_obj, field, val);
//	else {
//		LOGE("__setField_long:field '%s' not found", field_name);
//	}
//#ifdef ANDROID_NDK
//	env->DeleteLocalRef(clazz);
//#endif
//	return val;
//}
//
///**
// * @param env: this param should not be null
// * @param bullet_obj: this param should not be null
// */
//static jlong __setField_long(JNIEnv *env, jobject java_obj, jclass clazz, const char *field_name, jlong val) {
//#if LOCAL_DEBUG
//	LOGV("__setField_long:");
//#endif
//
//	jfieldID field = env->GetFieldID(clazz, field_name, "J");
//	if (LIKELY(field))
//		env->SetLongField(java_obj, field, val);
//	else {
//		LOGE("__setField_long:field '%s' not found", field_name);
//	}
//	return val;
//}

static UVCCamera *fromId(ID_TYPE id) {
	return dynamic_cast<UVCCamera *>(reinterpret_cast<ICamera *>(id));
}
static ID_TYPE nativeCreate(JNIEnv *env, jobject thiz) {

	ENTER();
	ICamera *camera = new UVCCamera();
	//setField_long(env, thiz, "mNativePtr", reinterpret_cast<ID_TYPE>(camera));
	RETURN(reinterpret_cast<ID_TYPE>(camera), ID_TYPE)
}

static void nativeDestroy(JNIEnv *env, jobject thiz,
	ID_TYPE id_camera) {

	ENTER();
	//setField_long(env, thiz, "mNativePtr", 0);
	UVCCamera *camera = fromId(id_camera);
	if (LIKELY(camera)) {
		SAFE_DELETE(camera);
	}
	EXIT();
}

static jint nativeConnect(JNIEnv *env, jobject thiz,
	ID_TYPE id_camera,
	jint vid, jint pid, jint fd,  jstring usbfs_str) {

	ENTER();
	int result = JNI_ERR;
	UVCCamera *camera = fromId(id_camera);
	const char *c_usbfs = env->GetStringUTFChars(usbfs_str, JNI_FALSE);
	if (LIKELY(camera && (fd > 0))) {
		 result =  camera->connect(vid, pid, fd, c_usbfs);
	}
	env->ReleaseStringUTFChars(usbfs_str, c_usbfs);
	RETURN(result, jint);
}

static jint nativeRelease(JNIEnv *env, jobject thiz,
	ID_TYPE id_camera) {

	ENTER();
	int result = JNI_ERR;
	UVCCamera *camera = fromId(id_camera);
	if (LIKELY(camera)) {
		result = camera->release();
	}
	RETURN(result, jint);
}

static jint nativeSetPreviewSize(JNIEnv *env, jobject thiz,
	ID_TYPE id_camera, jint width, jint height, jint mode) {

	ENTER();
	UVCCamera *camera = fromId(id_camera);
	if (LIKELY(camera)) {
		return camera->setPreviewSize(width, height, mode);
	}
	RETURN(JNI_ERR, jint);
}

static jint nativeStartPreview(JNIEnv *env, jobject thiz,
	ID_TYPE id_camera, jint angle) {    //   开启 预览 本地方法

	ENTER();
	UVCCamera *camera = fromId(id_camera);
	if (LIKELY(camera)) {
		return camera->startPreview();   //   开启   预览
	}
	RETURN(JNI_ERR, jint);
}

static jint nativeStopPreview(JNIEnv *env, jobject thiz,
	ID_TYPE id_camera) {

	jint result = JNI_ERR;
	ENTER();
	UVCCamera *camera = fromId(id_camera);
	if (LIKELY(camera)) {
		result = camera->stopPreview();
	}
	RETURN(result, jint);
}
///*****************************************************************/    //  by  flyyang
//static jint nativeSetRegisterValue(JNIEnv *env, jobject thiz,
//	ID_TYPE id_camera) {
//
//	jint result = JNI_ERR;
//	ENTER();
//	UVCCamera *camera = fromId(id_camera);
//	if (LIKELY(camera)) {
//
//		result = camera->setRegisterValue();    //  设置 寄存器数值
//	}
//	RETURN(result, jint);
//}
//
///*****************************************************************/


static jint nativeSetPreviewDisplay(JNIEnv *env, jobject thiz,
	ID_TYPE id_camera, jobject jSurface) {

	jint result = JNI_ERR;
	ENTER();
	UVCCamera *camera = fromId(id_camera);
	if (LIKELY(camera)) {
		ANativeWindow *preview_window = jSurface ? ANativeWindow_fromSurface(env, jSurface) : NULL;
		result = camera->setPreviewDisplay(preview_window);
	}
	RETURN(result, jint);
}

//static jint nativeSetFrameCallback(JNIEnv *env, jobject thiz,
//	ID_TYPE id_camera, jobject jIFrameCallback, jint pixel_format) {
//
//	jint result = JNI_ERR;
//
//	//LOGE("nativeSetFrameCallback pixel_format = %d\n",pixel_format);
//	ENTER();
//	UVCCamera *camera = fromId(id_camera);
//	if (LIKELY(camera)) {
//		jobject frame_callback_obj = env->NewGlobalRef(jIFrameCallback);
//		result = camera->setFrameCallback(env, frame_callback_obj, pixel_format);
//	}
//	RETURN(result, jint);
//}
//
//static jint nativeSetCaptureDisplay(JNIEnv *env, jobject thiz,
//	ID_TYPE id_camera, jobject jSurface) {
//
//	jint result = JNI_ERR;
//	ENTER();
//	UVCCamera *camera = fromId(id_camera);
//	if (LIKELY(camera)) {
//		ANativeWindow *capture_window = jSurface ? ANativeWindow_fromSurface(env, jSurface) : NULL;
//		result = camera->setCaptureDisplay(capture_window);
//	}
//	RETURN(result, jint);
//}

static jint nativeCaptureSnapshot(JNIEnv *env, jobject thiz,
	ID_TYPE id_camera,  jstring path_str) {

	jint result = JNI_ERR;
	ENTER();
	UVCCamera *camera = fromId(id_camera);
	if (LIKELY(camera))
	{
		const char *c_path = env->GetStringUTFChars(path_str, JNI_FALSE);
		result = camera->captureSnapshot(c_path);
		env->ReleaseStringUTFChars(path_str, c_path);
	}
	RETURN(result, jint);
}

//**********************************************************************
//
//**********************************************************************
jint registerNativeMethods(JNIEnv* env, const char *class_name, JNINativeMethod *methods, int num_methods) {
	int result = 0;

	jclass clazz = env->FindClass(class_name);
	if (LIKELY(clazz)) {
		int result = env->RegisterNatives(clazz, methods, num_methods);
		if (UNLIKELY(result < 0)) {
			LOGE("registerNativeMethods failed(class=%s)", class_name);
		}
	} else {
		LOGE("registerNativeMethods: class'%s' not found", class_name);
	}
	return result;
}

static JNINativeMethod methods[] = {
	{ "nativeCreate",			"()J", (void *) nativeCreate },
	{ "nativeDestroy",			"(J)V", (void *) nativeDestroy },

	{ "nativeConnect",			"(JIIILjava/lang/String;)I", (void *) nativeConnect },
	{ "nativeRelease",			"(J)I", (void *) nativeRelease },

	{ "nativeSetPreviewSize",	"(JIII)I", (void *) nativeSetPreviewSize },
	{ "nativeStartPreview",		"(JI)I", (void *) nativeStartPreview },
	{ "nativeStopPreview",		"(J)I", (void *) nativeStopPreview },
	{ "nativeSetPreviewDisplay","(JLandroid/view/Surface;)I", (void *) nativeSetPreviewDisplay },
	{ "nativeCaptureSnapshot",  "(JLjava/lang/String;)I", (void *) nativeCaptureSnapshot },

	//{ "nativeSetFrameCallback","(JLcom/serenegiant/usb/IFrameCallback;I)I", (void *) nativeSetFrameCallback },
	//{ "nativeSetCaptureDisplay","(JLandroid/view/Surface;)I", (void *) nativeSetCaptureDisplay },

	//{ "nativeSetRegisterValue","(J)I", (void *) nativeSetRegisterValue },              //    设置寄存器 本地方法绑定
};
//修改JNI包名
int register_uvccamera(JNIEnv *env) {
	LOGV("register_uvccamera:");
	if (registerNativeMethods(env,
		"com/seveninvensun/aseemobilesdk/UVCCamera",
		methods, NUM_ARRAY_ELEMENTS(methods)) < 0) {
		return -1;
	}
    return 0;
}
