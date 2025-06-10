/*
 * sound_task.c
 *
 *  Created on: May 22, 2025
 *      Author: andrewcarr
 */

#include "laser_buf.h"
#include "game_over_buf.h"
#include "start_buf.h"
#include "hit_buf.h"
#include "sound_task.h"
#include <stdio.h>

volatile uint32_t sample_index   = 0;
volatile uint8_t  pwm_div_count  = 0;
volatile uint32_t audio_buf_len;
volatile uint32_t playing = 0;
const int16_t *audio_buf_ptr;

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
	sound_task->state = 1;

}

void sound_task_state_1_wait(SoundTask *sound_task)
{

	if(sound_task->hit_snd == 1){
		sound_task->state = 3;
	}
	else if(sound_task->win_snd == 1){
		sound_task->state = 4;
	}
	else if(sound_task->start_snd == 1){
		sound_task->state = 5;
	}
	else if (sound_task->laser_snd == 1){
		sound_task->state = 2;
	}

}
void sound_task_state_2_laser(SoundTask *sound_task)
{
	if (playing) return;
	if (sound_task->laser_snd == 1){
		audio_buf_ptr = laser_buf;
		audio_buf_len = laser_buf_len;
		playing = 1;
		sample_index = 0;
		__HAL_TIM_SET_COMPARE(sound_task->htim, sound_task->chan, 0);  // reset PWM duty
		__HAL_TIM_CLEAR_FLAG(sound_task->htim, TIM_FLAG_UPDATE);
		__HAL_TIM_ENABLE_IT(sound_task->htim, TIM_IT_UPDATE);      // enable update IRQ
		HAL_TIM_PWM_Start(sound_task->htim, sound_task->chan);
		sound_task->laser_snd = 0;
	}

	if (playing == 0){
		sound_task->state = 6;
	}

}
void sound_task_state_3_hit(SoundTask *sound_task)
{
	if (playing) {
		sound_task->laser_snd = 0;
		return;
	}
	if (sound_task->hit_snd == 1){
		audio_buf_ptr = hit_buf;
		audio_buf_len = hit_buf_len;
		playing = 1;
		sample_index = 0;
		__HAL_TIM_SET_COMPARE(sound_task->htim, sound_task->chan, 0);  // reset PWM duty
		__HAL_TIM_CLEAR_FLAG(sound_task->htim, TIM_FLAG_UPDATE);
		__HAL_TIM_ENABLE_IT(sound_task->htim, TIM_IT_UPDATE);      // enable update IRQ
		HAL_TIM_PWM_Start(sound_task->htim, sound_task->chan);
		sound_task->hit_snd = 0;
	}

	if (playing == 0){
		sound_task->state = 6;
	}

}
void sound_task_state_4_win(SoundTask *sound_task)
{
	if (playing){
		sound_task->laser_snd = 0;
		sound_task->hit_snd = 0;
		return;
	}
	if (sound_task->win_snd == 1){
		audio_buf_ptr = game_over_buf;
		audio_buf_len = game_over_buf_len;
		playing = 1;
		sample_index = 0;
		__HAL_TIM_SET_COMPARE(sound_task->htim, sound_task->chan, 0);  // reset PWM duty
		__HAL_TIM_CLEAR_FLAG(sound_task->htim, TIM_FLAG_UPDATE);
		__HAL_TIM_ENABLE_IT(sound_task->htim, TIM_IT_UPDATE);      // enable update IRQ
		HAL_TIM_PWM_Start(sound_task->htim, sound_task->chan);
		sound_task->win_snd = 0;
	}

	if (playing == 0){
		sound_task->state = 6;
	}
}

void sound_task_state_5_start(SoundTask *sound_task)
{
	if (playing) {
		sound_task->laser_snd = 0;
		return;
	}
	if (sound_task->start_snd == 1){
		audio_buf_ptr = start_buf;
		audio_buf_len = start_buf_len;
		playing = 1;
		sample_index = 0;
		__HAL_TIM_SET_COMPARE(sound_task->htim, sound_task->chan, 0);  // reset PWM duty
		__HAL_TIM_CLEAR_FLAG(sound_task->htim, TIM_FLAG_UPDATE);
		__HAL_TIM_ENABLE_IT(sound_task->htim, TIM_IT_UPDATE);      // enable update IRQ
		HAL_TIM_PWM_Start(sound_task->htim, sound_task->chan);
		sound_task->start_snd = 0;
	}

	if (playing == 0){
		sound_task->state = 6;
	}

}
void sound_task_state_6_stop(SoundTask *sound_task){
	__HAL_TIM_DISABLE_IT(sound_task->htim, TIM_IT_UPDATE);     // turn off update IRQ
	HAL_TIM_PWM_Stop(sound_task->htim, sound_task->chan);
	sound_task->state = 1;
}
