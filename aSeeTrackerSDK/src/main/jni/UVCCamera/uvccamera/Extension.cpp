/*************************************************************************
	> File Name: main.c
	> Author: ma6174
	> Mail: ma6174@163.com
	> Created Time: 2015年12月31日 星期四 17时28分58秒
 ************************************************************************/

#include<stdio.h>
#include<malloc.h>
#include <string.h>
#include <unistd.h>
#include "Extension.h"

GUID PROPSETID_VIDCAP_EXTENSION_UNIT_2 = {0x2ccb0bda, 0x6331, 0x4fdb, {0x85, 0x0e, 0x79, 0x05, 0x4d, 0xbd, 0x56, 0x71}};
ULONG m_ulSize = 64;
WORD m_xuCtrlSize = 32;
DWORD m_dwExtensionNode = 3;

/*
static long get_Property(IKsControl *pIKsControl, DWORD dwExtensionNode, ULONG PropertyId, ULONG ulSize, BYTE* pValue)
{
    HRESULT hr = S_OK;
    KSP_NODE ExtensionProp;
    ULONG ulBytesReturned;

    ExtensionProp.Property.Set = PROPSETID_VIDCAP_EXTENSION_UNIT_2;
    ExtensionProp.Property.Id = PropertyId;
    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;
    ExtensionProp.NodeId = dwExtensionNode;

    hr = pIKsControl->KsProperty(
        (PKSPROPERTY) &ExtensionProp,
                  sizeof(ExtensionProp),
        (PVOID) pValue,
        ulSize,
        &ulBytesReturned);

    return hr;
}
*/
/*
static long put_Property(IKsControl *pIKsControl, DWORD dwExtensionNode, ULONG PropertyId, ULONG ulSize, BYTE* pValue)
{
    HRESULT hr = S_OK;
    KSP_NODE ExtensionProp;
    ULONG ulBytesReturned;

    ExtensionProp.Property.Set = PROPSETID_VIDCAP_EXTENSION_UNIT_2;
    ExtensionProp.Property.Id = PropertyId;
    ExtensionProp.Property.Flags = KSPROPERTY_TYPE_SET |
                                   KSPROPERTY_TYPE_TOPOLOGY;
    ExtensionProp.NodeId = dwExtensionNode;

    hr = pIKsControl->KsProperty(
        (PKSPROPERTY) &ExtensionProp,
                  sizeof(ExtensionProp),
        (PVOID) pValue,
        ulSize,
        &ulBytesReturned);

    return hr;
}
*/

static long ExtensionPut(uvc_device_handle_t *devh,ULONG ulSize, BYTE* pValue)   //  传输 大小 和 二进制 命令
{
	const uvc_extension_unit_t *unit = uvc_get_extension_units(devh);
	LOGE("unit = %d\n",unit->bUnitID);
	int ret = uvc_set_ctrl(devh,unit->bUnitID,2,pValue,ulSize);
	LOGE("ExtensionPut ret = %d",ret);
	if(ret > 0)
		return 0;//put_Property(m_pIKsControl, m_dwExtensionNode, 2, ulSize, pValue);
	else
		return (long)ret;
}

static long ExtensionGet(uvc_device_handle_t *devh,ULONG ulSize, BYTE* pValue)
{
	const uvc_extension_unit_t *unit = uvc_get_extension_units(devh);
	int ret = uvc_get_ctrl(devh,unit->bUnitID,2,pValue,ulSize,UVC_RC_UNDEFINED);
	LOGE("ExtensionGet ret = %d",ret);
	if(ret > 0)
		return ret;//get_Property(m_pIKsControl, m_dwExtensionNode, 2, ulSize, pValue);
	else
		return ret;
}

static HRESULT HandleGetTry(uvc_device_handle_t *devh,unsigned char *buf)
{
	HRESULT hr;
	unsigned char tryTime = 0;

	while (1) {
		hr = ExtensionGet(devh,m_ulSize, buf);
		LOGE("hr = %ld\n",hr);
	    if ((hr==S_OK) || (tryTime>TRY_TIMES)) {
			break;
		} else {
		   usleep(ACK_DLY*1000);
		   tryTime++;
		}
	}

	return hr;
}

