/*
 * adc_task.h
 *
 *  Created on: Jun 5, 2025
 *      Author: andrewcarr
 */

#ifndef INC_ADC_TASK_H_
#define INC_ADC_TASK_H_

#include <stdlib.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"
#include <stdint.h> // Allows use of standard integer types

typedef struct ControllerTask ControllerTask;
typedef struct PhotoresistorTask PhotoresistorTask;
typedef struct ADCTask ADCTask; // forward declaration

// all states take in a GameTask pointer
typedef void (*adc_fcn_t)(ADCTask *adc_task);

// A datatype for a structure to hold task configuration and state.
// Additional fields can be added as desired.
struct ADCTask
{
    int32_t     		state;
    int32_t     		num_states;
    ControllerTask		*red_contr_ptr;
    ControllerTask		*blue_contr_ptr;
    PhotoresistorTask	*red_photor_ptr;
    PhotoresistorTask	*blue_photor_ptr;
    ADC_HandleTypeDef 	*hadc;
    adc_fcn_t 			state_list[];
};

// A prototype for each function implemented in task_1.c
void adc_task_state_0_init(ADCTask *adc_task);
void adc_task_run(ADCTask *adc_task);
void adc_task_state_1_read(ADCTask *adc_task);


#endif /* INC_ADC_TASK_H_ */
