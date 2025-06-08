/*
 * shoot_task.h
 *
 *  Created on: Jun 3, 2025
 *      Author: Andrew Carr
 */

#ifndef INC_SHOOT_TASK_H_
#define INC_SHOOT_TASK_H_

#include <stdint.h> // Allows use of standard integer types
#include "stm32f4xx_hal.h"  // or your STM32 family

typedef struct ShootTask ShootTask; // forward declaration
// all states take in a GameTask pointer
typedef void (*shoot_fcn_t)(ShootTask *shoot_task);

// A datatype for a structure to hold task configuration and state.
// Additional fields can be added as desired.
struct ShootTask
{
    int32_t     		state;
    int32_t     		num_states;
    int32_t			   	button;
    TIM_HandleTypeDef 	*servo_tim;
    uint32_t 			channel;
    int32_t				shield_val;
    int32_t				unshield_val;
    uint32_t			laser_gpio;
    shoot_fcn_t 		state_list[];
};

// A prototype for each function implemented in task_1.c
void shoot_task_state_0_init(ShootTask *shoot_task);
void shoot_task_run(ShootTask *shoot_task);
void shoot_task_state_1_wait(ShootTask *shoot_task);
void shoot_task_state_2_unshield(ShootTask *shoot_task);
void shoot_task_state_3_shoot(ShootTask *shoot_task);

#endif /* INC_SHOOT_TASK_H_ */
