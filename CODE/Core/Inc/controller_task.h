/*
 * controller_task.h
 *
 *  Created on: Jun 3, 2025
 *      Author: kaidc
 */

#ifndef INC_CONTROLLER_TASK_H_
#define INC_CONTROLLER_TASK_H_

#include "motor_driver.h"
#include <stdlib.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include "encoder_driver.h"
#include <stdint.h> // Allows use of standard integer types

typedef struct ControllerTask ControllerTask; // forward declaration
// all states take in a GameTask pointer
typedef void (*controller_fcn_t)(ControllerTask *controller_task);

struct ControllerTask
{
	int32_t				color;
    int32_t 			state;
    int32_t 			num_states;
    uint32_t 			chan1;
    uint32_t 			chan2;
    int32_t				pot_zero;
    float 				control_signal;
    uint32_t			prev_time;
    uint32_t			current_time;
    uint32_t			prev_ticks;
    float				k_p;
    float				k_d;
    int32_t				cw_deadzone;
    int32_t				ccw_deadzone;
    uint32_t			adc_val;
    uint32_t			encoder_range;
    TIM_HandleTypeDef 	*htim_encoder;
    TIM_HandleTypeDef 	*htim_dt;  		// for encoder feedback delta time
    ADC_HandleTypeDef 	*hadc;          // ADC handle pointer to potentiometer
    motor_t 			*motor;         // Pointer to  motor struct
    encoder_t			*encoder;

    // PID control variables
    float prev_error; //could be dangerous

    controller_fcn_t state_list[];
};


// A prototype for each function implemented in controller_task.c
void controller_task_state_0_init(ControllerTask *controller_task);
void controller_task_run(ControllerTask *controller_task);
void controller_task_state_1_calc_vel(ControllerTask *controller_task);
void controller_task_state_2_pos(ControllerTask *controller_task);

#endif /* INC_CONTROLLER_TASK_H_ */

