//
// Created by 7Invensun on 2017/2/9.
//

#include "Config.h"
#include <stdio.h>  
#include <string.h>
#include <string>


char* GetIniKeyString(char* filename,char* title, char* key)
{
    FILE* fp;
    int flag = 0;
    char sTitle[64],*wTmp;
    static char sLine[1024];
    sprintf(sTitle,"[%s]",title);
    if(nullptr == (fp = fopen(filename,"r")))
    {
        perror("fopen");
        return nullptr;
    }
    while(nullptr != fgets(sLine,1024,fp))
    {
        if( 0 == strncmp("//",sLine,2))
            continue;
        if('#'== sLine[0])
            continue;
        wTmp = strchr(sLine,'=');
        if ((NULL != wTmp) && (1 == flag))
        {
            if(0 == strncmp(key, sLine, wTmp-sLine))
            { // 长度依文件读取的为准
                sLine[strlen(sLine) - 1] = '\0';
                fclose(fp);
                return wTmp + 1;
            }
        }
        else
        {
            if(0 == strncmp(sTitle, sLine, strlen(sTitle)))// 长度依文件读取的为准  
            {
                flag = 1;// 找到标题位置  
            }
        }
    }
    fclose(fp);
    return nullptr;
}
int GetIniKeyInt(char* filename,char* title, char* key)
{
    int value;
    char* str = GetIniKeyString(filename,title,key);
    sscanf(str,"%d",&value);
    return value;
}
float GetIniKeyFloat(char* filename,char* title, char* key)
{
    float value;
    char* str = GetIniKeyString(filename,title,key);
    sscanf(str,"%f",&value);
    return value;
}

int Config::Load(const char *pszPath)
{
    std::string name = std::string(pszPath) + std::string("/GearVrCfg.ini");

    m_nROIRectW = GetIniKeyInt((char*)name.c_str(),"EYEROI", "RectW" );

    m_nROTRectH = GetIniKeyInt((char*)name.c_str(),"EYEROI", "RectH");

    m_nCenterX = GetIniKeyInt((char*)name.c_str(),"MASK", "CenterPtX");

    m_nCenterY = GetIniKeyInt((char*)name.c_str(),"MASK", "CenterPtY");

    m_nRadius = GetIniKeyInt((char*)name.c_str(),"MASK", "Radius");

    m_nCutBellowHeight = GetIniKeyInt((char*)name.c_str(),"MASK", "CutBellowHeight");

    m_nRightUpRadius = GetIniKeyInt((char*)name.c_str(),"MASK", "RightUpRadius");

    m_nLeftOrRight = GetIniKeyInt((char*)name.c_str(),"MASK", "LeftOrRight");

    m_nHasClass = GetIniKeyInt((char*)name.c_str(),"MASK", "HasClass");

    int nEnable = GetIniKeyInt((char*)name.c_str(),"SMOOTH", "Enable");
    m_bSmoothEnabled = (nEnable == 0) ? false : true;
    if (m_bSmoothEnabled)
    {
        m_fSmoothTh = GetIniKeyFloat((char*)name.c_str(),"SMOOTH", "Th");
        m_fSmoothEx = GetIniKeyFloat((char*)name.c_str(),"SMOOTH", "Ex");
        m_nTimeLimit = GetIniKeyInt((char*)name.c_str(),"SMOOTH", "TimeLimit");
    }

    m_nDeviceType = GetIniKeyInt((char*)name.c_str(),"CAMERA", "DeviceType");
    m_nMethodType = GetIniKeyInt((char*)name.c_str(),"METHOD", "Type");

    return 0;

}