package com.seveninvensun.sdk;

/**
 * Created by 7Invensun on 2017/2/15.
 */

public interface EyesInfoCallback
{
    /**
     * 标定完成后回调函数。
     * @param pointIndex 标定点索引
     * @param seq 图像顺序
     * @param eyes 眼睛信息，瞳孔位置信息
     * @param percent 校准点完成度，百分比，0-100
     * @param count 校准点时尝试的次数，便于判断是否需要停止
     * @param error 错误码
     */
    void onEyesInfo(int pointIndex,int seq,EyesInfo eyes,int percent,int count,int error);
}
