/*
 * photoresistor_task.h
 *
 *  Created on: Jun 3, 2025
 *      Author: Andrew Carr
 */

#ifndef INC_PHOTORESISTOR_TASK_H_
#define INC_PHOTORESISTOR_TASK_H_

#include <stdint.h> // Allows use of standard integer types
#include "stm32f4xx_hal.h"  // or your STM32 family

typedef struct PhotoresistorTask PhotoresistorTask; // forward declaration
typedef struct GameTask GameTask;
// all states take in a GameTask pointer
typedef void (*photoresistor_fcn_t)(PhotoresistorTask *photoresistor_task);

// A datatype for a structure to hold task configuration and state.
// Additional fields can be added as desired.
struct PhotoresistorTask
{
    int32_t     			state;
    int32_t     			num_states;
    int32_t					hit_flag;
    int32_t					adc_val;
    int32_t					thresh;
    int32_t					zero;
    photoresistor_fcn_t 	state_list[];
};

// A prototype for each function implemented in task_1.c
void photoresistor_task_state_0_init(PhotoresistorTask *photoresistor_task);
void photoresistor_task_run(PhotoresistorTask *photoresistor_task);
void photoresistor_task_state_1_look(PhotoresistorTask *photoresistor_task);
void photoresistor_task_state_2_hit(PhotoresistorTask *photoresistor_task);


#endif /* INC_PHOTORESISTOR_TASK_H_ */
