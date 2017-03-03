/*
 * UVCCamera
 * library and sample to access to UVC web camera on non-rooted Android device
 *
 * Copyright (c) 2014 saki t_saki@serenegiant.com
 *
 * File name: utilbase.cpp
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

#include <string.h>
#include "utilbase.h"

static JavaVM *savedVm;

extern "C" void setVM(JavaVM *vm) {
	savedVm = vm;
}

extern "C" JavaVM *getVM() {
	return savedVm;
}

extern "C" JNIEnv *getEnv() {
    JNIEnv *env = NULL;
    if (savedVm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
    	env = NULL;
    }
    return env;
}

extern "C" const char *relativeName(const char *fileName) {
    static const char *flag = "jni/";
    static size_t flagSize = strlen(flag);

    const char *pos = strstr(fileName, flag);
    return pos != nullptr ? pos + flagSize : basename(fileName);
}
