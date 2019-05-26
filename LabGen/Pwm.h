#ifndef PWM_H
#define PWM_H
#include <Arduino.h>

#define PWM_SETTING_FONT 	u8g2_font_6x13B_mr 
#define PWM_DC_Y_POS		(15 + STR_HIGH)
#define PWM_FRQ_Y_POS		(36 + STR_HIGH)

#define PWM_DC_FRAME_Y_POS  13
#define PWM_FRQ_FRAME_Y_POS 34

#define PWM_DC_FRAME_WIDHT  19
#define PWM_DC_FRAME_HIGH   13
#define PWM_FRQ_FRAME_WIDHT 79
#define PWM_FRQ_FRAME_HIGH  13

void PwmFunction(uint8_t WichPwm);

#endif