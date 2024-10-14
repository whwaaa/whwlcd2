#ifndef __BSP_TOUCH__H
#define __BSP_TOUCH__H

#include "common.h"

//无校准时的初始触摸参数
#define INIT_UXVCC 0x0F5A
#define INIT_UXGND 0x00D5
#define INIT_UYVCC 0x0E80
#define INIT_UYGND 0x01A7

typedef struct _PRESS_STR {
    uint16_t uxVcc1;
    uint16_t uxGnd1;
    uint16_t uxVcc2;
    uint16_t uxGnd2;
    uint16_t uyVcc1;
    uint16_t uyGnd1;
    uint16_t uyVcc2;
    uint16_t uyGnd2;
    uint16_t x;
    uint16_t y;
    uint8_t calibrationNum;//校准次数，四个角共四次
    uint8_t pressSta;//1按下 0弹起
} PRESS_STR;
extern PRESS_STR press_str;

//检测X坐标
#define TOUCH_X_PWR_ON \
    HAL_GPIO_WritePin(X_GPIO_Port, X_Pin, GPIO_PIN_RESET);\
    HAL_GPIO_WritePin(X__GPIO_Port, X__Pin, GPIO_PIN_SET)
//断开检测x电源
#define TOUCH_X_PWR_OFF \
    HAL_GPIO_WritePin(X_GPIO_Port, X_Pin, GPIO_PIN_SET);\
    HAL_GPIO_WritePin(X__GPIO_Port, X__Pin, GPIO_PIN_RESET)
//检测y坐标
#define TOUCH_Y_PWR_ON \
    HAL_GPIO_WritePin(Y_GPIO_Port, Y_Pin, GPIO_PIN_RESET);\
    HAL_GPIO_WritePin(Y__GPIO_Port, Y__Pin, GPIO_PIN_SET)
//断开检测y电源
#define TOUCH_Y_PWR_OFF \
    HAL_GPIO_WritePin(Y_GPIO_Port, Y_Pin, GPIO_PIN_SET);\
    HAL_GPIO_WritePin(Y__GPIO_Port, Y__Pin, GPIO_PIN_RESET)
//Y+接VCC，X-接GND
#define TOUCH_F_PWR_ON \
    HAL_GPIO_WritePin(Y_GPIO_Port, Y_Pin, GPIO_PIN_RESET);\
    HAL_GPIO_WritePin(X__GPIO_Port, X__Pin, GPIO_PIN_SET)
//断开检测触摸力度电源
#define TOUCH_F_PWR_OFF \
    HAL_GPIO_WritePin(Y_GPIO_Port, Y_Pin, GPIO_PIN_SET);\
    HAL_GPIO_WritePin(X__GPIO_Port, X__Pin, GPIO_PIN_RESET)
//接通按下检测电源
#define TOUCH_PRESS_START \
    HAL_GPIO_WritePin(X__GPIO_Port, X__Pin, GPIO_PIN_SET); \
//是否松手,true为松手
#define PRESS_OFF \
    (HAL_GPIO_ReadPin( TC_GPIO_Port, TC_Pin ) == GPIO_PIN_SET)
//等待松手
#define WAITING_PRESS_OFF(x) \
    { \
        TOUCH_PRESS_START;/*打开接通电源*/ \
        HAL_Delay(x); \
        while( !PRESS_OFF );/*等待松手*/ \
        __HAL_GPIO_EXTI_CLEAR_IT(TC_Pin); \
        HAL_NVIC_EnableIRQ(EXTI4_15_IRQn); \
    }

//断开所有检测电源
#define TOUCH_ALL_PWR_OFF \
    TOUCH_X_PWR_OFF;\
    TOUCH_Y_PWR_OFF
	
	
uint32_t touch_check_x( void );
uint32_t touch_check_y( void );
uint16_t my_abs_diff( int32_t a, int32_t b );



#endif /*__BSP_TOUCH__H*/


