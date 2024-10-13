#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

extern UART_HandleTypeDef huart1;

void UART1_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

