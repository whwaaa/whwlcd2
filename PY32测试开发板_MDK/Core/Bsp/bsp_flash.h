#ifndef __BSP__FLASH__H
#define __BSP__FLASH__H

#include "common.h"

//Flash每页128字节，共160页20KB
#define FLASH_ADDR_PAGE1	0x08000000//首页地址
#define FLASH_ADDR_PAGE160	0x08004F80//最后一页地址

typedef struct _USER_DATA {
	uint16_t uxVcc;
	uint16_t uxGnd;
	uint16_t uyVcc;
	uint16_t uyGnd;
} USER_DATA;
extern USER_DATA udata;

void user_flash_erase( void );//擦除用户flash
void read_data_from_flash( void );//从flash中读取数据
void write_data_into_flash( void );//写入数据到flash


#endif /*__BSP__FLASH__H*/



