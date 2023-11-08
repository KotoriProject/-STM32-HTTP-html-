#ifndef _MYFATFS_
#define _MYFATFS_

#include "fatfs.h"

typedef enum
{
    FS_UART_READY,
    FS_UART_BUSY,
} FS_UART_STATUS;

extern volatile FS_UART_STATUS FS_uart_status;

void fatfs_test(void);
void sdio_test(void);
HAL_StatusTypeDef sdio_read(uint8_t *data);
FRESULT myfatfs_open(FIL *file, const TCHAR *path,
                     void (*UART_TRANSMIT)(uint8_t *pData, uint16_t Size));
void fatfs_open(uint8_t *filename);
uint32_t fatfs_size(uint8_t *filename);
#endif
