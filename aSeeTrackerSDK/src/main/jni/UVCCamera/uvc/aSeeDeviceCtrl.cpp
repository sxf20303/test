#include "aSeeDeviceCtrl.h"

#define CMD_SIZE 64

#define ASEE_LIGHT_I2C0
#define ASEE_LIGHT_I2C1
#define ASEE_LIGHT_OV580
#define ASEE_LIGHT_SCCB_BIT

#define XU_TASK_SET          0x50
#define XU_TASK_GET          0x51
#define XU_TASK_ACK          0x52
#define XU_TASK_FINISH       0x53
#define XU_TASK_ADDR         0x54

typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef unsigned long ULONG;

#define ASIC_INT_NULL        0xa2
#define ASIC_INT_NULL_I2C    0xa3
#define ASIC_INT_I2C         0xa5

#define ASEE_LIGHT_SAFE_DELETE(ptr) { if(ptr){free(ptr); ptr = 0;} }

static ULONG LightState[10] = {0x00, 0x02,0x0f, 0x1f, 0x2f, 0x3f, 0x4f, 0x5f, 0x6f, 0x7f};

#define LOGE printf

//uvc_context_t *ctx;
//uvc_device_t *dev;
//uvc_device_handle_t *devh;
const uvc_extension_unit_t *unit;

int aa_asic_init(uvc_device_handle_t *devh)
{
	long hr = 0;	
	BYTE buf[512];
	BYTE wakeCnt = 0;

	memset(buf,0,512);
	buf[0] = XU_TASK_SET;
	//if(unit == NULL)
	unit = uvc_get_extension_units(devh);
	while(1) {
		//Sleep(1);
		buf[0] = 0x51;
		buf[1] = 0x52;
		buf[254] = 0x53;
		buf[255] = 0x54;
		hr = uvc_set_ctrl(devh,unit->bUnitID,0x02,buf,0x180);
		LOGE("1_aa_asic_init hr = %d\n",hr);
		buf[0] = 0xa1;
		buf[1] = 0xa2;
		buf[254] = 0xa3;
		buf[255] = 0xa4;
		hr = uvc_get_ctrl(devh,unit->bUnitID,0x02,buf,0x180,UVC_GET_CUR);
		if(hr >= 0) break;		
		hr = uvc_set_ctrl(devh,unit->bUnitID,0x02,buf,0x40);
		LOGE("2_aa_asic_init hr = %d\n",hr);
		if(hr >= 0) break;
		wakeCnt++;
		if (wakeCnt > 10) break;
	}
	hr = uvc_get_ctrl_len(devh,unit->bUnitID,0x02);
	LOGE("3_aa_asic_init end hr = %d\n",hr);
	if(hr >=0){
		LOGE("aa_asic_init hr = %d\n",hr);
		return 0;
	}

	return -1;
}

