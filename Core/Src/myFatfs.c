#include "myFatfs.h"
#include "stdio.h"
#include "string.h"

#define BLOCK_START_ADDR 0                                   /* Block start address      */
#define NUM_OF_BLOCKS 1                                      /* Total number of blocks   */
#define BUFFER_WORDS_SIZE ((BLOCKSIZE * NUM_OF_BLOCKS) >> 2) /* Total data size in bytes */

extern SD_HandleTypeDef hsd;

volatile FS_UART_STATUS FS_uart_status = FS_UART_BUSY;

void sdio_test()
{
    printf("Micro SD Card Test...\r\n");
    /* 检测SD卡是否正常（处于数据传输模式的传输状态） */
    if (HAL_SD_GetCardState(&hsd) == HAL_SD_CARD_TRANSFER)
    {
        printf("Initialize SD card successfully!\r\n");
        // 打印SD卡基本信息
        printf(" SD card information! \r\n");
        printf(" CardCapacity  : %llu \r\n", (unsigned long long)hsd.SdCard.BlockSize * hsd.SdCard.BlockNbr); // 显示容量
        printf(" CardBlockSize : %d \r\n", hsd.SdCard.BlockSize);                                             // 块大小
        printf(" LogBlockNbr   : %d \r\n", hsd.SdCard.LogBlockNbr);                                           // 逻辑块数量
        printf(" LogBlockSize  : %d \r\n", hsd.SdCard.LogBlockSize);                                          // 逻辑块大小
        printf(" RCA           : %d \r\n", hsd.SdCard.RelCardAdd);                                            // 卡相对地址
        printf(" CardType      : %d \r\n", hsd.SdCard.CardType);                                              // 卡类型
        // 读取并打印SD卡的CID信息
        HAL_SD_CardCIDTypeDef sdcard_cid;
        HAL_SD_GetCardCID(&hsd, &sdcard_cid);
        printf(" ManufacturerID: %d \r\n", sdcard_cid.ManufacturerID);
    }
    else
    {
        printf("SD card init fail!\r\n");
    }
}

