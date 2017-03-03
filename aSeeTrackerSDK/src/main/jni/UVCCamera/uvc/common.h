//
// Created by 7Invensun on 2017/2/7.
//

#ifndef GEARVR_V1_COMMON_H
#define GEARVR_V1_COMMON_H

#include <unistd.h>
#include "utilbase.h"

#define	LOCAL_DEBUG 0

// 这个也是native_window.h中支持的格式,但未在其头文件中定义
#define WINDOW_FORMAT_RGB_888 3

#define MAX_FRAME 4
#define PREVIEW_PIXEL_BYTES 3	// 3: RGB, 4: RGBA/RGBX
#define DEFAULT_PREVIEW_FORMAT WINDOW_FORMAT_RGB_888
#define DEFAULT_PREVIEW_MODE 0      // YUV
//#if 0
//#define DEFAULT_PREVIEW_WIDTH 960     // 背夹相机的的图像,从相机上来时,其报告的图像宽度
//#define DEFAULT_PREVIEW_HEIGHT 1080
//#define DEFAULT_PREVIEW_FPS 60
//#endif

typedef void (*FCopyFrame)(uint8_t *dst, const uint8_t *src, int width, int height, int stride);
void beijia_copyFrame(uint8_t *dst, const uint8_t *src, int width, int height, int stride)
{
    // 背夹需要左右倒一下，否则图像不是镜像的
    for (int h = 0; h < height; h++) {
        const uint8_t *srcLine = src + stride * h;
        const uint8_t *srcLineEnd = srcLine + width;
        uint8_t       *dstIt = dst + stride * h + width - 1;
        //int left = 0, right = width - 1;
        for (const uint8_t *it = srcLine ; it < srcLineEnd; it++, dstIt--) {
            *dstIt = *it;
        }
    }
}

void yuy2_to_nv21(uint8_t *dst, const uint8_t *src, int width, int height, int stride)
{
    int lineWidth = stride * 2;
    for (int h = 0; h < height; h++) {
        const uint8_t *lineEnd = src + lineWidth;
        for (; src < lineEnd; ((src+=2), dst++)) {
            *dst = *src;
        }
        src = lineEnd;
        dst += (stride - width);
    }
}

// 背夹相机: vendorId: 0x5a9, productId: 0x581
struct CameraParam {
    int vendorId;
    int productId;
    int previewWidth;
    int previewHeight;
    int previewMinFps;
    int previewMaxFps;

    // 背夹相机的实际宽度,应为其返回宽度的2倍
    int frameWidthRate;

    FCopyFrame copyFrameData;
};
static const CameraParam normalCamera {    0,     0, 1920, 1080, 1, 30, 1, yuy2_to_nv21};
static const CameraParam beijiaCamera {0x5a9, 0x581,  960, 1080, 1, 60, 2, beijia_copyFrame};
static const CameraParam VRCamera {0x5a9, 0x4301,  200, 400, 1, 120, 2, beijia_copyFrame};

static const CameraParam &_default(int vendorId, int productId) {
    if (vendorId == beijiaCamera.vendorId && productId == beijiaCamera.productId) {
        LOGI("背夹");
        // 背夹摄像头
        return beijiaCamera;
    } else if(vendorId == VRCamera.vendorId && productId == VRCamera.productId){
        LOGI("Gear VR");
        return VRCamera;
    } else{
        LOGI("普通摄像头");
        return normalCamera;
    }
}
static uvc_frame_format adjustFrameFormat(const CameraParam *defaultCameraParam, uvc_frame_format frameFormat) {
    return (defaultCameraParam == &beijiaCamera ? UVC_FRAME_FORMAT_RAW8 : frameFormat);
}

#endif //GEARVR_V1_COMMON_H
