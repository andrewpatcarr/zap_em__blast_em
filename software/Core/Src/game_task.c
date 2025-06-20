/**
 * @file game_task.c
 * @brief State machine for Zap'em Blast'em Robots game control logic.
 * @author Andrew Carr and Kai De La Cruz
 * game_task.c
 *
 *  Created on: May 22, 2025
 *
 */
#include "motor_driver.h"
#include "game_task.h"
#include "sound_task.h"
#include "photoresistor_task.h"
#include "stm32f4xx_hal_tim.h"
#include "stm32f4xx_it.h"
#include "lcd.h"

#include <stdio.h>

/**
 * @brief Runs the current state of the GameTask.
 *
 * Checks whether the current state index is valid. If so, it calls the
 * corresponding state function from the task's function pointer array.
 *
 * @param game_task Pointer to the GameTask instance.
 */

void game_task_run(GameTask *game_task)
{
    if (game_task->state >= 0 &&
        game_task->state < game_task->num_states)
    {
        game_task->state_list[game_task->state](game_task);
    }
    else
    {
        while (1) {}
    }
}

/**
 * @brief Initializes the game task (state 0).
 *
 * Clears and sets up the LCD, then advances to state 1.
 *
 * @param game_task Pointer to the GameTask instance.
 */

void game_task_state_0_init(GameTask *game_task)
{
    lcd_init();
    lcd_clear();
    game_task->state = 1;
}
/**
 * @brief Waits for player to initiate the game (state 1).
 *
 * Displays title and instructions. When play_flag is set, transitions
 * to the next state and triggers the start sound.
 *
 * @param game_task Pointer to the GameTask instance.
 */
void game_task_state_1_home(GameTask *game_task)
{
	set_duty(game_task->mred,0);
	set_duty(game_task->mblue,0);
	lcd_set_cursor(0, 0);
	lcd_print("  Zap'em Blast'em   ");
	lcd_set_cursor(1, 0);
	lcd_print("       Robots       ");
	lcd_set_cursor(3, 0);
	lcd_print("HOLD 'SHOOT' TO PLAY");
	if (game_task->play_flag == 1){
    	game_task->state = 2;
    	game_task->sound_task_ptr->start_snd = 1;
    }

}
/**
 * @brief Handles gameplay logic and scoring (state 2).
 *
 * Increments scores based on photoresistor hit flags, updates LCD with
 * score, and transitions to end state when a score threshold is reached.
 *
 * @param game_task Pointer to the GameTask instance.
 */
void game_task_state_2_play(GameTask *game_task)
{
	char r_score[5];
	char b_score[5];


	// score counting and delaying
	if (game_task->red_photoresistor_task_ptr->hit_flag && game_task->red_delay_flag == 0){
		game_task->score_blue++;
		game_task->red_photoresistor_task_ptr->hit_flag = 0;
		game_task->red_delay_flag = 1;
		game_task->red_delay_start = ms_counter;
		game_task->sound_task_ptr->hit_snd = 1;
		game_task->sound_task_ptr->state = 1;
		if(playing){
			playing = 0;
			game_task->sound_task_ptr->state = 6;
		}
	}
	if (game_task->blue_photoresistor_task_ptr->hit_flag && game_task->blue_delay_flag == 0){
		game_task->score_red++;
		game_task->blue_photoresistor_task_ptr->hit_flag = 0;
		game_task->blue_delay_flag = 1;
		game_task->blue_delay_start = ms_counter;
		game_task->sound_task_ptr->hit_snd = 1;
		game_task->sound_task_ptr->state = 1;
		if(playing){
			playing = 0;
			game_task->sound_task_ptr->state = 6;
		}
	}

	if ((ms_counter - game_task->red_delay_start) > game_task->delay && game_task->red_delay_flag)
	{

		game_task->red_delay_flag = 0;
		game_task->red_photoresistor_task_ptr->hit_flag = 0;
	}
	if ((ms_counter - game_task->blue_delay_start) > game_task->delay && game_task->blue_delay_flag)
	{
		game_task->blue_delay_flag = 0;
		game_task->blue_photoresistor_task_ptr->hit_flag = 0;
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
	if (game_task->score_red != game_task->score_red_prev && game_task->score_red < game_task->score_thresh){
		sprintf(r_score,"%ld",game_task->score_red);
		lcd_set_cursor(2, 6);
		lcd_print(r_score);
		game_task->score_red_prev = game_task->score_red;
	}
	if (game_task->score_blue != game_task->score_blue_prev && game_task->score_blue < game_task->score_thresh){
		sprintf(b_score,"%ld",game_task->score_blue);
		lcd_set_cursor(3, 6);
		lcd_print(b_score);
		game_task->score_blue_prev = game_task->score_blue;
	}

	if (game_task->score_red >= game_task->score_thresh || game_task->score_blue >= game_task->score_thresh)
	{
		game_task->state = 3;
		game_task->red_delay_flag = 0;
		game_task->blue_delay_flag = 0;
		game_task->sound_task_ptr->win_snd = 1;

	}
}
/**
 * @brief Displays game over screen and resets (state 3).
 *
 * Shows a winning message based on who reached the score threshold
 * first. After a delay, resets the game state and variables.
 *
 * @param game_task Pointer to the GameTask instance.
 */
void game_task_state_3_end(GameTask *game_task)
{
	char r_wins[50];
	char b_wins[50];

	if (game_task->score_blue >= game_task->score_thresh)
	{
		if(game_task->blue_delay_flag == 0)
		{

			sprintf(b_wins,"       %ld to %ld       ",game_task->score_blue, game_task->score_red);
			lcd_set_cursor(0, 0);
			lcd_print("     GAME OVER!     ");
			lcd_set_cursor(1, 0);
			lcd_print("                    ");
			lcd_set_cursor(2, 0);
			lcd_print("     Blue Wins!!    ");
			lcd_set_cursor(3, 0);
			lcd_print(b_wins);

			game_task->blue_delay_flag = 1;
			game_task->blue_delay_start = ms_counter;
		}
		uint32_t now = ms_counter;
		if ((uint32_t)(now - game_task->blue_delay_start) > game_task->end_delay)
		{
		    game_task->play_flag = 0;
		    game_task->state = 1;
		    game_task->score_blue = 0;
		    game_task->score_red = 0;
		    game_task->num = 0;
		    game_task->blue_delay_flag = 0;
		    lcd_clear();
		}
	}
	if (game_task->score_red >= game_task->score_thresh)
		{
			if(game_task->red_delay_flag == 0)
			{
				sprintf(r_wins,"       %ld to %ld       ",game_task->score_red, game_task->score_blue);
				lcd_set_cursor(0, 0);
				lcd_print("     GAME OVER!     ");
				//         01234567890123456789
				lcd_set_cursor(1, 0);
				lcd_print("                    ");
				lcd_set_cursor(2, 0);
				lcd_print("     Red Wins!!     ");
				lcd_set_cursor(3, 0);
				lcd_print(r_wins);


				game_task->red_delay_flag = 1;
				game_task->red_delay_start = ms_counter;
			}
			uint32_t now = ms_counter;
			if ((uint32_t)(now - game_task->red_delay_start) > game_task->end_delay)
			{
			    game_task->play_flag = 0;
			    game_task->state = 1;
			    game_task->score_blue = 0;
			    game_task->score_red = 0;
			    game_task->num = 0;
			    game_task->red_delay_flag = 0;
			    lcd_clear();
			}
		}
	game_task->red_photoresistor_task_ptr->hit_flag = 0;
	game_task->blue_photoresistor_task_ptr->hit_flag = 0;

}

