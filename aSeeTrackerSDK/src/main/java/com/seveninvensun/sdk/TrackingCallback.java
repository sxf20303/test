package com.seveninvensun.sdk;

/**
 * Created by 7Invensun on 2017/2/15.
 */

public interface TrackingCallback
{
    /**
     * 标定完成后回调函数。
     * @param gaze 追踪的信息
     * @param seq 图像顺序
     * @param eyes 眼睛信息，瞳孔位置信息
     * @param error 错误码
     */
    void onGaze(EyeGaze gaze,EyesInfo eyes,int seq,int error);
}
