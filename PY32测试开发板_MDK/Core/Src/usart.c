#include "usart.h"

UART_HandleTypeDef huart1;

/**
  * @brief  USART配置函数
  * @param  无
  * @retval 无
  */
void UART1_Init(void) {
  /* USART1初始化 */
  
  huart1.Instance          = USART1;
  huart1.Init.BaudRate     = 115200;
  huart1.Init.WordLength   = UART_WORDLENGTH_8B;
  huart1.Init.StopBits     = UART_STOPBITS_1;
  huart1.Init.Parity       = UART_PARITY_NONE;
  huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  huart1.Init.Mode         = UART_MODE_TX_RX;
  if (HAL_UART_DeInit(&huart1) != HAL_OK)
  {
    while(1);
  }
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    while(1);
  }
}



int fputc(int ch, FILE *f) {	
  HAL_UART_Transmit(&huart1, (uint8_t *) &ch, 1, 0xffff);
	return 0;
}


