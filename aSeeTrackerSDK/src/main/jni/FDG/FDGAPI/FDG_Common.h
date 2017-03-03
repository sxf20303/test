//
// Created by 7Invensun on 2017/2/24.
//

#ifndef GEARVR_V1_FDG_COMMON_H
#define GEARVR_V1_FDG_COMMON_H
#include <functional>

// new callbacks based on C++11
#define FDG_CALLBACK_FCP(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, ##__VA_ARGS__)
#define FDG_CALLBACK_FC(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,##__VA_ARGS__)
#define FDG_CALLBACK_CFDI(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,##__VA_ARGS__)
#define FDG_CALLBACK_GT(__selector__,__target__, ...) std::bind(&__selector__,__target__, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,##__VA_ARGS__)


namespace FDG_Interface {

	//FD relative config info
	struct FDConfig
	{
		int has_glass;//wether using glasses,0 no ,1 yes
		int c_center_x;
		int c_center_y;

		int c_rowsMask;
		int c_colsMask;
		int c_radius;

		int c_cutBellow;
		int c_ruRadius;
		int c_leftOrRight;
		bool c_glass;
	};
	//gaze relative config info
	struct GazeConfig
	{
		int device;
		int method;
		float th;
		float ex;
		int tl;
	};
	#define CAL_COE_INFO_DOUBLE_NUM 41 // result double num
	#define CAL_COE_INFO_CHAR_NUM 32 // result char num
	//user's cobibration result struct
	struct CalCoeInfo
	{
		double value[CAL_COE_INFO_DOUBLE_NUM];
		char ver[CAL_COE_INFO_CHAR_NUM];
	};
	//eyes feature detected info
	struct FDInfo
	{
		int				processOK;			///< 眼睛特征数据是否处理成功, 1 true; 0 false

		int				realLeftEyeFound;	///< 左眼是否检测到
		int				leftROI[4];			///< 左眼区域 [0]x [1]y [2]w [3]h
		float			leftPupil[3];		///< 左眼瞳孔直径[0] 左眼瞳孔位置x[1] 左眼瞳孔位置y[2]
		int				leftGlintNum;		///< 左眼光斑数量
		float			leftGlints[8][3];	///< 左眼光斑数组

		int				realRightEyeFound;	///< 右眼是否检测到
		int				rightROI[4];		///< 右眼区域 [0]x [1]y [2]w [3]h
		float			rightPupil[3];		///< 右眼瞳孔直径[0] 右眼瞳孔位置x[1] 右眼瞳孔位置y[2]
		int				rightGlintNum;		///< 右眼光斑数量
		float			rightGlints[8][3];	///< 右眼光斑数组

		unsigned char*  imageData;			///< 图像数据指针
		int				nImageSize;			///< 图像大小
		int				nImageWidth;		///< 图像宽
		int				nImageHeight;		///< 图像高
	};
	struct CalAllPointPos
	{
		int pointNum;//标定点个数
		float point[9][2];//最多9个点的坐标值,point[][0]为x，point[][1]为y
	};
	struct CalPointInfo
	{
		
		int seq;
		int percent;//完成百分比
		int finish;//成功了几次提取
		int totel;//一共需要提取次数
		int count;//总共尝试了几次
		FDInfo* fd;
	};
	struct GazePoint
	{
		float x;
		float y;
		float re;
		FDInfo* fd;
	};

	//pointIndex为校准哪个点，error错误码，0为成功，其他为失败原因
	typedef void (FinishCalibrationPoint)(int pointIndex, int error);
	typedef std::function<FinishCalibrationPoint>  FinishCalibrationPointCallback;
	//totelPoint为总校准点数，cal校准最后的系数结构指针，error错误码，0为成功，其他为失败原因
	typedef void (FinishCalibration)(int totelPoint, CalCoeInfo* cal, int error);
	typedef std::function<FinishCalibration> FinishCalibrationCallback;
	//pointIndex校准第几个点,seq为调用DealImage函数时传入的seq，fd特征值信息指针，percent为这个点校准完成的百分比，finishNum完成了几张图，count这个点总获取的图片，error错误码，0为成功，其他为失败原因
	typedef void (CalibrationFDInfoCall)(int pointIndex,CalPointInfo info,int error);
	typedef std::function<CalibrationFDInfoCall> CalibrationFDInfoCallback;
	//seq为DealImage函数时传入的seq，x为跟踪结果x值，y跟踪结果y值，re是某个参数，fd为特征值信息指针，error错误码，0为成功，其他为失败原因
	typedef void (GazeTracking)(int seq, GazePoint point,int error);
	typedef std::function<GazeTracking> GazeTrackingCallback;
	//图像预处理函数，暂时可以不用
	typedef int (PreDealImage)(unsigned char* pDstImage,int& nDstImageSize,const unsigned char* pSrcImage,int width,int height);
	typedef std::function<PreDealImage> PreDealImageCallback;
	//线程启动和结束回调
	typedef void (ThreadStartEndCall)();
	typedef std::function<ThreadStartEndCall> ThreadStartEndCallback;


}

#endif //GEARVR_V1_FDG_COMMON_H
