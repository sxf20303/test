package com.seveninvensun.sdk;

/**
 * Created by 7Invensun on 2017/2/15.
 */
/**
 * 表示一个[(float)x,(float)y]坐标点
 */

public class PointF
{
    public float x, y;

    public PointF() {
    }

    /**
     * 创建一个坐标点
     * @param x
     * @param y
     */
    public PointF(float x, float y) {
        this.x = x;
        this.y = y;
    }

    public PointF(PointF r) {
        x = r.x;
        y = r.y;
    }
}