HAL_StatusTypeDef sdio_read(uint8_t *data)
{
    HAL_SD_Erase(&hsd, 10, 10);
    uint8_t buf[] = {
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff};
    if (HAL_SD_Erase(&hsd, 10, 10) == HAL_OK)
    {
        /* Wait until SD cards are ready to use for new operation */
        while (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
            ;
        printf("\r\nErase Block Success!\r\n");
    }
    else
    {
        printf("\r\nErase Block Failed!\r\n");
    }
    if (HAL_SD_WriteBlocks_DMA(&hsd, buf, 10, 1) == HAL_OK)
    {
        while (HAL_SD_GetCardState(&hsd) != HAL_SD_CARD_TRANSFER)
            ;
        return HAL_SD_ReadBlocks(&hsd, data, 10, 1, 100);
    }
    else
        return HAL_ERROR;
}

__weak void FS_TRANS(uint8_t *pData, uint16_t Size)
{
}

FRESULT myfatfs_open(FIL *file, const TCHAR *path,
                     void (*UART_TRANSMIT)(uint8_t *pData, uint16_t Size))
{
    FRESULT res;
    uint32_t open_num = 0;

    res = f_open(file, path, FA_READ | FA_OPEN_EXISTING);
    if (res == FR_OK)
    {
        uint8_t tx_buf[512];
        uint64_t size = f_size(file);
        for (uint32_t i = 0; i < (size / 512); i++)
        {
            f_read(file, tx_buf, 512, &open_num);
            if (open_num != 512)
            {
                res = FR_DISK_ERR;
                printf("Error");
                break;
            }
            else
            {
                UART_TRANSMIT(tx_buf, 512);
            }
        }
        f_read(file, tx_buf, size % 512, &open_num);
        if (open_num != size % 512)
        {
            res = FR_DISK_ERR;
            printf("Error");
        }
        else
        {
            UART_TRANSMIT(tx_buf, open_num);
        }
        f_close(file);
    }
    return res;
}

void fatfs_test()
{
    FATFS fs;                    /* FatFs 文件系统对象 */
    FIL file;                    /* 文件对象 */
    FRESULT f_res;               /* 文件操作结果 */
    UINT fnum;                   /* 文件成功读写数量 */
    BYTE ReadBuffer[1024] = {0}; /* 读缓冲区 */
    BYTE WriteBuffer[] =         /* 写缓冲区 */
        "This is STM32 working with FatFs\r\n";
    // 在外部 SD 卡挂载文件系统，文件系统挂载时会对 SD 卡初始化
    f_res = f_mount(&fs, "0:", 1);

    /*----------------------- 格式化测试 ---------------------------*/
    printf("\r\n****** Register the file system object to the FatFs module ******\r\n");
    /* 如果没有文件系统就格式化创建创建文件系统 */
    if (f_res == FR_NO_FILESYSTEM)
    {
        printf("The SD card does not yet have a file system and is about to be formatted...\r\n");
        /* 格式化 */
        f_res = f_mkfs("0:", 0, 0);
        if (f_res == FR_OK)
        {
            printf("The SD card successfully formatted the file system\r\n");
            /* 格式化后，先取消挂载 */
            f_res = f_mount(NULL, "0:", 1);
            /* 重新挂载 */
            f_res = f_mount(&fs, "0:", 1);
        }
        else
        {
            printf("The format failed\r\n");
            while (1)
                ;
        }
    }
    else if (f_res != FR_OK)
    {
        printf(" mount error : %d \r\n", f_res);
        while (1)
            ;
    }
    else
    {
        printf(" mount sucess!!! \r\n");
    }

    /*----------------------- 文件系统测试：写测试 -----------------------------*/
    /* 打开文件，如果文件不存在则创建它 */
    printf("\r\n****** Create and Open new text file objects with write access ******\r\n");
    f_res = f_open(&file, "0:FatFs STM32cube.txt", FA_CREATE_ALWAYS | FA_WRITE);
    if (f_res == FR_OK)
    {
        printf(" open file sucess!!! \r\n");
        /* 将指定存储区内容写入到文件内 */
        printf("\r\n****** Write data to the text files ******\r\n");
        f_res = f_write(&file, WriteBuffer, sizeof(WriteBuffer), &fnum);
        if (f_res == FR_OK)
        {
            printf(" write file sucess!!! (%d)\n", fnum);
            printf(" write Data : %s\r\n", WriteBuffer);
        }
        else
        {
            printf(" write file error : %d\r\n", f_res);
        }
        /* 不再读写，关闭文件 */
        f_close(&file);
    }
    else
    {
        printf(" open file error : %d\r\n", f_res);
    }

    /*------------------- 文件系统测试：读测试 ------------------------------------*/
    printf("\r\n****** Read data from the text files ******\r\n");
    f_res = f_open(&file, "0:FatFs STM32cube.txt", FA_OPEN_EXISTING | FA_READ);
    if (f_res == FR_OK)
    {
        printf(" open file sucess!!! \r\n");
        f_res = f_read(&file, ReadBuffer, sizeof(ReadBuffer), &fnum);
        if (f_res == FR_OK)
        {
            printf("read sucess!!! (%d)\n", fnum);
            printf("read Data : %s\r\n", ReadBuffer);
        }
        else
        {
            printf(" read error!!! %d\r\n", f_res);
        }
    }
    else
    {
        printf(" open file error : %d\r\n", f_res);
    }
    /* 不再读写，关闭文件 */
    f_close(&file);
    /* 不再使用文件系统，取消挂载文件系统 */
    f_mount(NULL, "0:", 1);
    /* 操作完成，停机 */
}

void fatfs_open(uint8_t *filename)
{
    FS_uart_status = FS_UART_READY;
    FATFS fs;
    FIL file;
    f_mount(&fs, "0:", 1);

    myfatfs_open(&file, (const TCHAR *)filename, FS_TRANS);

    f_mount(NULL, "0:", 1);
}

uint32_t fatfs_size(uint8_t *filename)
{
    uint32_t filesize = 0;
    FATFS fs;
    FIL file;
    f_mount(&fs, "0:", 1);
    f_open(&file, (const TCHAR *)filename, FA_READ | FA_OPEN_EXISTING);
    filesize = f_size(&file);
    f_mount(NULL, "0:", 1);
    return filesize;
}
