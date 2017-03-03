package com.seveninvensun.sdk;

/**
 * Created by 7Invensun on 2017/2/15.
 */
/**
 * 注视点坐标相对屏幕坐标的比例，
 * 如果想得到注视屏幕的绝对坐标值，只需要乘以屏幕尺寸即可。
 * 例如：计算左眼注视点坐标在屏幕上的x值：EyeGaze.leftEyeGaze.x * screenWidth;
 */
public class EyeGaze
{
    /**
     * 左眼的注视点坐标相对整个注视屏幕坐标的比例
     */
    public PointF leftEyeGaze;
    /**
     * 右眼的注视点坐标相对整个注视屏幕坐标的比例
     */
    public PointF rightEyeGaze;
    /**
     * 左眼的注视点平滑位置坐标相对整个注视屏幕坐标的比例
     */
    public PointF leftSmoothEyeGaze;
    /**
     * 右眼的注视点平滑位置坐标相对整个注视屏幕坐标的比例
     */
    public PointF rightSmoothEyeGaze;

    public EyeGaze() {}
    public EyeGaze(PointF leftEye, PointF rightEye, PointF leftSmoothEye, PointF rightSmoothEye) {
        this.leftEyeGaze = leftEye;
        this.rightEyeGaze = rightEye;
        this.leftSmoothEyeGaze = leftSmoothEye;
        this.rightSmoothEyeGaze = rightSmoothEye;
    }
}
