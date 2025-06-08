/*
 * encoder_task.c
 *
 *  Created on: Jun 7, 2025
 *      Author: andrewcarr and Kai De La Cruz
 */



#include "encoder_driver.h"
#include <stdio.h>
#include "stm32f4xx_hal.h"


void setup_encoder(encoder_t *p_enc){
    p_enc->zero = __HAL_TIM_GET_COUNTER(p_enc->htim);
    p_enc->ticks = 0;
    p_enc->last_ticks = 0;
}
void read_encoder(encoder_t *p_enc){

	int32_t current_ticks = (int32_t)(__HAL_TIM_GET_COUNTER(p_enc->htim)) - (int32_t)(p_enc->zero);
	int32_t delta = current_ticks - p_enc->last_ticks;

	if (delta > (p_enc->ar + 1)/2){
		delta -= p_enc->ar + 1;
	}
	else if (delta < (-p_enc->ar-1)/2){
		delta += p_enc->ar + 1;
	}

	p_enc->last_ticks = current_ticks;
	p_enc->ticks += delta;


}
