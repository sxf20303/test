package com.example.a7invensun.gearvr_v1;

import android.content.Context;
import android.util.Log;

import com.seveninvensun.sdk.ASeeTracker;
import com.seveninvensun.sdk.EyeGaze;
import com.seveninvensun.sdk.EyesInfo;
import com.seveninvensun.sdk.EyesInfoCallback;
import com.seveninvensun.sdk.FinishCalCallback;
import com.seveninvensun.sdk.FinishCalPointCallback;
import com.seveninvensun.sdk.CameraCallback;
import com.seveninvensun.sdk.ImageCallback;
import com.seveninvensun.sdk.TrackingCallback;

import java.io.IOException;
import java.util.Date;

/**
 * Created by 7Invensun on 2017/2/27.
 */

public class GearVrTest implements CameraCallback,ImageCallback,EyesInfoCallback,FinishCalCallback,FinishCalPointCallback,TrackingCallback {


    private ASeeTracker m_gearvrApi;

    protected long _handle = 0;
//    private USBCamera usbCamera;

    private long fdTime;
    private long gazeTime;

    public void Init(Context context) throws IOException {
        m_gearvrApi = ASeeTracker.getInstance(context);
    }

    public void StartCamera(Context context)
    {
        m_gearvrApi.connectCamera(this);
    }

    @Override
    public void onConnect()
    {
        m_gearvrApi.startImageCallback(this);
        m_gearvrApi.startCalibration(9,this);
        m_gearvrApi.startCalibrationPoint(1,this,this);

        Date curDate = new Date(System.currentTimeMillis());
        fdTime =  curDate.getTime();
    }
    @Override
    public void onClose()
    {
        m_gearvrApi.disconnectCamera();
    }

    @Override
    public void onImage(byte[] image, int width, int height, int seq)
    {
//                Date curDate = new Date(System.currentTimeMillis());
//                long delay = curDate.getTime() - imageTime;
//                Log.i("onImage", " time = " + Long.toString(delay) );
//                imageTime =  curDate.getTime();
    }
    @Override
    public void onEyesInfo(int pointIndex, int seq, EyesInfo eyes, int percent, int count, int error)
    {
        Date curDate = new Date(System.currentTimeMillis());
        long delay = curDate.getTime() - fdTime;
        fdTime =  curDate.getTime();
        if(error == 0)
        {
            Log.i("onImage", " time = " + Long.toString(delay) );
        }

    }

    @Override
    public void onFinishCal(int totelPoint,int error,byte[] cal)
    {
        Log.i("onFinishCal", " error = " + Integer.toString(error) );
        m_gearvrApi.startTracking(this);
        Date curDate = new Date(System.currentTimeMillis());
        gazeTime =  curDate.getTime();
    }
    @Override
    public void onFinishCalPoint(int pointIndex,int error)
    {
        if(error == 0)
        {
            Log.i("onFinishCalPoint", " point = " + Integer.toString(pointIndex) );
            if(pointIndex < 9)
                m_gearvrApi.startCalibrationPoint(pointIndex+1,this,this);
        }
        else
        {
            m_gearvrApi.startCalibrationPoint(pointIndex,this,this);
        }
    }
    @Override
    public void onGaze(EyeGaze gaze, EyesInfo eyes, int seq, int error)
    {
        Date curDate = new Date(System.currentTimeMillis());
        long delay = curDate.getTime() - gazeTime;
        gazeTime =  curDate.getTime();
        Log.i("onGaze", " point (" + Float.toString(gaze.rightEyeGaze.x) + "," + Float.toString(gaze.rightEyeGaze.y)+ ") time = " + Long.toString(delay) );
    }
}
