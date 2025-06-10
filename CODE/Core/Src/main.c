/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "controller_task.h"
#include "motor_driver.h"
#include "game_task.h"
#include "sound_task.h"
#include "shoot_task.h"
#include "lcd.h"
#include "motor_driver.h"
#include "photoresistor_task.h"
#include "adc_task.h"
#include "encoder_driver.h"

#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;
TIM_HandleTypeDef htim9;

/* USER CODE BEGIN PV */
uint8_t red_held = 0, blue_held = 0;
uint32_t red_start = 0, blue_start = 0;
uint32_t red_elapsed = 0, blue_elapsed = 0;
const uint32_t hold_time = 2000000; // 2s @ 1MHz
void calibration(void);
void wait_for_stall(encoder_t *encoder);
// create structs here so that variable pointers can be passed around
motor_t mred = {
		.tim = &htim1,
		.chan1 = TIM_CHANNEL_3,
		.chan2 = TIM_CHANNEL_4
};
motor_t mblue = {
		.tim = &htim1,
		.chan1 = TIM_CHANNEL_1,
		.chan2 = TIM_CHANNEL_2
};
encoder_t red_encoder = {.zero = 0,
						 .ar = 0xFFFFFFFF,
						 .ticks = 0,
						 .last_ticks = 0,
						 .range = 1300,
						 .htim = &htim5

};
encoder_t blue_encoder = {.zero = 0,
						  .ar = 0xFFFF,
						  .ticks = 0,
						  .last_ticks = 0,
						  .range = 1300,
						  .htim = &htim3
};
// sound task will be passed into other tasks so needs to be declared first
SoundTask sound_task = {.state = 0,
                      .num_states = 7,
					  .laser_snd = 0,
					  .hit_snd = 0,
					  .start_snd = 0,
					  .win_snd = 0,
					  .htim = &htim2,
					  .chan = TIM_CHANNEL_1,
                      .state_list = {&sound_task_state_0_init,
									 &sound_task_state_1_wait,
									 &sound_task_state_2_laser,
									 &sound_task_state_3_hit,
									 &sound_task_state_4_win,
									 &sound_task_state_5_start,
									 &sound_task_state_6_stop}

};
PhotoresistorTask red_photoresistor_task = {.state = 0,
											.num_states = 3,
											.hit_flag = 0,
											.adc_val = 0,
											.thresh = 2000,
											.zero = 0,
											.state_list = {&photoresistor_task_state_0_init,
													       &photoresistor_task_state_1_look,
														   &photoresistor_task_state_2_hit}
};
PhotoresistorTask blue_photoresistor_task = {.state = 0,
											.num_states = 3,
											.hit_flag = 0,
											.adc_val = 0,
											.thresh = 2500,
											.zero = 0,
											.state_list = {&photoresistor_task_state_0_init,
													       &photoresistor_task_state_1_look,
														   &photoresistor_task_state_2_hit}
};
GameTask game_task = {.state = 0,
                      .num_states = 4,
					  .play_flag = 0,
					  .score_red = 0,
					  .score_blue = 0,
					  .score_red_prev = 0,
					  .score_blue_prev = 0,
					  .score_thresh = 5,
					  .num = 0,
					  .delay_start = 0,
					  .delay_flag = 0,
					  .delay = 2000000, 	// 2 second delay
					  .end_delay = 5000000,	// 3 second delay
					  .htim = &htim9,
					  .sound_task_ptr = &sound_task,
					  .red_photoresistor_task_ptr = &red_photoresistor_task,
					  .blue_photoresistor_task_ptr = &blue_photoresistor_task,
					  .mred = &mred,
					  .mblue = &mblue,
					  .i2c_handle = &hi2c1,
                      .state_list = {&game_task_state_0_init,
                                     &game_task_state_1_home,
                                     &game_task_state_2_play,
									 &game_task_state_3_end}

};
ShootTask red_shoot_task = {.state = 0,
							.num_states = 4,
   							.button = 0,
							.servo_tim = &htim4,
							.channel = TIM_CHANNEL_2, // make sure this is red
							.shield_val = 2900, // tune
							.unshield_val = 400, // tune
							.laser_gpio = GPIO_PIN_15, // make sure this is red
							.state_list = {&shoot_task_state_0_init,
										   &shoot_task_state_1_wait,
										   &shoot_task_state_2_unshield,
										   &shoot_task_state_3_shoot}
};
ShootTask blue_shoot_task = {.state = 0,
							.num_states = 4,
   							.button = 0,
							.servo_tim = &htim4,
							.channel = TIM_CHANNEL_1, // make sure this is blue
							.shield_val = 2900, // tune
							.unshield_val = 400, // tune
							.laser_gpio = GPIO_PIN_14, // make sure this is blue, 15 and chan 2 are tied
							.state_list = {&shoot_task_state_0_init,
										   &shoot_task_state_1_wait,
										   &shoot_task_state_2_unshield,
										   &shoot_task_state_3_shoot}
};
ControllerTask blue_controller_task = {.dir = -1, // blue is fighter 2
									   .state = 0,
									   .num_states = 2,
									   .chan1 = TIM_CHANNEL_3,
									   .chan2 = TIM_CHANNEL_4,
									   .pot_zero = 0,
									   .des_pos = 0,
									   .prev_time = 0,
									   .current_time = 0,
									   .prev_ticks = 0,
									   .k_p = 1,
									   .k_i = 0,
									   .adc_val = 0,
									   .integral_error = 0,
									   .htim_encoder = &htim3,		// encoder timer for blue motor
									   .htim_dt = &htim2,
									   .hadc = &hadc1,              // ADC handle for blue motor potentiometer input
									   .motor = &mblue,
									   .encoder = &blue_encoder,
									   .state_list = {&controller_task_state_0_init,
													  &controller_task_state_1_pos}
};
ControllerTask red_controller_task = {.dir = 1, // red is fighter 1
									  .state = 0,
									  .num_states = 2,
									  .chan1 = TIM_CHANNEL_1,
									  .chan2 = TIM_CHANNEL_2,
									  .pot_zero = 0,
									  .des_pos = 0,
									  .prev_time = 0,
									  .current_time = 0,
									  .prev_ticks = 0,
									  .k_p = -1,
									  .k_i = 0,
									  .adc_val = 0,
									  .integral_error = 0,
									  .htim_encoder = &htim5,
									  .htim_dt = &htim2,
									  .hadc = &hadc1,
									  .motor = &mred,
									  .encoder = &red_encoder,
									  .state_list = {&controller_task_state_0_init,
													 &controller_task_state_1_pos}
};
ADCTask adc_task = {.state = 0,
					.num_states = 2,
					.red_contr_ptr = &red_controller_task,
					.blue_contr_ptr = &blue_controller_task,
					.red_photor_ptr = &red_photoresistor_task,
					.blue_photor_ptr = &blue_photoresistor_task,
					.hadc = &hadc1,
					.state_list = {&adc_task_state_0_init,
								   &adc_task_state_1_read}

};


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_TIM2_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM9_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_TIM5_Init();
  MX_TIM2_Init();
  MX_ADC1_Init();
  MX_TIM9_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

  HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_4);

  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);

  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_TIM_Base_Start(&htim4);
  HAL_TIM_Base_Start(&htim9);

  HAL_TIM_Encoder_Start(red_encoder.htim, TIM_CHANNEL_ALL);
  HAL_TIM_Encoder_Start(blue_encoder.htim, TIM_CHANNEL_ALL);

  enable(&mred);
  enable(&mblue);
  //
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
  // sleep pins
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
  adc_task_run(&adc_task);
  HAL_Delay(2000); // 2 second delay to let stuff get set up
  adc_task_run(&adc_task);
  lcd_clear();
  lcd_set_cursor(1, 0);
  lcd_print("  Zap'em Blast'em   ");
  lcd_set_cursor(2, 0);
  lcd_print("       Robots       ");
  calibration();
  HAL_Delay(1000);
  setup_encoder(&red_encoder);
  setup_encoder(&blue_encoder);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  adc_task_run(&adc_task);
	  game_task_run(&game_task);
	  sound_task_run(&sound_task);

	  // set play flag by each player holding button for 2 seconds
	  if (red_held && blue_held && game_task.play_flag == 0)
	  {
	      uint32_t now = __HAL_TIM_GET_COUNTER(game_task.htim);
	      red_elapsed = now - red_start;
	      blue_elapsed = now - blue_start;

	      if (red_elapsed > hold_time && blue_elapsed > hold_time)
	      {
	          game_task.play_flag = 1;
	          red_held = 0;
	          blue_held = 0;
	          red_shoot_task.button = 0;
	          blue_shoot_task.button = 0;
	      }
	  }
	  if (game_task.play_flag){ //shooting and scoring disabled when game hasn't started

		  shoot_task_run(&red_shoot_task);
		  shoot_task_run(&blue_shoot_task);
		  controller_task_run(&blue_controller_task);
		  controller_task_run(&red_controller_task);
		  photoresistor_task_run(&red_photoresistor_task);
		  photoresistor_task_run(&blue_photoresistor_task);
	  }

	  HAL_Delay(1);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = ENABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 4;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_56CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_6;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_7;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 0;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 4799;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM2;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1088;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 83;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 19999;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim5, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief TIM9 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM9_Init(void)
{

  /* USER CODE BEGIN TIM9_Init 0 */

  /* USER CODE END TIM9_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};

  /* USER CODE BEGIN TIM9_Init 1 */

  /* USER CODE END TIM9_Init 1 */
  htim9.Instance = TIM9;
  htim9.Init.Prescaler = 95;
  htim9.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim9.Init.Period = 65535;
  htim9.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim9.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim9) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim9, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM9_Init 2 */

  /* USER CODE END TIM9_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream0_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 PA3 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_13) // RED
	    {
	        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET) {
	            red_held = 1;
	            red_start = __HAL_TIM_GET_COUNTER(game_task.htim);
	            red_shoot_task.button = 1;
	            if (game_task.play_flag == 1){
	            	sound_task.laser_snd = 1;
	            }
	            if(playing){
					playing = 0;
					sound_task.state = 6;
				}

	        } else {
	            red_held = 0;
	            red_shoot_task.button = 0;
	        }
	    }

	    if (GPIO_Pin == GPIO_PIN_12) // BLUE
	    {
	        if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_12) == GPIO_PIN_RESET) {
	            blue_held = 1;
	            blue_start = __HAL_TIM_GET_COUNTER(game_task.htim);
	            blue_shoot_task.button = 1;
	            if (game_task.play_flag == 1){
					sound_task.laser_snd = 1;
				if(playing){
					playing = 0;
					sound_task.state = 6;
				}

				}
	        } else {
	            blue_held = 0;
	            blue_shoot_task.button = 0;
	        }
	    }

//    // === Photoresistor Hit Detection ===
//    if (blue_photoresistor_task.adc_val > blue_photoresistor_task.zero + blue_photoresistor_task.thresh)
//    {
//        blue_photoresistor_task.hit_flag = 1;
//    }
//    if (red_photoresistor_task.adc_val > red_photoresistor_task.zero + red_photoresistor_task.thresh)
//    {
//        red_photoresistor_task.hit_flag = 1;
//    }
}

void calibration(void) {
    const float Kp = 1.0f;
    const int  HOMING_TOL = 5;      // ticks tolerance
    const uint32_t TIMEOUT = 3000;  // ms
    uint32_t red_photo_sum = 0;
    uint32_t blue_photo_sum = 0;

    // 1) Center shields & zero photoresistors
    __HAL_TIM_SET_COMPARE(red_shoot_task.servo_tim,   red_shoot_task.channel,   red_shoot_task.shield_val);
    __HAL_TIM_SET_COMPARE(blue_shoot_task.servo_tim,  blue_shoot_task.channel,  blue_shoot_task.shield_val);
    HAL_Delay(1200);
    red_photo_sum  += red_photoresistor_task.adc_val;
    blue_photo_sum += blue_photoresistor_task.adc_val;

    // --- Drive both to MIN end ---
    set_duty(&mred,  +30);
    set_duty(&mblue, -30);
    wait_for_stall(&red_encoder);
    wait_for_stall(&blue_encoder);
    int32_t red_min  = red_encoder.ticks;
    int32_t blue_min = blue_encoder.ticks;
    set_duty(&mred,  0);
    set_duty(&mblue, 0);
    red_photo_sum  += red_photoresistor_task.adc_val;
	blue_photo_sum += blue_photoresistor_task.adc_val;

    // --- Drive both to MAX end ---
    set_duty(&mred,  -30);
    set_duty(&mblue, +30);
    wait_for_stall(&red_encoder);
    wait_for_stall(&blue_encoder);
    int32_t red_max  = red_encoder.ticks;
    int32_t blue_max = blue_encoder.ticks;
    set_duty(&mred,  0);
    set_duty(&mblue, 0);
    red_photo_sum  += red_photoresistor_task.adc_val;
	blue_photo_sum += blue_photoresistor_task.adc_val;

    // --- Compute midpoints ---
    int32_t red_home  = (-red_min  - red_max)  / 2;
    int32_t blue_home = (blue_min + blue_max) / 2;

    // --- Compute photo resistor zeros ---
    red_photoresistor_task.zero  = red_photo_sum / 3;
    blue_photoresistor_task.zero = blue_photo_sum / 3;

    // --- Drive both to their homes under P-control ---
    uint32_t t0 = HAL_GetTick();
    do {
        read_encoder(&red_encoder);
        read_encoder(&blue_encoder);

        go_to(&mred,  Kp, red_home,  -red_encoder.ticks);
        go_to(&mblue, Kp, blue_home, blue_encoder.ticks);

        HAL_Delay(1);
    } while ((abs(-red_encoder.ticks  - red_home)  > HOMING_TOL ||
              abs(blue_encoder.ticks - blue_home) > HOMING_TOL)
             && (HAL_GetTick() - t0) < TIMEOUT);

    set_duty(&mred,  0);
    set_duty(&mblue, 0);

    // 5) Reset encoder zero-reference at true home
    setup_encoder(&red_encoder);
    setup_encoder(&blue_encoder);

    // --- Compute and store travel ranges ---
    red_encoder.range  = abs(red_max  - red_min);
    blue_encoder.range = abs(blue_max - blue_min);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
