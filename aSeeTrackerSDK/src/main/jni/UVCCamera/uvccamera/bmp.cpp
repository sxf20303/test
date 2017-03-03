//
// Created by 韩呈杰 on 16/2/2.
//

#include "bmp.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

void Snapshot( BYTE * pData, int width, int height,int num)
{
    time_t now;
    struct tm *curTime;
   char filename[256];

    memset(filename,0,256);

    now = time(NULL);
    curTime = localtime(&now);
    sprintf(filename,"/sdcard/aSeePro/%04d%02d%02d%02d%02d%02d%02d.bmp",curTime->tm_year+1900,
            curTime->tm_mon+1,curTime->tm_mday,curTime->tm_hour,curTime->tm_min,
            curTime->tm_sec,num);

    //LOGE("filename = %s\n",filename);

    int size = width * height * 3; // 每个像素点3个字节
    // 位图第一部分，文件信息
    BMPFILEHEADER_T bfh;
    bfh.bfType = 0x4D42;
    bfh.bfSize = (unsigned long)(size + sizeof( BMPFILEHEADER_T ) + sizeof( BMPINFOHEADER_T ));
    bfh.bfReserved1 = 0; // reserved
    bfh.bfReserved2 = 0; // reserved
    bfh.bfOffBits = bfh.bfSize - size;

    // 位图第二部分，数据信息
    BMPINFOHEADER_T bih;
    bih.biSize = sizeof(BMPINFOHEADER_T);
    bih.biWidth = width;
    bih.biHeight = height;
    bih.biPlanes = 1;
    bih.biBitCount = 24;
    bih.biCompression = 0;
    bih.biSizeImage = size;
    bih.biXPelsPerMeter = 0;
    bih.biYPelsPerMeter = 0;
    bih.biClrUsed = 0;
    bih.biClrImportant = 0;
    FILE * fp = fopen( filename,"wb+");
    if( !fp ) {
        //LOGE ("open %s faile\v",filename);
        return;
    }
    fwrite( &bfh, 1, sizeof(BMPFILEHEADER_T), fp );
    fwrite( &bih, 1, sizeof(BMPINFOHEADER_T), fp );
    fwrite( pData, 1, size, fp );
    fclose( fp );
}

bool SaveBMP(unsigned char* pdata,int width, int height,int num)
{
    time_t now;
    struct tm *curTime;
    char filename[256];

    memset(filename,0,256);

    now = time(NULL);
    curTime = localtime(&now);
    sprintf(filename,"/sdcard/aSeePro/%04d%02d%02d%02d%02d%02d%02d.bmp",curTime->tm_year+1900,
            curTime->tm_mon+1,curTime->tm_mday,curTime->tm_hour,curTime->tm_min,
            curTime->tm_sec,num);

    int nAlignWidth = (width * 24 + 31) / 32;
    BMPFILEHEADER_T Header;
    BMPINFOHEADER_T HeaderInfo;
    Header.bfType = 0x4D42;
    Header.bfReserved1 = 0;
    Header.bfReserved2 = 0;
    Header.bfOffBits = (unsigned long)(sizeof(BMPFILEHEADER_T)+sizeof(BMPINFOHEADER_T));
    Header.bfSize = (unsigned long)(sizeof(BMPFILEHEADER_T)+sizeof(BMPINFOHEADER_T)+nAlignWidth* height * 4);
    HeaderInfo.biSize = sizeof(BMPINFOHEADER_T);
    HeaderInfo.biWidth = width;
    HeaderInfo.biHeight = height;
    HeaderInfo.biPlanes = 1;
    HeaderInfo.biBitCount = 24;
    HeaderInfo.biCompression = 0;
    HeaderInfo.biSizeImage = 4 * nAlignWidth * height;
    HeaderInfo.biXPelsPerMeter = 0;
    HeaderInfo.biYPelsPerMeter = 0;
    HeaderInfo.biClrUsed = 0;
    HeaderInfo.biClrImportant = 0;
    FILE *pfile;
    if (!(pfile = fopen(filename, "wb+")))
    {
        //LOGE ("open %s faile\v",filename);
        return false;
    }
    fwrite(&Header, 1, sizeof(BMPFILEHEADER_T), pfile);
    fwrite(&HeaderInfo, 1, sizeof(BMPINFOHEADER_T), pfile);
    fwrite(pdata, 1, HeaderInfo.biSizeImage, pfile);
    fclose(pfile);
    //LOGE ("open %s usccess\n",filename);
    return true;
}

int SnapshotBmp(const char* pszPath, unsigned char* pImage, int nWidth, int nHeight, int nBitCount)
{
	//LOGE ("SnapshotBmp--------------------------\v");

	static int num = 0;
	++num;

	int nReturnValue = 0;

    do
    {
		time_t now;
		struct tm *curTime;
		char filename[256];

		memset(filename, 0, 256);

		now = time(NULL);
		curTime = localtime(&now);
		sprintf(filename,
				"/sdcard/asee/%04d%02d%02d%02d%02d%02d%02d.bmp",
				curTime->tm_year + 1900, curTime->tm_mon + 1, curTime->tm_mday,
				curTime->tm_hour, curTime->tm_min, curTime->tm_sec, num);

		FILE *pfile;
		if (!(pfile = fopen(filename, "wb+"))) {
			//LOGE ("open %s faile\v",filename);
			break;
		}

        int nColorTableSize = 0;
        if(nBitCount == 8)
        {
            nColorTableSize = sizeof(RGBQUAD_T) * 256;
        }

        int nLineByte = ((nWidth * nBitCount >> 3) + 3) / 4 * 4;
        int nBufSize = nLineByte * nHeight;

        BMPFILEHEADER_T bfh;
        memset( &bfh, 0, sizeof( bfh ) );
        bfh.bfType = 'MB';
        bfh.bfOffBits = sizeof( BMPINFOHEADER_T ) + sizeof( BMPFILEHEADER_T ) + nColorTableSize;

        if(nBitCount == 8)
        {
            bfh.bfSize = sizeof( BMPFILEHEADER_T ) + sizeof( BMPINFOHEADER_T ) + nColorTableSize
                + nBufSize;
        }
        else
        {
            bfh.bfSize = sizeof( BMPFILEHEADER_T ) + sizeof( BMPINFOHEADER_T ) + nBufSize;
        }

        DWORD nWritten = 0;
		fwrite(&bfh, 1, sizeof(bfh), pfile);

        BMPINFOHEADER_T bih;
        memset( &bih, 0, sizeof( bih ) );
        bih.biSize = sizeof( bih );
        bih.biWidth = nWidth;
        bih.biHeight = nHeight;
        bih.biPlanes = 1;
        bih.biBitCount = nBitCount;
        bih.biSizeImage = nBufSize;

		fwrite(&bih, 1, sizeof(bih), pfile);

        if(nBitCount == 8)
        {
            RGBQUAD_T colorTable[256] = {0};
            for(int i = 0; i < 256; i++)
            {
                colorTable[i].rgbBlue = colorTable[i].rgbGreen = colorTable[i].rgbRed = colorTable[i].rgbReserved = (unsigned char)i;
            }
    		fwrite(colorTable, 1, nColorTableSize, pfile);
        }

		fwrite(pImage, 1, nBufSize, pfile);
		fclose(pfile);

		nReturnValue = 1;

    } while (false);

    return nReturnValue;
}
