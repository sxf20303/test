package com.seveninvensun.sdk;

import android.content.Context;
import android.util.Log;

import java.io.File;
import java.io.IOException;

/**
 * Created by 7Invensun on 2017/2/10.
 */

public class ASeeTracker {

        static{
            System.loadLibrary("ASeeTrackerSDK");
        }
        protected long _handle = 0;
        private USBCamera usbCamera = null;
        private Context mContext;
//        private long imageTime;
//        private long fdTime;
//        private long gazeTime;

        private static ASeeTracker mASeeTracker=null;

        /**
         * 初始化，注意需要保证assets目录中存在指定文件
         *
         * @param context
         */
        private ASeeTracker(Context context) throws IOException {
                this.mContext=context;
                File file = FileUtil.writeRawFileToPrivateDirectoryIfNotExist(context, "Config", "GearVrCfg.ini", "GearVrCfg.ini");
                _handle =  createSDK();
                Log.e("handle",_handle+"");
                initSDK(_handle,file.getAbsolutePath());
                Log.i("initSDK", "initSDK(" + file.getAbsolutePath() + ")");
        }

        public static ASeeTracker getInstance(Context context) throws IOException {
                if(mASeeTracker==null){
                        mASeeTracker=new ASeeTracker(context);
                }
                return mASeeTracker;
        }

        public void connectCamera(final CameraCallback cameraCallback)
        {
                usbCamera = new USBCamera(mContext, new USBCameraCallback() {
                        @Override
                        public void onConnect(int vendorId, int productId, int fileDescriptor, String usbFs) {
                                connectCamera(_handle, vendorId,productId,fileDescriptor, usbFs);
                                if(cameraCallback != null)
                                {
                                        cameraCallback.onConnect();
                                }
                        }

                        @Override
                        public void onClose() {
                                if(cameraCallback != null)
                                {
                                        cameraCallback.onClose();
                                }
                        }
                });
                usbCamera.openCamera();
        }
        public void disconnectCamera()
        {
                disconnectCamera(_handle);
        }
        public void startImageCallback(ImageCallback cb)
        {
                startCamera(_handle,cb);
        }
        public void stopImageCallback()
        {
                stopCamera(_handle);
        }

        public void startCalibration(int totalNum, FinishCalCallback cb)
        {
                startCalibration(_handle,totalNum,cb);
        }
        public void startCalibrationPoint(int pointIndex, FinishCalPointCallback fcb, EyesInfoCallback ecb)
        {
                startCalibrationPoint(_handle,pointIndex,fcb,ecb);
        }
        public void stopCalibrationPoint()
        {
                stopCalibrationPoint(_handle);
        }
        public void startTracking(TrackingCallback cb)
        {
                startTracking(_handle,cb);
        }
        public void startTrackingWithoutCal(byte[] calFactor, TrackingCallback cb)
        {
                startTrackingWithoutCal(_handle,calFactor,cb);
        }
        public void stopTracking()
        {
                stopTracking(_handle);
        }

        //初始化
        private native static long createSDK();
        //释放
        private native static int releaseSDK(long handle);
        //初始化sdk，configFilePath为配置文件路径
        private native int initSDK(long handle,String configFilePath);
        //连接摄像头
        private native int connectCamera(long handle, int venderId, int productId, int fileDescriptor, String usbfs);
        //断开摄像头
        private native int disconnectCamera(long handle);
        //启动摄像头，开始图像回调
        private native int startCamera(long handle, ImageCallback cb);
        //停止摄像头图像回调
        private native int stopCamera(long handle);

        private native void captureSnapshot(long handle,String path);

        //开始校准，参数为校准完成时的回调
        private native int startCalibration(long handle,int totelnum,FinishCalCallback cb);
        //开始校准某点，参数为校准某点完成时的回调，并开启每张图的瞳孔信息回调。
        private native int startCalibrationPoint(long handle,int pointIndex,FinishCalPointCallback fcb,EyesInfoCallback ecb);
        //停止某点校准的回调，停止每张图瞳孔信息的回调
        private native int stopCalibrationPoint(long handele);
        //开始跟踪，开启跟踪回调
        private native int startTracking(long handle,TrackingCallback cb);
        //直接输入校准系数的跟踪，开始跟踪回调
        private native int startTrackingWithoutCal(long handle,byte[] cal,TrackingCallback cb);
        //停止跟踪
        private native int stopTracking(long handle);
}
