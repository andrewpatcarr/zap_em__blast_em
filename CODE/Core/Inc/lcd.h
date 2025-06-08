///*
// * lcd.h
// *
// *  Created on: May 22, 2025
// *      Author: andrewcarr and Kai De La Cruz
// */
//
#ifndef LCD_H
#define LCD_H

#include "stm32f4xx_hal.h"

// Adjust to match your LCD I2C address (typically 0x27 or 0x3F)
#define LCD_I2C_ADDR (0x27 << 1)

// External handle to use your I2C peripheral
extern I2C_HandleTypeDef hi2c1;

void lcd_init(void);
void lcd_clear(void);
void lcd_set_cursor(uint8_t row, uint8_t col);
void lcd_print(char *str);
void lcd_send_command(uint8_t cmd);
void lcd_send_data(uint8_t data);
void lcd_force_backlight_on(void);

#endif // LCD_H
