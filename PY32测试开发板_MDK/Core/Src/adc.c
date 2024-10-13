#include "adc.h"

ADC_HandleTypeDef hadc;

/**
  * @brief  系统时钟配置函数
  * @param  无
  * @retval 无
  */
void ADC_Init(void) {
  ADC_ChannelConfTypeDef sConfig = {0};

  __HAL_RCC_ADC_FORCE_RESET();
  __HAL_RCC_ADC_RELEASE_RESET();
  __HAL_RCC_ADC_CLK_ENABLE();                                                /* 使能ADC时钟 */

  hadc.Instance = ADC1;
  if (HAL_ADCEx_Calibration_Start(&hadc) != HAL_OK)                          /* ADC校准 */
  {
    while(1);
  }
 
  hadc.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;                /* 设置ADC时钟*/
  hadc.Init.Resolution            = ADC_RESOLUTION_12B;                      /* 转换分辨率12bit*/
  hadc.Init.DataAlign             = ADC_DATAALIGN_RIGHT;                     /* 数据右对齐 */
  hadc.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;              /* 扫描序列方向：向上(从通道0到通道11)*/
  hadc.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;                     /* ADC_EOC_SINGLE_CONV:单次采样 ; ADC_EOC_SEQ_CONV:序列采样*/
  hadc.Init.LowPowerAutoWait      = ENABLE;                                  /* ENABLE=读取ADC值后,开始下一次转换 ; DISABLE=直接转换 */
  hadc.Init.ContinuousConvMode    = DISABLE;                                 /* 单次转换模式 */
  hadc.Init.DiscontinuousConvMode = DISABLE;                                 /* 不使能非连续模式 */
  hadc.Init.ExternalTrigConv      = ADC_SOFTWARE_START;                      /* 软件触发 */
  hadc.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;           /* 触发边沿无 */
  hadc.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;                /* 当过载发生时，覆盖上一个值 */
  hadc.Init.SamplingTimeCommon    = ADC_SAMPLETIME_41CYCLES_5;               /* 通道采样时间为41.5ADC时钟周期 */
  if (HAL_ADC_Init(&hadc) != HAL_OK)                                         /* ADC初始化*/
  {
    while(1);
  }

  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;                             /* 设置是否排行, 想设置单通道采样,需配置ADC_RANK_NONE */
  sConfig.Channel      = ADC_CHANNEL_0;                                       /* 设置采样通道 */
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)                       /* 配置ADC通道 */
  {
    while(1);
  }

  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;                             /* 设置是否排行, 想设置单通道采样,需配置ADC_RANK_NONE */
  sConfig.Channel      = ADC_CHANNEL_1;                                       /* 设置采样通道 */
  if (HAL_ADC_ConfigChannel(&hadc, &sConfig) != HAL_OK)                       /* 配置ADC通道 */
  {
    while(1);
  }



}
