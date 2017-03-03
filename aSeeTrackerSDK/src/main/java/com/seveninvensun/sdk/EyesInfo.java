package com.seveninvensun.sdk;

/**
 * Created by 7Invensun on 2017/2/15.
 */
/**
 * 眼睛瞳孔中心在图像中的坐标位置相对图像尺寸的比例
 */

public class EyesInfo
{
    /**
     *  左眼的瞳孔中心坐标x,y相对图像width，height的比例，即(x/width,y/height)
     */
    public PointF leftEye;
    /**
     *  右眼的瞳孔中心坐标x,y相对图像width，height的比例，即(x/width,y/height)
     */
    public PointF rightEye;

    public EyesInfo() {}

    public EyesInfo(PointF leftEye, PointF rightEye) {
        this.leftEye = leftEye;
        this.rightEye = rightEye;
    }
}
