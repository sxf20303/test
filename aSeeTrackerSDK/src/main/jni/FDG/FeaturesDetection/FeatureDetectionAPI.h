#pragma once

#ifndef FDDATADEFINE_H_
#define FDDATADEFINE_H_

typedef  struct FD_POINT
{
	int x;
	int y;
} ASEE_POINT;

typedef struct FD_ROI_INFO
{
	int x;
	int y;
	int w;
	int h;
} ASEE_ROI_INFO;


typedef struct FD_EyeGlint
{
	 float x;
	 float y;
	 float re;
} ASEE_EyeGlint;

typedef struct FD_FeatureDetectionData
{
	int				processOK;			///< 眼睛特征数据是否处理成功, 1 true; 0 false

	int				realLeftEyeFound;	///< 左眼是否检测到
	int				realRightEyeFound;	///< 右眼是否检测到

	FD_ROI_INFO		leftROI;			///< 左眼区域

	float			leftPupilDiameter;	///< 左眼瞳孔直径
	float			leftPupilx;			///< 左眼瞳孔位置x
	float			leftPupily;			///< 左眼瞳孔位置y

	int				leftGlintNum;		///< 左眼光斑数量
	FD_EyeGlint		leftGlints[8];		///< 左眼光斑数组

	FD_ROI_INFO		rightROI;			///< 右眼区域

	float			rightPupilDiameter;	///< 右眼瞳孔直径
	float			rightPupilx;		///< 右眼瞳孔位置x
	float			rightPupily;		///< 右眼瞳孔位置y

	int				rightGlintNum;		///< 右眼光斑数量
	FD_EyeGlint		rightGlints[8];		///< 右眼光斑数组
} ASEE_FeatureDetectionData;

struct CONFIG_LIST
{

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
#endif /* FDDATADEFINE_H_ */

#define BYTE unsigned char



class IFeaturesDetection
{
public:
	//Vive已不使用，GearVR也需要更改成SefCfg函数
	virtual bool LoadCfgFile(const char* cfgPath) =0;
	//配置参数，现在仅Vive使用，gearVR也需要改成这个
	virtual bool SefCfg(CONFIG_LIST *inCfg)=0;
	//设置GearVR戴眼镜参数，需在运行之前设置，若不设置，默认为不戴眼镜
	//VIVE不使用此函数
	virtual void SetGlassStatus(int isGlass) = 0;

	virtual void Init(int nImageWidth, int nImageHeight, int nChannel, bool bLeft) = 0;
	virtual void Init(int nImageWidth, int nImageHeight, int nChannel,
		const ASEE_ROI_INFO& eyeRect, FD_POINT ptMaskCenter, int nRadius, bool bLeft) = 0;

	

	virtual void Release() = 0;

	virtual bool DealImage(const BYTE* pSrcImageData, BYTE* pDstImageData, 
		ASEE_FeatureDetectionData* pOutFeatureDetection) = 0;

	static void SetShowImageWindowState(bool bShow);
	static void SetDrawImageFeaturesState(bool bDraw);
	static void SetFlipImageState(bool bFlip);

	static bool SaveImageFile(const char* pImageFileName, const BYTE* pImageData,
		int nWidth, int nHeight, int nChannel, bool bFlip = false);

};

IFeaturesDetection* CreateFeaturesDetection();
void DestroyFeaturesDetection(IFeaturesDetection*& pFDObject);
char* GetFDVersion();