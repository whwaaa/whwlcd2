/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private define ------------------------------------------------------------*/
#include "common.h"
/* Private variables ---------------------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
//重新定位中断向量表VECT_TAB_OFFSET到0x08000100
//static const char version[256]__attribute__((at(0x08000000)))= "V1.0.0";

/**
  * @brief  The application entry point
  * @retval int
  */
int main(void)
{
  /* Reset of all periqpherals, Initializes the Flash interface and the Systick */
  HAL_Init();

  /* Configure the system clock */
	APP_SystemClockConfig();
  GPIO_Init();
  TIM1_Init();
  #ifdef USE_UART
  UART1_Init();
  #endif
  SPI_Init();
	ADC_Init();

  #ifdef USE_UART
  printf("感谢关注猫狗之家电子！\n更多产品和教程正在开发中敬请期待！\n\n2.3寸4阶触摸\n128*96 SPI通信\nCCSB4736W G12896-08\n");
  #endif

  TOUCH_ALL_PWR_OFF;//关闭触摸电源
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);//背光PWM
  __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 50);//占空比
  st7571_lcd_init();//初始化
  read_data_from_flash();//读取FLASH数据

  TOUCH_PRESS_START;//X轴开启GND电源，检测引脚上拉通过100K电阻串接y轴低电平端，按下屏幕后，x轴y轴相连，检测引脚会被拉低，从而触发触摸中断
  page_str.page = PAGE_HOME;
  st7571_lcd_display_home();
  
  /* Infinite loop */
  while (1) {
    
    if ( press_str.pressSta ) {//触摸中断里置位pressSta状态
      press_str.pressSta = 0;
      touch_check_x();
      touch_check_y();

      //首页
      if ( page_str.page == PAGE_HOME ) {
        //进入菜单页面
        st7571_lcd_display_menu();//菜单
        page_str.page = PAGE_MENU;
        WAITING_PRESS_OFF(1);//等待松手
        continue;
      }

      //图片显示
      if ( page_str.page == PAGE_IMG ) {
        //进入菜单页面
        st7571_lcd_display_menu();//菜单
        page_str.page = PAGE_MENU;
        WAITING_PRESS_OFF(1);//等待松手
        continue;
      }
      
      //菜单模式
      if ( page_str.page == PAGE_MENU ) {
        if ( press_str.y>31 && press_str.y<=48 ) {
          //首页按钮区域：x:[7,40] y:[31,48]
          if ( press_str.x>7 && press_str.x<=40 ) {
            page_str.page = PAGE_HOME;
            st7571_lcd_display_home();

          //绘画按钮区域：x:[47,80] y:[31,48]
          } else if ( press_str.x>47 && press_str.x<=80 ) {
            page_str.page = PAGE_PAINTING;
            st7571_lcd_display_painting();

          //校准按钮区域：x:[87,120] y:[31,48]
          } else if ( press_str.x>87 && press_str.x<=120 ) {
            page_str.page = PAGE_TOUCH_CALIBRATION;
            st7571_lcd_display_touch_calibration();
          }
          
        //图片按钮区域：x:[47,80] y:[55,64]
        } else if ( press_str.x>47 && press_str.x<=80 && press_str.y>55 && press_str.y<=71 ) {
          page_str.page = PAGE_IMG;
          st7571_lcd_display_img();
        }
        WAITING_PRESS_OFF(1);//等待松手
        continue;
      }

      //校准模式
      if ( page_str.page == PAGE_TOUCH_CALIBRATION ) {
        press_str.calibrationNum++;
        if ( press_str.calibrationNum == 1 ) {
          //获取端点电压
          press_str.uxVcc1 = touch_check_x();
          press_str.uyVcc1 = touch_check_y();
          //清除左上角
          st7571_writeDataToRAM(0, 0, 9, (uint8_t *)graphDot[2], 0);
          st7571_writeDataToRAM(1, 0, 9, (uint8_t *)graphDot[2], 0);
          //绘制左下角圆点
          st7571_writeDataToRAM(11, 0, 9, (uint8_t *)graphDot[0], 0);
          st7571_writeDataToRAM(12, 0, 9, (uint8_t *)graphDot[1], 0);
        } else if ( press_str.calibrationNum == 2 ) {
          //获取端点电压
          press_str.uxVcc2 = touch_check_x();
          press_str.uyGnd1 = touch_check_y();
          //清除左下角圆点
          st7571_writeDataToRAM(11, 0, 9, (uint8_t *)graphDot[2], 0);
          st7571_writeDataToRAM(12, 0, 9, (uint8_t *)graphDot[2], 0);
          //绘制右下角圆点
          st7571_writeDataToRAM(11, 118, 127, (uint8_t *)graphDot[0], 0);
          st7571_writeDataToRAM(12, 118, 127, (uint8_t *)graphDot[1], 0);
        } else if ( press_str.calibrationNum == 3 ) {
          //获取端点电压
          press_str.uxGnd1 = touch_check_x();
          press_str.uyGnd2 = touch_check_y();
          //清除右下角圆点
          st7571_writeDataToRAM(11, 118, 127, (uint8_t *)graphDot[2], 0);
          st7571_writeDataToRAM(12, 118, 127, (uint8_t *)graphDot[2], 0);
          //绘制右上角圆点
          st7571_writeDataToRAM(0, 118, 127, (uint8_t *)graphDot[0], 0);
          st7571_writeDataToRAM(1, 118, 127, (uint8_t *)graphDot[1], 0);
        } else if ( press_str.calibrationNum == 4 ) {
          press_str.calibrationNum = 0;
          //获取端点电压
          press_str.uxGnd2 = touch_check_x();
          press_str.uyVcc2 = touch_check_y();
          //清除右上角圆点
          st7571_writeDataToRAM(0, 118, 127, (uint8_t *)graphDot[2], 0);
          st7571_writeDataToRAM(1, 118, 127, (uint8_t *)graphDot[2], 0);
          if ( my_abs_diff((int32_t)press_str.uxVcc1, (int32_t)press_str.uxVcc2) < 0xff &&
              my_abs_diff((int32_t)press_str.uxGnd1, (int32_t)press_str.uxGnd2) < 0xff &&
              my_abs_diff((int32_t)press_str.uyVcc1, (int32_t)press_str.uyVcc2) < 0xff &&
              my_abs_diff((int32_t)press_str.uyGnd1, (int32_t)press_str.uyGnd2) < 0xff ) {
              //采集两次xVCC，两次xGND，两次yVCC，两次yGND值近似，则校验成功
              udata.uxVcc = (uint16_t)(((uint32_t)press_str.uxVcc1+(uint32_t)press_str.uxVcc2)/2);
              udata.uxGnd = (uint16_t)(((uint32_t)press_str.uxGnd1+(uint32_t)press_str.uxGnd2)/2);
              udata.uyVcc = (uint16_t)(((uint32_t)press_str.uyVcc1+(uint32_t)press_str.uyVcc2)/2);
              udata.uyGnd = (uint16_t)(((uint32_t)press_str.uyGnd1+(uint32_t)press_str.uyGnd1)/2);
              #ifdef USE_UART
              printf("校验成功!\nuxvcc:%04X uxgnd:%04X uyvcc:%04X uygnd:%04X\n",
                  press_str.uxVcc1, press_str.uxGnd1, press_str.uyVcc1, press_str.uyGnd2);
              #endif
              //校验值存入flash
              write_data_into_flash();
              page_str.page = PAGE_MENU;
              st7571_lcd_display_menu();//菜单
            
          } else {//失败重新校验
            #ifdef USE_UART
            printf("校验失败请重新尝试！\n");
            printf("uxVcc1:%04X uxVcc2:%04X uxGnd1:%04X uxGnd2:%04X\nuyVcc1:%04X uyVcc2:%04X uyGnd1:%04X uyGnd2:%04X\n",
              (uint16_t)press_str.uxVcc1, (uint16_t)press_str.uxVcc2, (uint16_t)press_str.uxGnd1, (uint16_t)press_str.uxGnd2,
              (uint16_t)press_str.uyVcc1, (uint16_t)press_str.uyVcc2, (uint16_t)press_str.uyGnd1, (uint16_t)press_str.uyGnd2
            );
            #endif
            //绘制左上角圆点
            st7571_writeDataToRAM(0, 0, 9, (uint8_t *)graphDot[0], 0);
            st7571_writeDataToRAM(1, 0, 9, (uint8_t *)graphDot[1], 0);
          } 
        }
        TOUCH_PRESS_START;/*打开接通电源*/
        HAL_Delay(10);
        while( !PRESS_OFF );/*等待松手*/
        HAL_Delay(200);
        //确认松手
        WAITING_PRESS_OFF(1);
        continue;
      }

      //绘画模式
      if ( page_str.page == PAGE_PAINTING ) {
        //绘画区域：2~12页 8~120列 x:[8,120] y:[16,88]
        if ( press_str.x>=8 && press_str.x<120 && press_str.y>=16 && press_str.y<88 ) {
          //进入绘画模式
          while(1) {
            st7571_painting(press_str.x, press_str.y);
            HAL_Delay(2);
            touch_check_x();
            touch_check_y();
            // printf("坐标：x %d y %d\n", press_str.x, press_str.y );
            TOUCH_PRESS_START;
            for(int i=0; i<0xFFF; i++) {;;}//延时等待电源稳定
            //判断松手跳出
            if ( PRESS_OFF ) break;
          }
        
        //返回区域
        } else if ( press_str.x>=96 && press_str.x<128 && press_str.y>2 && press_str.y<16 ) {
          page_str.page = PAGE_MENU;
          st7571_lcd_display_menu();

        } else if ( press_str.x>=2 && press_str.x<34 && press_str.y>=2 && press_str.y<16 ) {
          st7571_lcd_display_painting_clear();//清屏
        }

        WAITING_PRESS_OFF(1);//等待松手
        continue;
      }
    }
  }
}

