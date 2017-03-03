#ifndef CALIBRATIONGAZE_H
#define CALIBRATIONGAZE_H

// Ŀǰֻ֧��VC��GCC������
#if !defined(_MSC_VER) && !defined(__GNUC__)
#	error Unsupported C++ Compiler!!!
#endif

#if defined(WIN32) || defined(_WIN32)
#	define GAZECALL __stdcall
//  Windowsƽ̨�ϵ�gcc������֧�֡�__declspec(dllimport)���͡�__declspec(dllexport)��
//#	ifdef __GAZE_DLL__
//#		define GAZEAPI __declspec(dllexport)
//#	elif !defined(__GAZE_STUB__)
//#		define GAZEAPI __declspec(dllimport)
//#	else
#		define GAZEAPI
//#	endif
#else
#	define GAZECALL __attribute__ ((visibility ("default")))
#	define GAZEAPI
#endif

#define GAZE_VER "170111GEAR"

namespace CalibrationGaze{

#ifdef ASEE2G
static const int nglints = 2;
#else
static const int nglints = 8;
#endif

struct PointF
{
	float x,y;
};

struct EyeGlint
{
	float x,y,re;
};

struct EyeFeature
{
	float pupilx;
	float pupily;
	EyeGlint glints[nglints];

	PointF gaze;

	float opt;
};

struct vestPara
{
	double kappa[2];
	double R;
	double K;
	double backoff;
};

struct EyeCalCoe
{
	double calCoeff[12];
	double auxCoeff[12];
	vestPara vp;
	double dc[12];
	char ver[32];
};


// ������ɼ�������֡���ݣ�����ѡȡ����Ҫ���10֡��
struct gazeSelectData
{
	int totalNum;
	int selectNum;

	EyeFeature eyes[100];
};

// ���е�ɼ���֡���ݣ����ڼ���궨ϵ����
struct gazeCalData
{
	int frameNum;		//#Frame selected for each cal Point on the screen.
	int calNum;         //Point number 
	bool finalCal;		// true if this is the final decision.

	EyeFeature	eyes[250];

	EyeCalCoe	coeff; //Output
};

// �����۾���������ע�ӵ������
struct gazeEstData
{
	EyeFeature	eye;
	EyeCalCoe	coeff;
};


#ifdef __cplusplus
extern "C" {
#endif


GAZEAPI int GazeEstimation(gazeEstData &s, void* pGazePara); // 0 for fail, >0 for success.

//Calibration for gazeCalData::coeff. calNum can be 1~9
GAZEAPI int Calibration(gazeCalData &data, void* pGazePara); //input:gaze should be the cor of point on the screen, return 0 for fail, 1 for success.

GAZEAPI double Selection(gazeSelectData &s, void* pGazePara);//input:gaze does not care, return <0 for fail, >=0 for success.


GAZEAPI int SmoothGaze(PointF &gaze, float &re, int t, void* pSmoothPara);//gaze:input and output; re:output, the reliability of output, >=1; t:input,  t:time from last frame (ms), >10000 for the first, return:0

GAZEAPI int InitGazeEstimation(int device, int method, void* &pGazePara);	//return: 0-success   other-fail
GAZEAPI int InitSmooth(float th, float ex, int tl, void* &pSmoothPara); //return: 0-success   other-fail

#ifdef __cplusplus
}
#endif

} //namespace CalibrationGaze


#endif //CALIBRATIONGAZE_H
