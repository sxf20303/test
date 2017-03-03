#ifndef _EXTENSION_H_
#define _EXTENSION_H_

#include "libusb.h"
#include "libuvc.h"
#include "utilbase.h"

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef  long HRESULT;
typedef unsigned long ULONG;

#define S_OK            ((HRESULT)0L)

#define WRITE_MODE 0
#define READ_MODE 1

#define XU_TASK_SET          0x50
#define XU_TASK_GET          0x51
#define XU_TASK_ACK          0x52
#define XU_TASK_FINISH       0x53
#define XU_TASK_ADDR         0x54

#define ACK_DLY 1
#define TRY_TIMES 2

#define CONFIGURE_IIC_BITRATE             0xB0
#define CONFIGURE_SPI_BIERATE             0xB1
#define CONFIGURE_SPI_PROTOCOL            0xB2

#define INTF_IIC             0xa0
#define INTF_SPI             0xa1
#define ASIC_INT_NULL        0xa2
#define ASIC_INT_NULL_I2C    0xa3
#define ASIC_INT_NULL_SPI    0xa4
#define ASIC_INT_I2C         0xa5
#define ASIC_INT_I2C_I2C     0xa6
#define ASIC_INT_I2C_SPI     0xa7
#define ASIC_INT_SPI         0xa8
#define ASIC_INT_SPI_I2C     0xa9
#define ASIC_INT_SPI_SPI     0xaa

#define XU_FW_VERSION        0xf0

typedef struct _GUID {
    unsigned long  Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char  Data4[ 8 ];
} GUID;

int aa_asic_read (uvc_device_handle_t *devh, WORD num_bytes,  BYTE * data_in);

int aa_asic_write (uvc_device_handle_t *devh, WORD num_bytes, BYTE * data_out);

int myReadRegisterValue(uvc_device_handle_t *devh);
int mySetRegisterValue(uvc_device_handle_t *devh,ULONG ulId,ULONG ulAddr,ULONG ulValue);

#endif
