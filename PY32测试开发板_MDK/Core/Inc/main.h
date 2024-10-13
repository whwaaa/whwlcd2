/**
  ******************************************************************************
  * @file    main.h
  * @author  MCU Application Team
  * @brief   Header for main.c file.
  *          This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "py32f0xx.h"

/* Private includes ----------------------------------------------------------*/
#define ADx_Pin GPIO_PIN_0
#define ADx_GPIO_Port GPIOA
#define ADy_Pin GPIO_PIN_1
#define ADy_GPIO_Port GPIOA
#define TC_Pin GPIO_PIN_7
#define TC_GPIO_Port GPIOA
#define DC_Pin GPIO_PIN_0
#define DC_GPIO_Port GPIOB
#define CS_Pin GPIO_PIN_1
#define CS_GPIO_Port GPIOB
#define RST_Pin GPIO_PIN_2
#define RST_GPIO_Port GPIOB
#define BL_Pin GPIO_PIN_3
#define BL_GPIO_Port GPIOA
#define X_Pin GPIO_PIN_4//X+ 拉低接通VCC
#define X_GPIO_Port GPIOA//X+
#define X__Pin GPIO_PIN_6//X- 拉高接通GND
#define X__GPIO_Port GPIOA//X-
#define Y_Pin GPIO_PIN_6//Y+ 拉低接通VCC 
#define Y_GPIO_Port GPIOB//Y+
#define Y__Pin GPIO_PIN_3//Y- 拉高接通GND
#define Y__GPIO_Port GPIOB//Y-
/* Private defines -----------------------------------------------------------*/
	
/* Exported variables prototypes ---------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void APP_ErrorHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
