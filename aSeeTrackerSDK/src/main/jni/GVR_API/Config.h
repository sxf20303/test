//
// Created by 7Invensun on 2017/2/9.
//

#ifndef GEARVR_V1_CONFIG_H
#define GEARVR_V1_CONFIG_H

/* 
 * 函数名：         GetIniKeyString 
                               GetIniKeyInt
                               GetIniKeyFloat

 * 入口参数：        title 
 *                      配置文件中一组数据的标识 
 *                  key 
 *                      这组数据中要读出的值的标识 
 *                  filename 
 *                      要读取的文件路径 
 * 返回值：         找到需要查的值则返回正确结果 
 *                  否则返回NULL 
 */
char* GetIniKeyString(char* filename,char* title, char* key);
int GetIniKeyInt(char* filename,char* title, char* key);
float GetIniKeyFloat(char* filename,char* title, char* key);

class Config
{
public:
    int m_nROIRectW;
    int m_nROTRectH;
    int m_nCenterX;
    int m_nCenterY;
    int m_nRadius;
    int m_nCutBellowHeight;
    int m_nRightUpRadius;
    int m_nLeftOrRight;
    int m_nHasClass;

    bool m_bSmoothEnabled;

    float m_fSmoothTh;
    float m_fSmoothEx;
    int m_nTimeLimit;

    int m_nDeviceType;
    int m_nMethodType;

    Config()
    {
        m_nROIRectW = 392;
        m_nROTRectH = 392;
        m_nCenterX = 200;
        m_nCenterY = 200;
        m_nRadius = 196;
        m_nCutBellowHeight = 48;
        m_nRightUpRadius = 2;
        m_nLeftOrRight = 0;
        m_bSmoothEnabled = true;
        m_fSmoothTh = 0.028;
        m_fSmoothEx = -0.0001;
        m_nTimeLimit = 1300;
        m_nDeviceType = 3;
        m_nMethodType = 3;
        m_nHasClass = 0;
    }

public:
    int Load(const char* pszPath);

    int GetROIRectW(void) { return m_nROIRectW; }
    int GetROTRectH(void) { return m_nROTRectH; }
    int GetCenterX(void) { return m_nCenterX; }
    int GetCenterY(void) { return m_nCenterY; }
    int GetRadius(void) { return m_nRadius; }
    int GetCutBellowHeight(void) { return m_nCutBellowHeight; }
    int GetRightUpRadius(void) { return m_nRightUpRadius; }
    int GetLeftOrRight(void) { return m_nLeftOrRight; }

};

#endif //GEARVR_V1_CONFIG_H
