/*
 * audio_data.h
 *
 *  Created on: Jun 9, 2025
 *      Author: andrewcarr
 */

#ifndef INC_LASER_BUF_H_
#define INC_LASER_BUF_H_

#include <stdint.h>

// the actual array lives in audio_data.c

extern const int16_t laser_buf[];
extern const uint32_t laser_buf_len;

#endif /* INC_LASER_BUF_H_ */
