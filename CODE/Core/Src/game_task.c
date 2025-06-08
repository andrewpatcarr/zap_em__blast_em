/*
 * game_task.c
 *
 *  Created on: May 22, 2025
 *      Author: andrewcarr and Kai De La Cruz
 */

#include "game_task.h"
#include "sound_task.h"
#include "photoresistor_task.h"
#include "stm32f4xx_hal_tim.h"
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
	lcd_clear();
	game_task->state = 1;


}
// A function to implement state 1 of the task
// keeps for user start sequence to happen then starts the game
// Prints message and sets sound flag when game starts
void game_task_state_1_home(GameTask *game_task)
{
	lcd_set_cursor(0, 0);
	lcd_print("  Zap'em Blast'em   ");
	lcd_set_cursor(1, 0);
	lcd_print("       Robots       ");
	lcd_set_cursor(3, 0);
	lcd_print("HOLD 'SHOOT' TO PLAY");
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


	// score counting and delaying
	if (game_task->red_photoresistor_task_ptr->hit_flag && game_task->delay_flag == 0){
		game_task->score_blue++;
		game_task->red_photoresistor_task_ptr->hit_flag = 0;
		game_task->delay_flag = 1;
		game_task->delay_start = __HAL_TIM_GET_COUNTER(game_task->htim);
	}
	if (game_task->blue_photoresistor_task_ptr->hit_flag && game_task->delay_flag == 0){
		game_task->score_red++;
		game_task->blue_photoresistor_task_ptr->hit_flag = 0;
		game_task->delay_flag = 1;
		game_task->delay_start = __HAL_TIM_GET_COUNTER(game_task->htim);
	}

	if ((__HAL_TIM_GET_COUNTER(game_task->htim) - game_task->delay_start) > game_task->delay)
	{
		game_task->delay_flag = 0;
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

	if (game_task->score_red >= game_task->score_thresh && game_task->state != 3)
			{
		//		// print win message and set end sound
				if(game_task->delay_flag == 0)
				{
				lcd_set_cursor(0, 0);
				lcd_print("                    ");
				lcd_set_cursor(1, 0);
				lcd_print("     GAME OVER!     ");
				lcd_set_cursor(2, 0);
				lcd_print("     Red Wins!!     ");
				lcd_set_cursor(3, 0);
				lcd_print("                    ");
				game_task->delay_flag = 1;
				game_task->delay_start = __HAL_TIM_GET_COUNTER(game_task->htim);
				}
				if ((__HAL_TIM_GET_COUNTER(game_task->htim) - game_task->delay_start) > game_task->end_delay)
				{
				game_task->state = 3;
				}
			}

	if (game_task->score_blue >= game_task->score_thresh && game_task->state != 3)
		{
	//		// print win message and set end sound
			if(game_task->delay_flag == 0)
			{
			lcd_set_cursor(0, 0);
			lcd_print("                    ");
			lcd_set_cursor(1, 0);
			lcd_print("     GAME OVER!     ");
			lcd_set_cursor(2, 0);
			lcd_print("     Blue Wins!!     ");
			lcd_set_cursor(3, 0);
			lcd_print("                    ");
			game_task->delay_flag = 1;
			game_task->delay_start = __HAL_TIM_GET_COUNTER(game_task->htim);
			}
			if ((__HAL_TIM_GET_COUNTER(game_task->htim) - game_task->delay_start) > game_task->end_delay)
			{
			game_task->state = 3;
			}
		}
}
// A function to implement state 3
// Ends the game when the score threshold has been met
// Prints messages and sets sound flags
void game_task_state_3_end(GameTask *game_task)
{

	game_task->play_flag = 0;
	game_task->state = 1;
	game_task->score_blue = 0;
	game_task->score_red = 0;
	game_task->num = 0;
	lcd_clear();

}

