#ifndef __COMMON_H__
#define __COMMON_H__

#define DIS_STR0 "获取屏幕开发资料"
#define DIS_STR1 "微信搜索公众号："
#define DIS_STR2  "猫狗之家电子"

/* LIB ------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include "gpio.h"
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "spi.h"
#include "adc.h"
#include "stm32f1xx_it.h"

/* BSP ------------------------------------------------------------*/
#include "bsp_st7539.h"
#include "bsp_st7571.h"

/* FreeRTOS ------------------------------------------------------------*/
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define FW_VERSION  "V1.0.0"


#endif /* __COMMON_H__ */



