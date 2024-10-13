#include "tim.h"

TIM_HandleTypeDef htim1;
TIM_OC_InitTypeDef sConfig;


/**
  * @brief  错误执行函数
  * @param  无
  * @retval 无
  */
void Error_Handler(void) {
  /* 无限循环 */
  while (1) { }
}

/**
  * @brief  TIM1初始化函数
  * @param  无
  * @retval 无
  */
void TIM1_Init(void) {

  htim1.Instance = TIM1;/* 选择TIM1 */
  htim1.Init.Period            = 100 - 1;/* 自动重装载值 */
  htim1.Init.Prescaler         = 10 - 1;/* 预分频为10-1 */
  htim1.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;/* 时钟不分频 */
  htim1.Init.CounterMode       = TIM_COUNTERMODE_DOWN;/* 向上计数*/
  // htim1.Init.RepetitionCounter = 20;/* 不重复计数 */
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;/* 自动重装载寄存器没有缓冲 */
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)/* 基础时钟初始化 */                                
  {
    Error_Handler();
  }

  sConfig.OCMode       = TIM_OCMODE_PWM2;/*输出配置为翻转模式 */
  sConfig.OCPolarity   = TIM_OCPOLARITY_LOW;/*OC通道输出高电平有效 */
  sConfig.OCFastMode   = TIM_OCFAST_DISABLE;/*输出快速使能关闭 */
  sConfig.OCNPolarity  = TIM_OCNPOLARITY_HIGH;/*OCN通道输出高电平有效 */
  // sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;/*空闲状态OC1N输出低电平 */
  sConfig.OCIdleState  = TIM_OCIDLESTATE_SET;//TIM_OCIDLESTATE_RESET;/*空闲状态OC1输出低电平*/
  sConfig.Pulse = 99;
  /* 通道1配置 */
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfig, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }

  /* 通道2开始输出PWM */
  if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1) != HAL_OK)                  
  {
    Error_Handler();
  }
}

/**
 * 设置PWM占空比
 * pulse：0~99 数值越大屏幕越亮
 */
void set_pwm_configchannel_2( uint8_t pulse ) {
	sConfig.Pulse = pulse;
	HAL_TIM_PWM_ConfigChannel(&htim1, &sConfig, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	
}
