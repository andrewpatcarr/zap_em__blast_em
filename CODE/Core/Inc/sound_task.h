/*
 * sound_task.h
 *
 *  Created on: May 22, 2025
 *      Author: andrewcarr
 */

#ifndef INC_SOUND_TASK_H_
#define INC_SOUND_TASK_H_

#include <stdint.h> // Allows use of standard integer types

typedef struct SoundTask SoundTask;
// function must have no input arguments and have no return
typedef void (*sound_fcn_t)(SoundTask *sound_task);


// A datatype for a structure to hold task configuration and state.
// Additional fields can be added as desired.
struct SoundTask
{
    int32_t     state;
    int32_t     num_states;
    int32_t		laser_snd;
    int32_t		hit_snd;
    int32_t		start_snd;
    int32_t		win_snd;
    sound_fcn_t state_list[];
};

// A prototype for each function implemented in task_1.c
void sound_task_run(SoundTask *sound_task);
void sound_task_state_0_init(SoundTask *sound_task);
void sound_task_state_1_wait(SoundTask *sound_task);
void sound_task_state_2_laser(SoundTask *sound_task);
void sound_task_state_3_hit(SoundTask *sound_task);
void sound_task_state_4_win(SoundTask *sound_task);
void sound_task_state_5_start(SoundTask *sound_task);


#endif /* INC_SOUND_TASK_H_ */
