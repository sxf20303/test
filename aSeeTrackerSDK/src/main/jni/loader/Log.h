//
// Created by 韩呈杰 on 16/1/15.
//

#ifndef NDKJNI_LOG_H
#define NDKJNI_LOG_H
#include <libgen.h>
#include <string>

const char *relativeName(const char *fileName);
#ifdef __ANDROID__
#   include <android/log.h>

// log标签
#   define  TAG    "SDKTest.C++"

// 定义info信息
#   define LOGI(FMT, ...) __android_log_print(ANDROID_LOG_INFO , TAG, (std::string("[%s:%d:%s]:") + FMT).c_str(), relativeName(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
// 定义debug信息
#   define LOGD(FMT, ...) __android_log_print(ANDROID_LOG_DEBUG, TAG, (std::string("[%s:%d:%s]:") + FMT).c_str(), relativeName(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
// 定义error信息
#   define LOGE(FMT, ...) __android_log_print(ANDROID_LOG_ERROR, TAG, (std::string("[%s:%d:%s]:") + FMT).c_str(), relativeName(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
#else
#   include <stdio.h>
void printLog(const char *format, ...);
// 定义info信息
#   define LOGI(format, ...) printLog((std::string("[%s:%d:%s]:") + FMT + "\n").c_str(), relativeName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
// 定义debug信息
#   define LOGD(format, ...) printLog((std::string("[%s:%d:%s]:") + FMT + "\n").c_str(), relativeName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
// 定义error信息
#   define LOGE(format, ...) printLog((std::string("[%s:%d:%s]:") + FMT + "\n").c_str(), relativeName(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)
#endif

#endif //NDKJNI_LOG_H
