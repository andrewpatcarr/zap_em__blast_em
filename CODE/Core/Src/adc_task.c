/*
 * adc_task.c
 *
 *  Created on: Jun 5, 2025
 *      Author: andrewcarr
 */

#include "adc_task.h"
#include "stm32f4xx_hal_tim.h"
#include <stdio.h>
#include "controller_task.h"
#include "photoresistor_task.h"

static uint16_t dma_buffer[4];

// A function to run the appropriate state of the task
void adc_task_run(ADCTask *adc_task)
{    // Check for a valid state
    if (adc_task->state >= 0 && adc_task->state < adc_task->num_states)
    {
        // Index the list of state functions and then call the appropriate
        // method while passing in (this) task_1 object

    	// game_task is a pointer right now
    	// -> dereferences pointer and accesses class member in one step
    	// the same as (*ptr).member

    	adc_task->state_list[adc_task->state](adc_task);

    }
    // Big problems if the state is invalid
    else
    {

        while(1){}
    }

}

void adc_task_state_0_init(ADCTask *adc_task){

	adc_task->state = 1;
	HAL_ADC_Start_DMA(adc_task->hadc,
	                      (uint32_t*)dma_buffer,
	                      4);
}

void adc_task_state_1_read(ADCTask *adc_task){
//	HAL_ADC_Start(adc_task->hadc);
//
//	// Wait for first conversion (ADC4)
//	if (HAL_ADC_PollForConversion(adc_task->hadc, 10) == HAL_OK) {
//		adc_task->red_photor_ptr->adc_val = HAL_ADC_GetValue(adc_task->hadc); //
//	}
//
//	// Wait for second conversion (ADC5)
//	if (HAL_ADC_PollForConversion(adc_task->hadc, 10) == HAL_OK) {
//		adc_task->blue_photor_ptr->adc_val = HAL_ADC_GetValue(adc_task->hadc);
//	}
//	// Wait for first conversion (ADC6)
//	if (HAL_ADC_PollForConversion(adc_task->hadc, 10) == HAL_OK) {
//		adc_task->blue_contr_ptr->adc_val = HAL_ADC_GetValue(adc_task->hadc); // 6
//	}
//
//	// Wait for second conversion (ADC7)
//	if (HAL_ADC_PollForConversion(adc_task->hadc, 10) == HAL_OK) {
//		adc_task->red_contr_ptr->adc_val = HAL_ADC_GetValue(adc_task->hadc);
//	}
//
//
//
//	HAL_ADC_Stop(adc_task->hadc);
	adc_task->blue_photor_ptr->adc_val   = dma_buffer[0];
	adc_task->red_photor_ptr->adc_val  = dma_buffer[1];
	adc_task->blue_contr_ptr->adc_val   = dma_buffer[2];
	adc_task->red_contr_ptr->adc_val    = dma_buffer[3];


}