int aa_asic_write(uvc_device_handle_t *devh,int extension,WORD num_bytes,BYTE * data_out)
{	
	long hr;
	BYTE *xu_buf;
	xu_buf = NULL;
	xu_buf = (BYTE *)malloc(64);
	memset(xu_buf, 0,64);
	WORD i;
	unsigned int addr,limit;

	xu_buf[0] = XU_TASK_SET;
	xu_buf[1] = *(data_out+0);
	xu_buf[2] = *(data_out+1);
	xu_buf[3] = *(data_out+2);  //addr width
	xu_buf[4] = *(data_out+3);  //value width	

	LOGE("%x\n",xu_buf[3]);
	LOGE("%x\n",xu_buf[4]);

	addr = (*(data_out+4)<<24) + (*(data_out+5)<<16) + (*(data_out+6)<<8) + (*(data_out+7)<<0); 
	xu_buf[5] = ((addr)>>24) & 0xff;
	xu_buf[6] = ((addr)>>16) & 0xff;	
	xu_buf[7] = ((addr)>>8) & 0xff;
	xu_buf[8] = (addr) & 0xff;
	xu_buf[11] = ((num_bytes-8)>>8) & 0xff;
	xu_buf[12] = ((num_bytes-8)>>0) & 0xff;
	for(i=0; i<num_bytes-8; i+=32){		
		if(num_bytes-i-8 > 32){
			limit = 32;			
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
		if(i+32>=num_bytes-8){
			xu_buf[9] = xu_buf[9] | 0x80;
		}
		memcpy(&xu_buf[16], data_out+i+8,limit);
	    //uvc_extension_unit_t *unit = uvc_get_extension_units(devh);
		LOGE("uvc_set_ctrl");
		hr = uvc_set_ctrl(devh,unit->bUnitID,0x02,xu_buf,64);
		LOGE("156 hr = %d\n",hr);
	}	

	if(xu_buf) 
		free(xu_buf);
	return 1;
}

int aa_asic_read(uvc_device_handle_t *devh,int extension,WORD num_bytes,BYTE * data_in)
{
	long hr;
	BYTE *xu_buf;
	xu_buf = NULL;
	xu_buf = (BYTE *)malloc(64);
	memset(xu_buf, 0,64);
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
	for(i=0; i<num_bytes-8; i+=32){		
		if(num_bytes-i-8 > 32){
			limit = 32;			
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
		if(i+32>=num_bytes-8){
			xu_buf[9] = xu_buf[9] | 0x80;
		}
		hr = uvc_set_ctrl(devh,unit->bUnitID,0x02,xu_buf,64);
		
		LOGE("set_hr = %d\n",hr);

		BYTE *re_buf;
		re_buf = NULL;
		re_buf = (BYTE *)malloc(64);
		memset(re_buf, 0,64);

		BYTE *temp_buf;
		temp_buf = NULL;
		temp_buf = (BYTE *)malloc(64);
		memset(temp_buf, 0,64);
		
		hr = uvc_get_ctrl(devh,unit->bUnitID,0x02,re_buf,64,UVC_GET_CUR);
		LOGE("get_hr = %d\n",hr);

		LOGE("get_hr = %d\n",hr);

		memcpy(temp_buf,re_buf+1,31);

		memcpy(data_in+i+8,&temp_buf[16],limit);
		if(re_buf) free(re_buf);
		if(temp_buf) free(temp_buf);
	}		

	if(xu_buf) free(xu_buf);
	return 1;
}

int _light_control(uvc_device_handle_t *devh,ULONG id, ULONG addr, ULONG value)
{
	int retval = ASEE_DEVICE_SUCCESS;
	do
	{
		ULONG ulId = id;
		ULONG ulAddr = addr;
		ULONG ulValue = value;

		BYTE* sbuf = (BYTE *)malloc(64);
		memset(sbuf, 0, 64);

		sbuf[0] = ASIC_INT_NULL_I2C;
		sbuf[2] = 1;

		sbuf[1] = (BYTE)ulId & 0xff;	
		sbuf[3] = 1;	

		for (BYTE i = 0; i < 4; i++) 
		{
			sbuf[4+i] = (BYTE)((ulAddr>>(3-i)*8)) & 0xff;
		}

		for (BYTE i = 0; i < 1; i++) 
		{
			sbuf[8+i] = (BYTE)((ulValue>>(1-1-i)*8)) & 0xff;
		}

		aa_asic_write(devh,0, 1*1+8, sbuf);

		ASEE_LIGHT_SAFE_DELETE(sbuf);

	} while (false);

	return retval;
}

int read_light_control(uvc_device_handle_t *devh,ULONG id, ULONG addr)
{
	int retval = ASEE_DEVICE_SUCCESS;
	do
	{
		int h_extension = 0;
		h_extension = aa_asic_init(devh);
		if(h_extension < 0){
			retval = ASEE_DEVICE_FAILED;
			break;
		}

		ULONG ulId = id;
		ULONG ulAddr = addr;
		ULONG ulValue ;//= value;

		BYTE* sbuf = (BYTE *)malloc(64);
		memset(sbuf, 0, 64);

		sbuf[0] = ASIC_INT_NULL_I2C;
		sbuf[2] = 1;

		sbuf[1] = (BYTE)ulId & 0xff;	
		sbuf[3] = 1;	

		for (BYTE i = 0; i < 4; i++) 
		{
			sbuf[4+i] = (BYTE)((ulAddr>>(3-i)*8)) & 0xff;
		}

		/*for (BYTE i = 0; i < 1; i++) 
		{
			sbuf[8+i] = (BYTE)((ulValue>>(1-1-i)*8)) & 0xff;
		}*/

		aa_asic_read(devh,0, 1*1+8, sbuf);

		ulValue = 0;
		for(BYTE i=0; i<1; i++) {
			ulValue = ((sbuf[8+i]<<(8*(1-1-i))));				
		}

		LOGE("read_ulValue = %ld",ulValue);

		ASEE_LIGHT_SAFE_DELETE(sbuf);


	} while (false);

	return retval;
}

int brightness(uvc_device_handle_t *devh,int value) // ¡¡∂»µ˜Ω⁄ checked I2C0 8/16
{
	int retval = ASEE_DEVICE_SUCCESS;

	do
	{
		int h_extension = 0;
		h_extension = aa_asic_init(devh);
		if(h_extension < 0){
			retval = ASEE_DEVICE_FAILED;
			break;
		}
		int _value = value;

		if (_value < 0){
			_value = 0;
		}

		if (_value > 9){
			_value = 9;
		}

		if (1 == value)
		{
			if(ASEE_DEVICE_SUCCESS != _light_control(devh,0xC6, 0x0A, LightState[1]))
			{
				retval = ASEE_DEVICE_FAILED;
				break;
			}

			LOGE("LED ON");
			if(ASEE_DEVICE_SUCCESS != _light_control(devh,0xC6, 0x09, 0x0f))
			{
				retval = ASEE_DEVICE_FAILED;
				break;
			}
		}
		else if(0 == value)
		{
			if(ASEE_DEVICE_SUCCESS != _light_control(devh,0xC6, 0x0A, LightState[0]))
			{
				retval = ASEE_DEVICE_FAILED;
				break;
			}
		}
		else
		{
			if(ASEE_DEVICE_SUCCESS != _light_control(devh,0xC6, 0x09, LightState[_value]))
			{
				retval = ASEE_DEVICE_FAILED;
				break;
			}
		}

	} while (false);

	return retval;
}

//ÊõùÂÖâ checked I2C0
int private_exposure(uvc_device_handle_t *devh,ULONG id, ULONG addr, ULONG value)
{
	int retval = ASEE_DEVICE_SUCCESS;

	do
	{
		int h_extension = 0;
		h_extension = aa_asic_init(devh);
		if(h_extension < 0){
			retval = ASEE_DEVICE_FAILED;
			break;
		}

		ULONG ulId = id;
		ULONG ulAddr = addr;
		ULONG ulValue = value;

		BYTE* sbuf = (BYTE *)malloc(64);
		memset(sbuf, 0, 64);

		sbuf[0] = ASIC_INT_NULL_I2C;
		sbuf[2] = 2;

		sbuf[1] = (BYTE)ulId & 0xff;	
		sbuf[3] = 1;		
		for (BYTE i = 0; i < 4; i++) 
		{
			sbuf[4+i] = (BYTE)((ulAddr>>(3-i)*8)) & 0xff;
		}

		for (BYTE i = 0; i < 1; i++) 
		{
			sbuf[8+i] = (BYTE)((ulValue>>(1-1-i)*8)) & 0xff;
		}

		aa_asic_write(devh,0, 1*1+8, sbuf);

		ASEE_LIGHT_SAFE_DELETE(sbuf);

	} while (false);

	return retval;
}


int exposure(uvc_device_handle_t *devh,int value)
{
	int retval = ASEE_DEVICE_SUCCESS;

	do
	{
		if(value <= 0)
		{
			value = 0x0;
		}

		if(value > 0x458)
		{
			value = 0x458;
		}

		unsigned long high_8 = value >> 4;
		unsigned long low_8 = value & 0x00F;
		low_8 = low_8 * 0x10;
		if(ASEE_DEVICE_SUCCESS != private_exposure(devh,0x20, 0x3501, high_8))
		{
			retval = ASEE_DEVICE_FAILED;
			break;
		}

		if(ASEE_DEVICE_SUCCESS != private_exposure(devh,0x20, 0x3502, low_8))
		{
			retval = ASEE_DEVICE_FAILED;
			break;
		}

	} while (false);

	return retval;	
}

//Â¢ûÁõä checked I2C0
int private_gain(uvc_device_handle_t *devh,ULONG id, ULONG addr, ULONG value)
{
	int retval = ASEE_DEVICE_SUCCESS;

	do
	{
		int h_extension = 0;
		h_extension = aa_asic_init(devh);
		if(h_extension < 0){
			retval = ASEE_DEVICE_FAILED;
			break;
		}

		ULONG ulId = id;
		ULONG ulAddr = addr;
		ULONG ulValue = value;

		BYTE* sbuf = (BYTE *)malloc(64);
		memset(sbuf, 0, 64);

		sbuf[0] = ASIC_INT_NULL_I2C;
		sbuf[2] = 2;

		sbuf[1] = (BYTE)ulId & 0xff;	
		sbuf[3] = 1;		
		for (BYTE i = 0; i < 4; i++) 
		{
			sbuf[4+i] = (BYTE)((ulAddr>>(3-i)*8)) & 0xff;
		}

		for (BYTE i = 0; i < 1; i++) 
		{
			sbuf[8+i] = (BYTE)((ulValue>>(1-1-i)*8)) & 0xff;
		}

		aa_asic_write(devh,0, 1*1+8, sbuf);

		ASEE_LIGHT_SAFE_DELETE(sbuf);

	} while (false);

	return retval;
}

int gain(uvc_device_handle_t *devh,int valueA,int valueB)
{
	int retval = ASEE_DEVICE_SUCCESS;

	do
	{
		unsigned long high_8 = valueA;
		if (high_8 >= 7) {
			high_8 = 7;
		}else if(high_8 <= 0){
			high_8 = 0;
		}
		unsigned long low_8 = valueB;

		if (low_8 > 0x78) {
			low_8 = 0x78;
		}

		LOGD("gain___ valueA = %d,valueB = %d\n",valueA,valueB);
		switch (high_8) {
			case 0:
				if(ASEE_DEVICE_SUCCESS != private_gain(devh,0x20, 0x366A, 0))
				{
					retval = ASEE_DEVICE_FAILED;
				}
				break;
			case 1:
				if(ASEE_DEVICE_SUCCESS != private_gain(devh,0x20, 0x366A, 1))
				{
					retval = ASEE_DEVICE_FAILED;
				}
				break;
			case 3:
				if(ASEE_DEVICE_SUCCESS != private_gain(devh,0x20, 0x366A, 3))
				{
					retval = ASEE_DEVICE_FAILED;
				}
				break;
			case 7:
				if(ASEE_DEVICE_SUCCESS != private_gain(devh,0x20, 0x366A, 7))
				{
					retval = ASEE_DEVICE_FAILED;
				}
				break;
			default:
				retval = ASEE_DEVICE_FAILED;
				break;
		}

		if(ASEE_DEVICE_FAILED == retval)
		{
			break;
		}

		if(ASEE_DEVICE_SUCCESS != private_gain(devh,0x20, 0x3508, high_8))
		{
			retval = ASEE_DEVICE_FAILED;
			break;
		}

		if(ASEE_DEVICE_SUCCESS != private_gain(devh,0x20, 0x3509, low_8))
		{
			retval = ASEE_DEVICE_FAILED;
			break;
		}

	} while (false);

	return retval;
}
/*
int gain(uvc_device_handle_t *devh,int value)
{
	int retval = ASEE_DEVICE_SUCCESS;

	do
	{
		if(value <= 0)
		{
			value = 0x0;
		}

		if(value > 0x07FF)
		{
			value = 0x07FF;
		}

		unsigned long high_8 = value >> 8;
		unsigned long low_8 = value & 0x00FF;
		if(ASEE_DEVICE_SUCCESS != private_gain(devh,0xC0, 0x350A, high_8))
		{
			retval = ASEE_DEVICE_FAILED;
			break;
		}

		if(ASEE_DEVICE_SUCCESS != private_gain(devh,0xC0, 0x350B, low_8))
		{
			retval = ASEE_DEVICE_FAILED;
			break;
		}

	} while (false);

	return retval;
}
*/
