package com.seveninvensun.sdk;

/**
 * Created by 7Invensun on 2017/2/13.
 */

public interface ImageCallback
{
    /**
     * 每一帧原生图像流会回调到这里，格式是RGB。
     * @param image 图像字节流
     * @param width 图像宽度
     * @param height 图像高度
     * @param seq 图像顺序
     */
    void onImage(byte[] image, int width, int height, int seq);
}
