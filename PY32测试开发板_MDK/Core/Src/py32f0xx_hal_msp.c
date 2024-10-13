/**
  ******************************************************************************
  * @file    py32f0xx_hal_msp.c
  * @author  MCU Application Team
  * @brief   This file provides code for the MSP Initialization
  *          and de-Initialization codes.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by Puya under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  * @attention
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* External functions --------------------------------------------------------*/

/**
  * @brief 初始化全局MSP
  */
void HAL_MspInit(void)
{
}


//ADC
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc) {

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = ADx_Pin | ADy_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc) {
  HAL_GPIO_DeInit(GPIOA, ADx_Pin | ADy_Pin );
}


//SPI
void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) {
  GPIO_InitTypeDef  GPIO_InitStruct;

  __HAL_RCC_GPIOB_CLK_ENABLE();                   /* GPIOB时钟使能 */
  __HAL_RCC_GPIOA_CLK_ENABLE();                   /* GPIOA时钟使能 */
  __HAL_RCC_SYSCFG_CLK_ENABLE();                  /* 使能SYSCFG时钟 */
  __HAL_RCC_SPI1_CLK_ENABLE();                    /* SPI1时钟使能 */

  //PA2 SPI1_MOSI
  //PA5 SPI1_SCK
  /*MOSI*/
  GPIO_InitStruct.Pin       = GPIO_PIN_2;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*SCK*/
  GPIO_InitStruct.Pin       = GPIO_PIN_5;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      =  GPIO_PULLDOWN;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF0_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  // /*中断配置*/
  // HAL_NVIC_SetPriority(SPI1_IRQn, 1, 0);
  // HAL_NVIC_EnableIRQ(SPI1_IRQn);
}
void HAL_SPI_MspDeInit(SPI_HandleTypeDef *hspi) {
  if (hspi->Instance == SPI1) {
    /*##-1- Reset peripherals ##################################################*/
    __HAL_RCC_SPI1_FORCE_RESET();
    __HAL_RCC_SPI1_RELEASE_RESET();

    /*##-2- Disable peripherals and GPIO Clocks ################################*/
    /* Deconfigure SPI SCK */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2 | GPIO_PIN_5);
    // HAL_NVIC_DisableIRQ(SPI1_IRQn);
  }
}



//UART
void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
  GPIO_InitTypeDef  GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    while(1);
  }

  /*USART1时钟使能*/
  __HAL_RCC_USART1_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();

  /* GPIO初始化
  PF0 RX,PF1 TX
  */
  GPIO_InitStruct.Pin       = GPIO_PIN_0 | GPIO_PIN_1;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull      = GPIO_NOPULL;
  GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_USART1;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

}
void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle) {
  if(uartHandle->Instance==USART1) {
    __HAL_RCC_USART1_CLK_DISABLE();
    //PF0     ------> USART1_RX
    //PF1     ------> USART1_TX
    HAL_GPIO_DeInit(GPIOF, GPIO_PIN_0|GPIO_PIN_1);
  }
}


//TIM
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) {
  GPIO_InitTypeDef   GPIO_InitStruct;
  /*TIM1时钟使能 */
  __HAL_RCC_TIM1_CLK_ENABLE();                              
  /*GPIOA时钟使能 */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  /*复用功能 */
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;                   
  /* 上拉 */
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;                       
  /* 高速*/
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  /*GPIOB3初始化*/
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/************************ (C) COPYRIGHT Puya *****END OF FILE****/