static HRESULT WaitForAckGet(uvc_device_handle_t *devh,unsigned char *buff)
{
	HRESULT hr;
	unsigned char tryTime = 0;
	BYTE *xu_buf;
	xu_buf = NULL;
	xu_buf = (BYTE *)malloc(m_ulSize);
	memset(xu_buf,0, m_ulSize);

	while (1) {
		hr = HandleGetTry(devh,xu_buf);
		if (((hr==S_OK) && (xu_buf[0]==0x56)) || (tryTime>TRY_TIMES)) {
			break;
		} else {
		   usleep(ACK_DLY*1000);
		   tryTime++;
		}
	}

	memcpy(buff,xu_buf+1,m_ulSize-1);
	if(xu_buf) free(xu_buf);

	return hr;
}

static HRESULT HandleSetTry(uvc_device_handle_t *devh,unsigned char *buf)
{
	HRESULT hr;
	unsigned char tryTime = 0;

	while(1) {
		hr = ExtensionPut(devh,m_ulSize, buf);
		if ((hr==S_OK) || (tryTime>TRY_TIMES)) {
			break;
		} else {
		   usleep(ACK_DLY*1000);
		   tryTime++;
		}
	}
	return hr;
}

static HRESULT HandleChk(uvc_device_handle_t *devh,unsigned char *buf)
{
	HRESULT hr;
	unsigned char tryTime = 0;

	while (1) {
		hr = HandleSetTry(devh,buf);  //  发送数据到USB
		if((hr==S_OK) || (tryTime>TRY_TIMES)) {
			break;
		} else {
		   usleep(ACK_DLY*1000);
		   tryTime++;
		}
	}

	tryTime = 0;
	while (1) {
		hr = WaitForAckGet(devh,buf);
		if((hr==S_OK) || (tryTime>TRY_TIMES)) {
			break;
		} else {
		   usleep(ACK_DLY*1000);
		   tryTime++;
		}
	}

	return hr;
}


static bool isUvcDevice(uvc_device_handle_t *devh,ULONG *pPropSize, WORD *pCmdSize)
{
	HRESULT hr = S_OK;
	BYTE buf[512];
	BYTE wakeCnt = 0;

	memset(buf,0,512);
	buf[0] = XU_TASK_SET;


	while(1) {
		buf[0] = 0x51;
		buf[1] = 0x52;
		buf[254] = 0x53;
		buf[255] = 0x54;
		hr = ExtensionPut(devh,0x180, buf);
		buf[0] = 0xa1;
		buf[1] = 0xa2;
		buf[254] = 0xa3;
		buf[255] = 0xa4;
		hr = ExtensionGet(devh,0x180, buf);
		if(hr == S_OK) break;
		hr = ExtensionPut(devh,0x40, buf);
		if(hr == S_OK) break;
		wakeCnt++;
		if (wakeCnt > 10) break;
	}
	hr = 0;//get_PropertySize(m_pIKsControl, m_dwExtensionNode, 2, pPropSize);
	if (hr != S_OK) {
		//m_pIKsControl->Release();
		//m_pIKsControl = NULL;
		return false;
	}
	if(*pPropSize > 0x100) {
		*pCmdSize = 0x100;
	} else {
		*pCmdSize = 0x20;
	}

	return true;
}

int aa_asic_read (uvc_device_handle_t *devh, WORD num_bytes,BYTE * data_in)
{
    HRESULT hr;
	BYTE *xu_buf;
	xu_buf = NULL;
	xu_buf = (BYTE *)malloc(m_ulSize);
	memset(xu_buf,0, m_ulSize);
	WORD i;
	unsigned int addr,limit;

	xu_buf[0] = XU_TASK_GET;
	xu_buf[1] = *(data_in+0);
	xu_buf[2] = *(data_in+1);
	xu_buf[3] = *(data_in+2);  //addr width
	xu_buf[4] = *(data_in+3);  //value width
	addr = (*(data_in+4)<<24) + (*(data_in+5)<<16) + (*(data_in+6)<<8) + (*(data_in+7)<<0);
	xu_buf[5] = ((addr)>>24) & 0xff;
	xu_buf[6] = ((addr)>>16) & 0xff;
	xu_buf[7] = ((addr)>>8) & 0xff;
	xu_buf[8] = (addr) & 0xff;
	xu_buf[11] = ((num_bytes-8)>>8) & 0xff;
	xu_buf[12] = ((num_bytes-8)>>0) & 0xff;
	for(i=0; i<num_bytes-8; i+=m_xuCtrlSize){
		if(num_bytes-i-8 > m_xuCtrlSize){
			limit = m_xuCtrlSize;
		}else{
			limit = num_bytes-i-8;
		}
		xu_buf[9] = (limit>>8) & 0xff;
		xu_buf[10] = (limit>>0) & 0xff;
		//set page addr
		xu_buf[9] = xu_buf[9] & 0x0f;
		if(i == 0){
			xu_buf[9] = xu_buf[9] | 0x10;
		}
		//judge last package
		if(i+m_xuCtrlSize>=num_bytes-8){
			xu_buf[9] = xu_buf[9] | 0x80;
		}
		hr = HandleChk(devh,xu_buf);
		memcpy(data_in+i+8,&xu_buf[16],limit);
	}

	if(xu_buf)
		free(xu_buf);
	return 1;
}

