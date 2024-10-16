#ifndef __COMMON_H__
#define __COMMON_H__

//Flash空间不足，串口功能关闭Flash空间仅剩余32字节，否则需要关闭其他功能
//#define USE_UART
#define VERSION "V1.0.0"

/* LIB ------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

/* Inc ------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "tim.h"
#include "spi.h"
#include "adc.h"
#include "py32f0xx_it.h"

/* BSP ------------------------------------------------------------*/
#include "bsp_flash.h"
#include "bsp_font.h"
#include "bsp_st7571.h"
#include "bsp_touch.h"


/* Tools ------------------------------------------------------------*/





#endif /* __COMMON_H__ */



