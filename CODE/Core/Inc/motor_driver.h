/*
 * motor_driver.h
 *
 *  Created on: May 29, 2025
 *      Author: Kai De La Cruz
 */

#ifndef INC_MOTOR_DRIVER_H_
#define INC_MOTOR_DRIVER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"


// motor object data structure
typedef struct {
	TIM_HandleTypeDef *tim;
	uint32_t chan1;
	uint32_t chan2;
} motor_t;


// prototype for motor object 'method'
void set_duty(motor_t *p_mot, int32_t duty);
void enable(motor_t *p_mot);
void disable(motor_t *p_mot);
void go_to(motor_t *p_mot, int k_p, int where, int from);

#endif /* INC_MOTOR_DRIVER_H_ */
