//
// Created by 韩呈杰 on 16/2/2.
//

#ifndef ASEEMOBILESDK_BMP_H
#define ASEEMOBILESDK_BMP_H
#include <cstdint>

typedef long BOOL;
typedef long LONG;
typedef unsigned char BYTE;
typedef unsigned long DWORD;
typedef unsigned short WORD;

typedef struct   //  保存 BMP  用
{
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
}__attribute__((packed)) BMPFILEHEADER_T;

typedef struct
{
    uint32_t biSize;
    int32_t biWidth;
    int32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t biXPelsPerMeter;
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BMPINFOHEADER_T;

typedef struct {
        BYTE    rgbBlue;
        BYTE    rgbGreen;
        BYTE    rgbRed;
        BYTE    rgbReserved;
} RGBQUAD_T;

void Snapshot( BYTE * pData, int width, int height,int num);
bool SaveBMP(unsigned char* pdata,int width, int height,int num);
int SnapshotBmp(const char* pszPath, unsigned char* pImage, int nWidth, int nHeight, int nBitCount);


#endif //ASEEMOBILESDK_BMP_H
