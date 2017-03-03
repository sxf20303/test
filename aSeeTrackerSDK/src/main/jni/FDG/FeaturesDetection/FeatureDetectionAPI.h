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
	int				processOK;			///< �۾����������Ƿ���ɹ�, 1 true; 0 false

	int				realLeftEyeFound;	///< �����Ƿ��⵽
	int				realRightEyeFound;	///< �����Ƿ��⵽

	FD_ROI_INFO		leftROI;			///< ��������

	float			leftPupilDiameter;	///< ����ͫ��ֱ��
	float			leftPupilx;			///< ����ͫ��λ��x
	float			leftPupily;			///< ����ͫ��λ��y

	int				leftGlintNum;		///< ���۹������
	FD_EyeGlint		leftGlints[8];		///< ���۹������

	FD_ROI_INFO		rightROI;			///< ��������

	float			rightPupilDiameter;	///< ����ͫ��ֱ��
	float			rightPupilx;		///< ����ͫ��λ��x
	float			rightPupily;		///< ����ͫ��λ��y

	int				rightGlintNum;		///< ���۹������
	FD_EyeGlint		rightGlints[8];		///< ���۹������
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
	//Vive�Ѳ�ʹ�ã�GearVRҲ��Ҫ���ĳ�SefCfg����
	virtual bool LoadCfgFile(const char* cfgPath) =0;
	//���ò��������ڽ�Viveʹ�ã�gearVRҲ��Ҫ�ĳ����
	virtual bool SefCfg(CONFIG_LIST *inCfg)=0;
	//����GearVR���۾���������������֮ǰ���ã��������ã�Ĭ��Ϊ�����۾�
	//VIVE��ʹ�ô˺���
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