#ifndef LOG_H
#define LOG_H
#include <Arduino.h>

#define LOG_LIST_Y_POS	18

#define LOG_GRAPHIC_X_START    0
#define LOG_GRAPHIC_Y_START   15
#define LOG_GRAPHIC_WIDHT    127
#define LOG_GRAPHIC_HIGH      (64 - LOG_GRAPHIC_Y_START)

#define LOG_GRAPHIC_FONT	     u8g2_font_4x6_mf 
#define LOG_GRAPHIC_STR_Y_POS 	14 

typedef enum
{
	NO_RECORDS = 0,
	SUCCESS,
	FAIL,
	MAX_MSG
}LOG_RECORD_MSG;

void DrawListLog(void);
void DrawGaphicLog(void);
void ResetLogFunction(void);
#endif