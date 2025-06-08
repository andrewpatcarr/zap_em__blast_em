/*
 * encoder_task.h
 *
 *  Created on: Jun 7, 2025
 *      Author: andrewcarr and Kai De La Cruz
 */

#ifndef INC_ENCODER_DRIVER_H_
#define INC_ENCODER_DRIVER_H_

#include <stdint.h> // Allows use of standard integer types
#include "stm32f4xx_hal.h"  // or your STM32 family


// A datatype for a structure to hold task configuration and state.
// Additional fields can be added as desired.
typedef struct
{
    uint32_t             zero;        // Timer value when "zeroed"
    int32_t              ar;          // Auto-reload or max timer value
    int32_t              ticks;       // Accumulated ticks
    int32_t              last_ticks;  // Last read relative count
    int32_t              range;       // Logical range of encoder
    TIM_HandleTypeDef   *htim;        // Timer handle
} encoder_t;


void setup_encoder(encoder_t *p_enc);
void read_encoder(encoder_t *p_enc);

#endif /* INC_ENCODER_DRIVER_H_ */
