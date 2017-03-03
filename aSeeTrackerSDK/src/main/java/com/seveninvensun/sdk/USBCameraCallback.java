package com.seveninvensun.sdk;

/**
 * Created by 7Invensun on 2017/2/27.
 */

interface USBCameraCallback
{
    //连接成功
    void onConnect(int vendorId, int productId, int fileDescriptor, String usbFs);
    //连接断开
    void onClose();
}
