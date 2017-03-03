package com.seveninvensun.sdk;

/**
 * Created by 7Invensun on 2017/2/14.
 */

public interface FinishCalCallback
{
    /**
     * 标定完成后回调函数。
     * @param totalPoint 标定的总点数
     * @param calFactor 返回标定的系数
     * @param error 错误码
     */
    void onFinishCal(int totalPoint,int error,byte[] calFactor);
}
