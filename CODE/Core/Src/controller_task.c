/*
 * controller_task.c
 *
 *  Created on: Jun 3, 2025
 *      Author: kaidc
 */

#include "motor_driver.h"
#include "controller_task.h"
#include "stm32f4xx_hal_tim.h"
#include <stdio.h>


// A function to run the appropriate state of the task
void controller_task_run(ControllerTask *controller_task)
{    // Check for a valid state
    if (controller_task->state >= 0 && controller_task->state < controller_task->num_states)
    {
        // Index the list of state functions and then call the appropriate
        // method while passing in (this) task_1 object

    	// game_task is a pointer right now
    	// -> dereferences pointer and accesses class member in one step
    	// the same as (*ptr).member

        controller_task->state_list[controller_task->state](controller_task);

    }
    // Big problems if the state is invalid
    else
    {

        while(1){}
    }

}

void read_adc_channels_scan_mode(ADC_HandleTypeDef *hadc, int32_t *adc_val_6, int32_t *adc_val_7)
{
    HAL_ADC_Start(hadc);

    // Wait for first conversion (ADC6)
    if (HAL_ADC_PollForConversion(hadc, 10) == HAL_OK) {
        *adc_val_6 = HAL_ADC_GetValue(hadc);
    }

    // Wait for second conversion (ADC7)
    if (HAL_ADC_PollForConversion(hadc, 10) == HAL_OK) {
        *adc_val_7 = HAL_ADC_GetValue(hadc);
    }

    HAL_ADC_Stop(hadc);

}
// init stuff and zero pot
void controller_task_state_0_init(ControllerTask *controller_task)
{
	controller_task->prev_error = 0;
    HAL_TIM_Encoder_Start(controller_task->htim_encoder, TIM_CHANNEL_ALL);
    enable(controller_task->motor);

    int32_t adc_val_6 = 0, adc_val_7 = 0;
    HAL_Delay(1);
    read_adc_channels_scan_mode(controller_task->hadc, &adc_val_6, &adc_val_7);

    controller_task->pot_zero = (controller_task->color == 0) ? adc_val_7 : adc_val_6;

    controller_task->state = 1;
}
// run state for velocity controller based on pot input
void controller_task_state_1_calc_vel(ControllerTask *controller_task)
{
	int32_t high_thres = controller_task->pot_zero + controller_task->high_deadzone;
	int32_t low_thres = controller_task->pot_zero - controller_task->low_deadzone;

    // read adc
    int32_t adc_val_6 = 0, adc_val_7 = 0;
    HAL_Delay(1);
    read_adc_channels_scan_mode(controller_task->hadc, &adc_val_6, &adc_val_7);

    int32_t adc_val = (controller_task->color == 0) ? adc_val_7 : adc_val_6;

    // calc vel des
    const float MAX_ADC = 4095.0f;
    const float MAX_VELOCITY = 10.0f;

    float desired_velocity = 0.0f;
    if (adc_val > high_thres || adc_val < low_thres){
        adc_val -= controller_task->pot_zero;
        desired_velocity = ((float)adc_val / MAX_ADC) * MAX_VELOCITY;
    } else {
        desired_velocity = 0.0f;
    }

    // read encoder
    int32_t current_ticks = __HAL_TIM_GET_COUNTER(controller_task->htim_encoder);
    int32_t delta_ticks = current_ticks - controller_task->prev_ticks;
    controller_task->prev_ticks = current_ticks;

    controller_task->current_time = __HAL_TIM_GET_COUNTER(controller_task->htim_dt);
    uint32_t dt_ticks = (controller_task->current_time >= controller_task->prev_time) ? // calcs time diff in ticks
    		(controller_task->current_time - controller_task->prev_time) :
			(0xFFFFFFFF - controller_task->prev_time + controller_task->current_time); // overflow
    if (dt_ticks == 0) return;
    float delta_time = dt_ticks / 875.0f; // ms -- could set up so its just microseconds, if this was 1000 instead of 875

    // calc actual vel, error, derivative part
    float current_velocity = (float)delta_ticks / delta_time;
    float error = desired_velocity - current_velocity;
    float derivative = (error - controller_task->prev_error) / delta_time;
    controller_task->prev_error = error;

    // calc control signal and do saturation stuff
    controller_task->control_signal = controller_task->k_p * error + controller_task->k_d * derivative;
    float control_signal = controller_task->control_signal;
    if (control_signal > 100.0f) control_signal = 100.0f;
    else if (control_signal < -100.0f) control_signal = -100.0f;

    // set duty on the motor
    set_duty(controller_task->motor, (int32_t)control_signal);

    // remember the time that was used this time
    controller_task->prev_time = controller_task->current_time;
}
