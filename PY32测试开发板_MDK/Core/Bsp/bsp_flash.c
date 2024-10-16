#include "bsp_flash.h"

USER_DATA udata;

//擦除用户flash
void user_flash_erase( void ) {
	uint32_t pageError = 0;

	FLASH_EraseInitTypeDef eraseInitStruc;
	eraseInitStruc.TypeErase   = FLASH_TYPEERASE_PAGEERASE;   /* 擦写类型FLASH_TYPEERASE_PAGEERASE=Page擦, FLASH_TYPEERASE_SECTORERASE=Sector擦 */
	eraseInitStruc.PageAddress = FLASH_ADDR_PAGE160;          /* 擦写起始地址 */
	eraseInitStruc.NbPages  = ( sizeof(USER_DATA) + (FLASH_PAGE_SIZE-1) ) / FLASH_PAGE_SIZE;    /* 需要擦写的页数量 */
	// __HAL_FLASH_CLEAR_FLAG(
	// 	FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
	// FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
	//解锁FLASH
	HAL_FLASH_Unlock();
	//擦除FLASH
	if (HAL_FLASHEx_Erase(&eraseInitStruc, &pageError) != HAL_OK) {/* 执行page擦除,pageError返回擦写错误的page,返回0xFFFFFFFF,表示擦写成功 */
		while(1);
	}
	//锁定FLASH
  	HAL_FLASH_Lock();
}

//从flash中读取数据
void read_data_from_flash( void ) {
	memcpy(&udata, (uint32_t*)FLASH_ADDR_PAGE160, sizeof(USER_DATA));
	//若flash数据为空，初始化出厂参数
	if ( udata.uxVcc==0xFFFF && udata.uxGnd==0xFFFF && 
		 udata.uyVcc==0xFFFF && udata.uyGnd==0xFFFF ) {
		udata.uxVcc = INIT_UXVCC;
		udata.uxGnd = INIT_UXGND;
		udata.uyVcc = INIT_UYVCC;
		udata.uyGnd = INIT_UYGND;
	}
	#ifdef USE_UART
	printf("读取到校准参数：uxVcc:%d uxGnd:%d uyVcc:%d uyGnd:%d\n", udata.uxVcc, udata.uxGnd, udata.uyVcc, udata.uyGnd);
	#endif
}

//写入数据到flash
void write_data_into_flash( void ) {
	//擦除FLASH
	user_flash_erase();
	HAL_FLASH_Unlock();
	//数据写入FLASH
	HAL_FLASH_PageProgram(FLASH_ADDR_PAGE160, (uint32_t*)&udata);
	HAL_FLASH_Lock();
	/* STM32中数据只能传入变量非指针，此部分程序是传入4字节变量的写法做个参考
	uint8_t i, j, *pudata;
	uint16_t len_t;
	uint32_t addr;
	uint32_t da_t;
	
	addr = FLASH_ADDR_PAGE160;
	pudata = (uint8_t*)&udata;
	len_t = sizeof( USER_DATA );
	user_flash_erase();
	HAL_FLASH_Unlock();
	i = len_t / 8;
	j = len_t % 8;
	for( ; i>0; i-- ) {
		da_t = (uint32_t)pudata[0]<<24;
		da_t |= (uint32_t)pudata[1]<<16;
		da_t |= (uint32_t)pudata[2]<<8;
		da_t |= (uint32_t)pudata[3]<<0;
		//数据写入FLASH
		HAL_FLASH_PageProgram(addr, da_t);
		addr += 8;
		pudata += 8;
	}
	if ( j > 0 ) {
		if ( j >= 1 ) { da_t = (uint32_t)pudata[0]<<24; }
		if ( j >= 2 ) { da_t |= (uint32_t)pudata[1]<<16; }
		if ( j >= 3 ) { da_t |= (uint32_t)pudata[2]<<8; }
		if ( j >= 4 ) { da_t |= (uint32_t)pudata[3]<<0; }
		//数据写入FLASH
		HAL_FLASH_PageProgram(addr, da_t);
	}
	HAL_FLASH_Lock(); 
	*/
}






