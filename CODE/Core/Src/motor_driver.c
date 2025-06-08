/*
 * motor_driver.c
 *
 *  Created on: May 29, 2025
 *      Author: Kai De La Cruz
 */
#include "motor_driver.h"
#include "encoder_driver.h"
#include <stdint.h>


#define PWM_MAX 4800

void set_duty(motor_t* p_mot, int32_t duty)
{
    duty = duty*PWM_MAX/100;
	if(duty>=0)
	{
	__HAL_TIM_SET_COMPARE(p_mot->tim, p_mot->chan1, duty);
	__HAL_TIM_SET_COMPARE(p_mot->tim, p_mot->chan2, 0);
    }
	else if(duty<0)
	{
	__HAL_TIM_SET_COMPARE(p_mot->tim, p_mot->chan1, 0);
	__HAL_TIM_SET_COMPARE(p_mot->tim, p_mot->chan2, abs(duty));
	}

}

void disable(motor_t* p_mot)
{
	__HAL_TIM_SET_COMPARE(p_mot->tim, p_mot->chan1, 0);
	__HAL_TIM_SET_COMPARE(p_mot->tim, p_mot->chan2, 0);

}

void enable(motor_t* p_mot)
{
	__HAL_TIM_SET_COMPARE(p_mot->tim, p_mot->chan1, PWM_MAX);
	__HAL_TIM_SET_COMPARE(p_mot->tim, p_mot->chan2, PWM_MAX);
}

void go_to(motor_t *p_mot, int k_p, int where, int from){

	int error = where-from;

	if (error >100){
		error = 99;
	}
	if (error < -100){
		error = -99;
	}

	set_duty(p_mot, k_p*error);



}
