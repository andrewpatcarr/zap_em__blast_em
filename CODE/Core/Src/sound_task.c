/*
 * sound_task.c
 *
 *  Created on: May 22, 2025
 *      Author: andrewcarr
 */

#include "sound_task.h"

// A function to run the appropriate state of the task
void sound_task_run(SoundTask *sound_task)
{    // Check for a valid state
    if (sound_task->state >= 0 && sound_task->state < sound_task->num_states)
    {
        // Index the list of state functions and then call the appropriate
        // method while passing in (this) task_1 object
    	sound_task->state_list[sound_task->state](sound_task);

    }
    // Big problems if the state is invalid
    else
    {

        while(1){}
    }
}


// A function to initialize the task
void sound_task_state_0_init(SoundTask *sound_task)
{
    //add init stuff

}

void sound_task_state_1_wait(SoundTask *sound_task)
{
	if (sound_task->laser_snd == 1){
		sound_task->state = 2;
	}
	else if(sound_task->hit_snd == 1){
		sound_task->state = 3;
	}
	else if(sound_task->win_snd == 1){
		sound_task->state = 4;
	}
	else if(sound_task->start_snd == 1){
		sound_task->state = 5;
	}

}
void sound_task_state_2_laser(SoundTask *sound_task)
{
    // add sound with associated time (nonblocking, like HAL_Get_Ticks or something like that)
	// if sound done then
	// laser_snd_flg = 0;
	// sound_task.state = 1;

}
void sound_task_state_3_hit(SoundTask *sound_task)
{


}
void sound_task_state_4_win(SoundTask *sound_task)
{


}

void sound_task_state_5_start(SoundTask *sound_task)
{


}
