using UnityEngine;
using System.Collections;
using System.Runtime.Remoting.Contexts;

public class ASeeTracker : MonoBehaviour {
    AndroidJavaObject mASeeTracker ;

    public void Start(){
        AndroidJavaClass jc = new AndroidJavaClass ("com.unity3d.player.UnityPlayer");
    	AndroidJavaObject context = jc.GetStatic<AndroidJavaObject> ("currentActivity");
    	AndroidJavaClass aSeeTrackerClass = new AndroidJavaClass ("com.seveninvensun.sdk.ASeeTracker");
    	mASeeTracker = aSeeTrackerClass.CallStatic<AndroidJavaObject>("getInstance");
    }

    class CameraCallback : AndroidJavaProxy
    {
        public CameraCallback() : base("com.seveninvensun.sdk.CameraCallback") { }
         //连接成功
        public void onConnect(){
        };
            //连接断开
        public void onClose(){
        };
    }

    class ImageCallback : AndroidJavaProxy
	{
		public ImageCallback() : base("com.seveninvensun.sdk.ImageCallback") { }
		void onImage(byte[] image, int width, int height, int seq){
		}
	}

	class FinishCalCallback : AndroidJavaProxy
    {
        public FinishCalCallback() : base("com.seveninvensun.sdk.FinishCalCallback") { }
        /**
         * 标定完成后回调函数。
         * @param totalPoint 标定的总点数
         * @param calFactor 返回标定的系数
         * @param error 错误码
         */
        void onFinishCal(int totalPoint,int error,byte[] calFactor){
        };
    }

    public interface FinishCalPointCallback : AndroidJavaProxy
    {
        public FinishCalPointCallback() : base("com.seveninvensun.sdk.FinishCalPointCallback") { }

        /**
         * 标定完成后回调函数。
         * @param pointCount 标定的总点数
         * @param error 错误码
         */
        void onFinishCalPoint(int pointCount,int error){
        };
    }

    class TrackingCallback : AndroidJavaProxy
    {
        public TrackingCallback() : base("com.seveninvensun.sdk.TrackingCallback") { }
        void onGaze(AndroidJavaObject gaze,AndroidJavaObject eyes,int seq,int error)
		{
            /////////////////////////
            float leftGazeX=gaze.Get("leftEyeGaze").Get("x");
            float leftGazeY=gaze.Get("leftEyeGaze").Get("y");
            float rightGazeX=gaze.Get("rightEyeGaze").Get("x");
            float rightGazeY=gaze.Get("rightEyeGaze").Get("y");
            float leftSmoothGazeX=gaze.Get("leftSmoothEyeGaze").Get("x");
            float leftSmoothGazeY=gaze.Get("leftSmoothEyeGaze").Get("y");
            float rightSmoothGazeX=gaze.Get("rightSmoothEyeGaze").Get("x");
            float rightSmoothGazeY=gaze.Get("rightSmoothEyeGaze").Get("y");
            float leftEyesInfoX=eyes.Get("leftEye").Get("x");
            float leftEyesInfoY=eyes.Get("leftEye").Get("y");
            float rightEyesInfoX=eyes.Get("rightEye").Get("x");
            float rightEyesInfoY=eyes.Get("rightEye").Get("y");
		}
    }

    public interface EyesInfoCallback : AndroidJavaProxy
    {
        public EyesInfoCallback() : base("com.seveninvensun.sdk.EyesInfoCallback") { }

        /**
         * 标定完成后回调函数。
         * @param pointIndex 标定点索引
         * @param seq 图像顺序
         * @param eyes 眼睛信息，瞳孔位置信息
         * @param percent 校准点完成度，百分比，0-100
         * @param count 校准点时尝试的次数，便于判断是否需要停止
         * @param error 错误码
         */
        void onEyesInfo(int pointIndex,int seq,AndroidJavaObject eyes,int percent,int count,int error){
            /////////////////////////
            float leftEyesInfoX=eyes.Get("leftEye").Get("x");
            float leftEyesInfoY=eyes.Get("leftEye").Get("y");
            float rightEyesInfoX=eyes.Get("rightEye").Get("x");
            float rightEyesInfoY=eyes.Get("rightEye").Get("y");
        }
    }

    public void ConnectCamera(CameraCallback cb)
    {
        mASeeTracker.Call("connectCamera",cb);
    }

    public void DisconnectCamera()
    {
         mASeeTracker.Call("disconnectCamera");
    }

    public void StartImageCallback(ImageCallback cb)
    {
         mASeeTracker.Call("startImageCallback",cb);
    }

    public void StopImageCallback()
    {
         mASeeTracker.Call("stopImageCallback");
    }

    public void StartCalibration(int totalNum, FinishCalCallback cb)
    {
         mASeeTracker.Call("startCalibration",cb);
    }

    public void StartCalibrationPoint(int pointIndex, FinishCalPointCallback fcb, EyesInfoCallback ecb)
    {
         mASeeTracker.Call("startCalibrationPoint",pointIndex,fcb,ecb);
    }

    public void StopCalibrationPoint()
    {
         mASeeTracker.Call("stopCalibrationPoint");
    }

    public void StartTracking(TrackingCallback cb)
    {
         mASeeTracker.Call("startTracking",cb);
    }

    public void StartTrackingWithoutCal(byte[] calFactor, TrackingCallback cb)
    {
         mASeeTracker.Call("startTracking",calFactor,cb);
    }

    public void stopTracking()
    {
         mASeeTracker.Call("stopTracking");
    }
}