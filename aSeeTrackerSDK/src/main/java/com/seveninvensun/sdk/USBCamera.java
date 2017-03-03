package com.seveninvensun.sdk;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbManager;
import android.text.TextUtils;
import android.util.Log;
import android.widget.Toast;

import java.util.List;

/**
 * 设备USB相机
 *
 */
final class USBCamera implements USBMonitor.OnDeviceConnectListener {

    private static final String TAG = USBCamera.class.getSimpleName();
    private static final String DEFAULT_USBFS = "/dev/bus/usb";

    private USBCameraCallback camera;
//    private UVCCamera uvcCamera;
    private USBMonitor usbMonitor;
    private Context mContext;
    private UsbDevice mCameraDevice;
    //private Handler mPreviewHandler;
    private boolean mIsConnected = false;
    private boolean mAttached;
    private BroadcastReceiver mUsbReceiver;

    public USBCamera(Context context,USBCameraCallback camera) {
        this.mContext = context;
        this.camera = camera;
//        uvcCamera = new UVCCamera();
//        if (surface != null) {
//            uvcCamera.setPreviewDisplay(surface);
//        }

    }

    public synchronized boolean openCamera() {
        mIsConnected=false;
        usbMonitor = new USBMonitor(mContext, this);
        usbMonitor.register();//此处注册是监听USB_DEVICE_DETACHED
        DeviceFilter deviceFilter=new DeviceFilter(-1, -1, 239, 2, -1, null, null, null);
        List<UsbDevice> usbList = usbMonitor.getDeviceList(deviceFilter);
        if (usbList.size() != 0) {
            mCameraDevice = usbList.get(0);
            if(mCameraDevice!=null){
                usbMonitor.requestPermission(mCameraDevice);//请求权限（如果已有权限会直接打开）
            }
            return true;
        }else{
            Toast.makeText(mContext, "USB设备未连接好", Toast.LENGTH_SHORT).show();
            return false;
        }
    }

    public synchronized void closeCamera() {
        if (usbMonitor != null) {
            usbMonitor.unregister();
            usbMonitor.destroy();
            usbMonitor=null;
        }
        if(mUsbReceiver!=null){
            mContext.unregisterReceiver(mUsbReceiver);
            mUsbReceiver=null;
        }
        Log.i(TAG, "USB Camera has destroy");
    }
    private final String getUSBFSName(final USBMonitor.UsbControlBlock ctrlBlock) {
        String result = null;
        final String name = ctrlBlock.getDeviceName();
        final String[] v = !TextUtils.isEmpty(name) ? name.split("/") : null;
        if ((v != null) && (v.length > 2)) {
            final StringBuilder sb = new StringBuilder(v[0]);
            for (int i = 1; i < v.length - 2; i++)
                sb.append("/").append(v[i]);
            result = sb.toString();
        }
        if (TextUtils.isEmpty(result)) {
            Log.w(TAG, "failed to get USBFS path, try to use default path:" + name);
            result = DEFAULT_USBFS;
        }
        return result;
    }


    @Override
    public synchronized void onConnect(UsbDevice device, USBMonitor.UsbControlBlock ctrlBlock, boolean createNew) {
        this.camera.onConnect(ctrlBlock.getVenderId(), ctrlBlock.getProductId(), ctrlBlock.getFileDescriptor(), getUSBFSName(ctrlBlock));
//        uvcCamera.open(ctrlBlock);
//        uvcCamera.startPreview();
        if(mUsbReceiver!=null){
            //此处是销毁断开时监听的usb attach
            mContext.unregisterReceiver(mUsbReceiver);
            mUsbReceiver=null;
        }
        Log.i(TAG, "设备已连接----------");
        //Toast.makeText(mContext, "设备已连接", Toast.LENGTH_SHORT).show();
        this.mIsConnected = true;
    }

    @Override
    public synchronized void onDisconnect(UsbDevice device, USBMonitor.UsbControlBlock ctrlBlock) {
        if (camera != null) {
            camera.onClose();
            mIsConnected = false;
            Log.i(TAG, "设备已断开----------");

            IntentFilter filter = new IntentFilter(UsbManager.ACTION_USB_DEVICE_ATTACHED);
            mUsbReceiver=new BroadcastReceiver() {
                @Override
                public void onReceive(Context context, Intent intent) {
                    if(intent.getAction().equals(UsbManager.ACTION_USB_DEVICE_ATTACHED)){
                        Log.i(TAG, "重连设备----------");
                        if(usbMonitor!=null){
                            DeviceFilter deviceFilter=new DeviceFilter(-1, -1, 239, 2, -1, null, null, null);
                            List<UsbDevice> usbList = usbMonitor.getDeviceList(deviceFilter);
                            if (usbList.size() != 0) {
                                mCameraDevice = usbList.get(0);
                                usbMonitor.requestPermission(mCameraDevice);//请求权限（如果已有权限会直接打开）
                            }
                        }
                    }
                }
            };
            mContext.registerReceiver(mUsbReceiver, filter);
        }
    }

    @Override
    public void onAttach(UsbDevice device) {
        this.mAttached = true;
    }

    @Override
    public void onDettach(final UsbDevice device) {
        this.mAttached = false;
    }

    @Override
    public void onCancel() {
        Log.i(TAG, "设备未被授予权限----------");
        Toast.makeText(mContext, "未取得USB连接权限", Toast.LENGTH_SHORT).show();

    }

//    @Override
//    public long nativeHandle() {
//        return (uvcCamera != null ? uvcCamera.nativeHandle() : 0);
//    }

//    @Override
//    public synchronized void release() {
//        Log.i("USBCamera", "release");
//        stopPreview();
//        if (uvcCamera != null) {
//            uvcCamera.release();
//            uvcCamera = null;
//        }
//        Log.i(TAG, "USB设备已释放----------");
//    }

//    @Override
//    public boolean startPreview() {
//        return this.openCamera();
//    }
//
//    @Override
//    public void stopPreview() {
//        closeCamera();
//    }
//
//    @Override
//    public boolean isConnected() {
//        return mIsConnected;
//    }

    public void captureSnapshot(String path) {

        Log.e("----","captureSnapshot----------------2");
//        uvcCamera.captureSnapshot(path);
    }

}