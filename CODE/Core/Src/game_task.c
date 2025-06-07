/*
 * game_task.c
 *
 *  Created on: May 22, 2025
 *      Author: andrewcarr
 */

#include "game_task.h"
#include "sound_task.h"
#include "photoresistor_task.h"
#include "lcd.h"

#include <stdio.h>

// A function to run the appropriate state of the task
void game_task_run(GameTask *game_task)
{    // Check for a valid state
    if (game_task->state >= 0 && game_task->state < game_task->num_states)
    {
        // Index the list of state functions and then call the appropriate
        // method while passing in (this) task_1 object

    	// game_task is a pointer right now
    	// -> dereferences pointer and accesses class member in one step
    	// the same as (*ptr).member

        game_task->state_list[game_task->state](game_task);

    }
    // Big problems if the state is invalid
    else
    {

        while(1){}
    }

}


// A function to initialize the task
void game_task_state_0_init(GameTask *game_task)
{
    //add init stuff, display
	lcd_init();
	HAL_Delay(100);
	lcd_clear();
	game_task->state = 1;

}
// A function to implement state 1 of the task
// keeps for user start sequence to happen then starts the game
// Prints message and sets sound flag when game starts
void game_task_state_1_home(GameTask *game_task)
{
	lcd_clear();
	lcd_set_cursor(1, 0);
	lcd_print("  Zap'em Blast'em   ");
	lcd_set_cursor(2, 0);
	lcd_print("       Robots       ");
    //play_flg enabled from button task within shoot task?? or make button task
	if (game_task->play_flag == 1){
    	game_task->state = 2;
    	game_task->sound_task_ptr->start_snd = 1; // sets start sound flag for sound task to play it
    }

}
// A function to implement state 2 which is for when the game is being played
// This keeps track of score, prints score messages
void game_task_state_2_play(GameTask *game_task)
{
	char r_score[5];
	char b_score[5];

	if (game_task->red_photoresistor_task_ptr->hit_flag){
		game_task->score_red++;
		game_task->red_photoresistor_task_ptr->hit_flag = 0;
		//might need to add delay so that won't get extra hits for extended period of hitting
	}
	if (game_task->blue_photoresistor_task_ptr->hit_flag){
		game_task->score_blue++;
		game_task->blue_photoresistor_task_ptr->hit_flag = 0;
		//might need to add delay so that won't get extra hits for extended period of hitting
	}

	// add thing that prints score of each on the LCD
	//maybe only do once then adjust the score through a direct print index
	if (game_task->num == 0){
		lcd_clear();
		lcd_set_cursor(0, 0);
		lcd_print("  Zap'em Blast'em   ");
		lcd_set_cursor(1, 0);
		lcd_print("     First to 5     ");
		lcd_set_cursor(2, 0);
		lcd_print("Red:  0  Zaps       ");
		lcd_set_cursor(3, 0);
		lcd_print("Blue: 0  Blasts     ");
		//         01234567890123456789
		game_task->num++;
	}

	// check to see if score changed for the lcd
	if (game_task->score_red != game_task->score_red_prev){
		sprintf(r_score,"%ld",game_task->score_red);
		lcd_set_cursor(2, 6);
		lcd_print(r_score);
		game_task->score_red_prev = game_task->score_red;
	}
	if (game_task->score_blue != game_task->score_blue_prev){
		sprintf(b_score,"%ld",game_task->score_blue);
		lcd_set_cursor(3, 6);
		lcd_print(b_score);
		game_task->score_blue_prev = game_task->score_blue;
	}


	// check if someone won
	if (game_task->score_red >= game_task->score_thresh || game_task->score_blue >= game_task->score_thresh){
//		// print win message and set end sound
		lcd_clear();
		lcd_set_cursor(1, 0);
		lcd_print("  Zap'em Blast'em   ");
		lcd_set_cursor(2, 0);
		lcd_print("     GAME OVER!     ");
		game_task->state = 3;
	}

}

void game_task_state_3_end(GameTask *game_task)
{
    uint32_t start_time = 0;
    uint8_t timer_started = 0;

    uint32_t current_time = __HAL_TIM_GET_COUNTER(game_task->htim_dt);

    if (!timer_started)
    {
        start_time = current_time;
        timer_started = 1;

        // Reset game variables
        game_task->play_flag = 0;
        game_task->state = 1;
        game_task->score_blue = 0;
        game_task->score_red = 0;
    }

    uint32_t elapsed_ticks = current_time - start_time;  // safe for 32-bit wrap-around

    const uint32_t delay_ticks = 420000000;  // 5 seconds at 84 MHz timer clock

    if (elapsed_ticks >= delay_ticks)
    {
        timer_started = 0; // reset for next call

        // Delay done, proceed with next steps
        // e.g., game_task->state = NEXT_STATE;
    }
}