//X轴开启GND电源，检测引脚上拉通过100K电阻串接y轴低电平端，按下屏幕后，x轴y轴相连，检测引脚会被拉低触发中断
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if ( GPIO_Pin == TC_Pin ) {
    HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
    press_str.pressSta = 1;//屏幕按下状态，1被按下，while循环中会清0
  }
}

/**
  * @brief  System Clock Configuration
  * @param  None
  * @retval None
  */
static void APP_SystemClockConfig(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI | RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;                          /* Enable HSI */
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;                          /* HSI clock 1 division */
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_8MHz;  /* Config HSI 8MHz Calibration */
  RCC_OscInitStruct.HSEState = RCC_HSE_OFF;                         /* Disable HSE */
  /*RCC_OscInitStruct.HSEFreq = RCC_HSE_16_32MHz;*/
  RCC_OscInitStruct.LSIState = RCC_LSI_OFF;                         /* Disable LSI */
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    APP_ErrorHandler();
  }

  /* Configure Clock */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI; /* System clock selection HSI */
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;     /* AHB clock 1 division */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;      /* APB clock 1 division */
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    APP_ErrorHandler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void APP_ErrorHandler(void)
{
  /* Infinite loop */
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     for example: printf("Wrong parameters value: file %s on line %d\r\n", file, line)  */
  /* Infinite loop */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
