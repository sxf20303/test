#include "Log.h"

const char *relativeName(const char *fileName) {
    static const char *flag = "jni/";
    static size_t flagSize = strlen(flag);

    const char *pos = strstr(fileName, flag);
    return pos != nullptr ? pos + flagSize : basename(fileName);
}


#ifndef __ANDROID__
#include <iostream>
#include <mutex>

static std::mutex _mutex;

void printLog(const char *format, ...) {
    std::unique_lock<std::mutex> lock(_mutex);
    va_list argList;

    va_start(argList, format);
    vfprintf(stdout, format, argList);
    va_end(argList);
}

#endif