/*
 * shoot_task.c
 *
 *  Created on: Jun 3, 2025
 *      Author: Andrew Carr
 */

#include "shoot_task.h"
#include <stdio.h>

// game plan is to create two tasks, one for red and one for blue

// A function to run the appropriate state of the task
void shoot_task_run(ShootTask *shoot_task)
{    // Check for a valid state
    if (shoot_task->state >= 0 && shoot_task->state < shoot_task->num_states)
    {
        // Index the list of state functions and then call the appropriate
        // method while passing in (this) task_1 object

        shoot_task->state_list[shoot_task->state](shoot_task);

    }
    // Big problems if the state is invalid
    else
    {
        while(1){}
    }

}
// servo -- 300 at shiel

// A function to initialize the task
// init button, laser and photoresistor
void shoot_task_state_0_init(ShootTask *shoot_task)
{
	shoot_task->state = 1;
	HAL_GPIO_WritePin(GPIOB, shoot_task->laser_gpio, GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(shoot_task->servo_tim, shoot_task->channel, shoot_task->unshield_val);
}
// A function to implement state 1 of the task
// wait for button to be pressed which should be on an interrupt that changes a flag
void shoot_task_state_1_wait(ShootTask *shoot_task)
{
	HAL_GPIO_WritePin(GPIOB, shoot_task->laser_gpio, GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(shoot_task->servo_tim, shoot_task->channel, shoot_task->unshield_val);
	if (shoot_task->button == 1){
		shoot_task->state = 2;
	}
}
// A function to implement state 2 which is for when the game is being played
// This keeps track of score, prints score messages
void shoot_task_state_2_unshield(ShootTask *shoot_task)
{
	__HAL_TIM_SET_COMPARE(shoot_task->servo_tim, shoot_task->channel, shoot_task->shield_val);
	if (1){ // add delay using some ticks and tune it
		shoot_task->state = 3;
	}
	if(shoot_task->button == 0){
		shoot_task->state = 1;
	}
}
// A function to implement state 3

void shoot_task_state_3_shoot(ShootTask *shoot_task)
{
	HAL_GPIO_WritePin(GPIOB, shoot_task->laser_gpio, GPIO_PIN_RESET);
	if(shoot_task->button == 0){
		shoot_task->state = 1;
	}

}


