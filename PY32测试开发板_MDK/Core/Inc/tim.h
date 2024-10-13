#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

extern TIM_HandleTypeDef htim1;

void TIM1_Init(void);
void set_pwm_configchannel_2( uint8_t pulse );
	
#endif /*__TIM_H__*/



