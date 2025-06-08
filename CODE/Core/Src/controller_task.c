/*
 * controller_task.c
 *
 *  Created on: Jun 3, 2025
 *      Author: kaidc
 */

#include "motor_driver.h"
#include "controller_task.h"
#include "stm32f4xx_hal_tim.h"
#include "encoder_driver.h"
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


// init stuff and zero pot
void controller_task_state_0_init(ControllerTask *controller_task)
{
	controller_task->prev_error = 0;

    enable(controller_task->motor);

    controller_task->pot_zero = controller_task->adc_val;

    controller_task->state = 2;
}
// run state for velocity controller based on pot input
void controller_task_state_1_calc_vel(ControllerTask *controller_task)
{
	int32_t high_thres = controller_task->pot_zero + controller_task->ccw_deadzone;
	int32_t low_thres = controller_task->pot_zero - controller_task->cw_deadzone;

    // read adc

    // calc vel des
    const float MAX_ADC = 4095.0f;
    const float MAX_VELOCITY = 8.0f;

    int32_t adc_val_here = controller_task->adc_val;



    float desired_velocity = 0.0f;
    if (adc_val_here > high_thres){ //&& (adc_val_here+high_thres)<4095
    	adc_val_here += high_thres;
    	adc_val_here -= controller_task->pot_zero;
        desired_velocity = ((float)adc_val_here / MAX_ADC) * MAX_VELOCITY;
    }
    else if (adc_val_here < low_thres){ // && (adc_val_here-low_thres)>0
    	adc_val_here -= low_thres;
    	adc_val_here -= controller_task->pot_zero;
		desired_velocity = ((float)adc_val_here / MAX_ADC) * MAX_VELOCITY;
    }
    else {
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
    float delta_time = dt_ticks / 1000.0f; // ms

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
void controller_task_state_2_pos(ControllerTask *controller_task){

	int32_t des_pos = controller_task->adc_val - controller_task->pot_zero;

	read_encoder(controller_task->encoder);
	go_to(controller_task->motor, controller_task->k_p, des_pos, controller_task->encoder->ticks);



}