int aa_asic_write (uvc_device_handle_t *devh,WORD num_bytes,BYTE * data_out)
{
    HRESULT hr;
	BYTE *xu_buf;
	xu_buf = NULL;
	xu_buf = (BYTE *)malloc(m_ulSize);
	memset(xu_buf,0, m_ulSize);
	WORD i;
	unsigned int addr,limit;

	xu_buf[0] = XU_TASK_SET;
	xu_buf[1] = *(data_out+0);
	xu_buf[2] = *(data_out+1);
	xu_buf[3] = *(data_out+2);  //addr width
	xu_buf[4] = *(data_out+3);  //value width

	addr = (*(data_out+4)<<24) + (*(data_out+5)<<16) + (*(data_out+6)<<8) + (*(data_out+7)<<0);
	xu_buf[5] = ((addr)>>24) & 0xff;
	xu_buf[6] = ((addr)>>16) & 0xff;
	xu_buf[7] = ((addr)>>8) & 0xff;
	xu_buf[8] = (addr) & 0xff;
	xu_buf[11] = ((num_bytes-8)>>8) & 0xff;
	xu_buf[12] = ((num_bytes-8)>>0) & 0xff;
	for(i=0; i<num_bytes-8; i+=m_xuCtrlSize){
		if(num_bytes-i-8 > m_xuCtrlSize){
			limit = m_xuCtrlSize;
		}
		else{
			limit = num_bytes-i-8;
		}
		xu_buf[9] = (limit>>8) & 0xff;
		xu_buf[10] = (limit>>0) & 0xff;
		//set page addr
		xu_buf[9] = xu_buf[9] & 0x0f;
		if(i == 0){
			xu_buf[9] = xu_buf[9] | 0x10;
		}
		//judge last package
		if(i+m_xuCtrlSize>=num_bytes-8){
			xu_buf[9] = xu_buf[9] | 0x80;
		}
		memcpy(&xu_buf[16], data_out+i+8,limit);
		hr = HandleChk(devh,xu_buf);
	}

	if(xu_buf)
		free(xu_buf);
	return 1;
}

int myReadRegisterValue(uvc_device_handle_t *devh)
{
	ULONG ulId = 32;
	ULONG ulAddr = 13569;
	ULONG ulValue = 0;

	BYTE *sbuf;
	BYTE i;

	sbuf = NULL;

	sbuf = (BYTE *) malloc(64);
	memset(sbuf,0,64);

	sbuf[0] = ASIC_INT_NULL_I2C;  //  0xa3
	sbuf[1] = (BYTE)ulId & 0xff;
	sbuf[3] = 1;

	for (i=0; i<4; i++) {
		sbuf[4+i] = (BYTE)((ulAddr>>(3-i)*8)) & 0xff;
	}
	aa_asic_read(devh, 1*1+8, sbuf);   //  读取 寄存器 值

	ulValue = 0;
	for(i=0; i<1; i++) {
		ulValue += ((sbuf[8+i]<<(8*(1-1-i))));
	}

	LOGE("*********************************");
	LOGE("ulValue = %ld",ulValue);
	LOGE("*********************************");

	if(sbuf) free(sbuf);
}

int mySetRegisterValue(uvc_device_handle_t *devh,ULONG ulId,ULONG ulAddr,ULONG ulValue)
{
	BYTE *sbuf;
	BYTE i;
	sbuf = NULL;

	sbuf = (BYTE *) malloc(64);
	memset(sbuf,0,64);

	sbuf[0] = ASIC_INT_NULL_I2C;  //  0xa3
	sbuf[1] = (BYTE)ulId & 0xff;
	sbuf[3] = 1;

	for (i=0; i<4; i++) {
		sbuf[4+i] = (BYTE)((ulAddr>>(3-i)*8)) & 0xff;
	}
	for (i=0; i<1; i++) {
		sbuf[8+i] = (BYTE)((ulValue>>(1-1-i)*8)) & 0xff;
	}

	aa_asic_write(devh,1*1+8,sbuf);     //  调用写方法实现 写寄存器

	if (sbuf) {
		free(sbuf);
	}
	return 0;
}

