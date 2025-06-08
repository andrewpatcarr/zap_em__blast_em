/*
 * photoresistor_task.c
 *
 *  Created on: Jun 3, 2025
 *      Author: Andrew Carr
 */

#include "photoresistor_task.h"
#include <stdio.h>

// game plan is to create two tasks, one for red and one for blue

// A function to run the appropriate state of the task
void photoresistor_task_run(PhotoresistorTask *photoresistor_task)
{    // Check for a valid state
    if (photoresistor_task->state >= 0 && photoresistor_task->state < photoresistor_task->num_states)
    {
        // Index the list of state functions and then call the appropriate
        // method while passing in (this) task_1 object

    	photoresistor_task->state_list[photoresistor_task->state](photoresistor_task);

    }
    // Big problems if the state is invalid
    else
    {
        while(1){}
    }

}


// A function to initialize the task
// init button, laser and photoresistor
void photoresistor_task_state_0_init(PhotoresistorTask *photoresistor_task)
{
	photoresistor_task->state = 1;
}
// A function to implement state 1 of the task
// wait for button to be pressed which should be on an interrupt that changes a flag
void photoresistor_task_state_1_look(PhotoresistorTask *photoresistor_task)
{
	if ((photoresistor_task->adc_val - photoresistor_task->zero) > photoresistor_task->thresh){
		photoresistor_task->hit_flag = 1;
		photoresistor_task->state = 2;
	}

}
// A function to implement state 2 which is for when the game is being played
// This keeps track of score, prints score messages
void photoresistor_task_state_2_hit(PhotoresistorTask *photoresistor_task)
{
	if (photoresistor_task->hit_flag == 0){ // game_task will change when they have recorded the score
		photoresistor_task->state = 1;
	}
}

