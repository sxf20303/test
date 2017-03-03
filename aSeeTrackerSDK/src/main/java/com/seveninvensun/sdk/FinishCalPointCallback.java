package com.seveninvensun.sdk;

/**
 * Created by 7Invensun on 2017/2/15.
 */

public interface FinishCalPointCallback
{
    /**
     * 标定完成后回调函数。
     * @param pointCount 标定的总点数
     * @param error 错误码
     */
    void onFinishCalPoint(int pointCount,int error);
}
