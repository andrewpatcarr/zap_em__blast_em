/*
 * game_task.h
 *
 *  Created on: May 22, 2025
 *      Author: andrewcarr
 */

#ifndef INC_GAME_TASK_H_
#define INC_GAME_TASK_H_

#include <stdint.h> // Allows use of standard integer types
#include "stm32f4xx_hal.h"  // or your STM32 family

typedef struct SoundTask SoundTask; // forward declaration
typedef struct PhotoresistorTask PhotoresistorTask;
typedef struct GameTask GameTask; // forward declaration

// all states take in a GameTask pointer
typedef void (*game_fcn_t)(GameTask *game_task);

// A datatype for a structure to hold task configuration and state.
// Additional fields can be added as desired.
struct GameTask
{
    int32_t     		state;
    int32_t     		num_states;
    int32_t				play_flag;
    int32_t				score_red;
    int32_t				score_blue;
    int32_t 			score_red_prev;
    int32_t 			score_blue_prev;
    int32_t				score_thresh;
    int32_t				num;
    uint32_t			delay_start;
    int32_t				delay_flag;
    uint32_t			delay;
    uint32_t			end_delay;
    TIM_HandleTypeDef 	*htim;
    SoundTask			*sound_task_ptr;
    PhotoresistorTask 	*red_photoresistor_task_ptr;
    PhotoresistorTask 	*blue_photoresistor_task_ptr;
    I2C_HandleTypeDef 	*i2c_handle;
    game_fcn_t 			state_list[];
};

// A prototype for each function implemented in task_1.c
void game_task_state_0_init(GameTask *game_task);
void game_task_run(GameTask *game_task);
void game_task_state_1_home(GameTask *game_task);
void game_task_state_2_play(GameTask *game_task);
void game_task_state_3_end(GameTask *game_task);

#endif /* INC_GAME_TASK_H_ */
