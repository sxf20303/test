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
		int				processOK;			///< �۾����������Ƿ���ɹ�, 1 true; 0 false

		int				realLeftEyeFound;	///< �����Ƿ��⵽
		int				leftROI[4];			///< �������� [0]x [1]y [2]w [3]h
		float			leftPupil[3];		///< ����ͫ��ֱ��[0] ����ͫ��λ��x[1] ����ͫ��λ��y[2]
		int				leftGlintNum;		///< ���۹������
		float			leftGlints[8][3];	///< ���۹������

		int				realRightEyeFound;	///< �����Ƿ��⵽
		int				rightROI[4];		///< �������� [0]x [1]y [2]w [3]h
		float			rightPupil[3];		///< ����ͫ��ֱ��[0] ����ͫ��λ��x[1] ����ͫ��λ��y[2]
		int				rightGlintNum;		///< ���۹������
		float			rightGlints[8][3];	///< ���۹������

		unsigned char*  imageData;			///< ͼ������ָ��
		int				nImageSize;			///< ͼ���С
		int				nImageWidth;		///< ͼ���
		int				nImageHeight;		///< ͼ���
	};
	struct CalAllPointPos
	{
		int pointNum;//�궨�����
		float point[9][2];//���9���������ֵ,point[][0]Ϊx��point[][1]Ϊy
	};
	struct CalPointInfo
	{
		
		int seq;
		int percent;//��ɰٷֱ�
		int finish;//�ɹ��˼�����ȡ
		int totel;//һ����Ҫ��ȡ����
		int count;//�ܹ������˼���
		FDInfo* fd;
	};
	struct GazePoint
	{
		float x;
		float y;
		float re;
		FDInfo* fd;
	};

	//pointIndexΪУ׼�ĸ��㣬error�����룬0Ϊ�ɹ�������Ϊʧ��ԭ��
	typedef void (FinishCalibrationPoint)(int pointIndex, int error);
	typedef std::function<FinishCalibrationPoint>  FinishCalibrationPointCallback;
	//totelPointΪ��У׼������calУ׼����ϵ���ṹָ�룬error�����룬0Ϊ�ɹ�������Ϊʧ��ԭ��
	typedef void (FinishCalibration)(int totelPoint, CalCoeInfo* cal, int error);
	typedef std::function<FinishCalibration> FinishCalibrationCallback;
	//pointIndexУ׼�ڼ�����,seqΪ����DealImage����ʱ�����seq��fd����ֵ��Ϣָ�룬percentΪ�����У׼��ɵİٷֱȣ�finishNum����˼���ͼ��count������ܻ�ȡ��ͼƬ��error�����룬0Ϊ�ɹ�������Ϊʧ��ԭ��
	typedef void (CalibrationFDInfoCall)(int pointIndex,CalPointInfo info,int error);
	typedef std::function<CalibrationFDInfoCall> CalibrationFDInfoCallback;
	//seqΪDealImage����ʱ�����seq��xΪ���ٽ��xֵ��y���ٽ��yֵ��re��ĳ��������fdΪ����ֵ��Ϣָ�룬error�����룬0Ϊ�ɹ�������Ϊʧ��ԭ��
	typedef void (GazeTracking)(int seq, GazePoint point,int error);
	typedef std::function<GazeTracking> GazeTrackingCallback;
	//ͼ��Ԥ����������ʱ���Բ���
	typedef int (PreDealImage)(unsigned char* pDstImage,int& nDstImageSize,const unsigned char* pSrcImage,int width,int height);
	typedef std::function<PreDealImage> PreDealImageCallback;
	//�߳������ͽ����ص�
	typedef void (ThreadStartEndCall)();
	typedef std::function<ThreadStartEndCall> ThreadStartEndCallback;


}

#endif //GEARVR_V1_FDG_COMMON_H
