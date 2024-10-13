#ifndef __COMMON_H__
#define __COMMON_H__

#define USE_32x32_FONT  0//32x32字模，测试板空间不足未使用
#define USE_24x24_FONT  0//24x24字模，测试板空间不足未使用
#define USE_ASCII16x16BOLD_FONT  0//加粗ASCII16x16字模，测试板空间不足未使用
#define DIS_STR0 "获取屏幕开发资料"
#define DIS_STR1 "微信搜索公众号："
#define DIS_STR2  "猫狗之家电子"

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

/* BSP ------------------------------------------------------------*/
#include "bsp_font.h"
#include "bsp_st7571.h"


/* Tools ------------------------------------------------------------*/


// #define FONT0 "获取屏幕开发资料"
// #define FONT1 "微信搜索公众号："
// #define FONT2 "猫狗之家电子"
// //#define SLIDE_STR "微信搜索公众号：猫狗之家电子获取屏幕开发资料 "
// #define DISDEMO1_STR_0  "猫狗"
// #define DISDEMO1_STR_1  "之家"
// #define DISDEMO1_STR_2  "电子"

#endif /* __COMMON_H__ */
