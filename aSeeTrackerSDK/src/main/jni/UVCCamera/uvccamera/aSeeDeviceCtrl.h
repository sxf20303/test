#ifndef __ASEEDEVICECTRL_H_
#define __ASEEDEVICECTRL_H_

#define ASEE_DEVICE_SUCCESS 0L
#define ASEE_DEVICE_FAILED -1L

#include "libuvc/libuvc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//using namespace std;

#ifdef __cplusplus
extern "C"{
#endif

	/*
	*brief：调节灯光亮度
	*
	*param: devh uvc 设备句柄 ;
	*param: value 范围：[0~8]；0-关，1-开，2~8：亮度值
	*
	*return: 0-成功；非零失败
	*/
	int brightness(uvc_device_handle_t *devh,int value); // 浜害璋冭妭 checked I2C0 8/16


	/*
	*brief：调节相机曝光度
	*
	*param:
	*param: value 范围：[0~1112]
	*
	*return: 0-成功；非零失败
	*/
	int exposure(uvc_device_handle_t *devh,int value);


	/*
	*brief：调节相机增益
	*
	*param: devh uvc 设备句柄
	*param: valueA 范围：0,1,3,7
	*param: valueB 范围：0,[8~120]
	*
	*return: 0-成功；非零失败
	*/
	int gain(uvc_device_handle_t *devh,int valueA,int valueB);

	/*
	*brief：读取寄存器值
	*
	*param: devh uvc 设备句柄
	*param: id
	*param: 寄存器地址
	*
	*return: 0-成功；非零失败
	*/
	int read_light_control(uvc_device_handle_t *devh,unsigned long id, unsigned long addr);

#ifdef __cplusplus
}
#endif

#endif // __DEVICECTRL_H_
