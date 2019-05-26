#ifndef DISPLAY_H
#define DISPLAY_H
#include <Arduino.h>
#include <U8g2lib.h>

#define PIGRECO               3.141592653
#define TO_GRADES(Angle)     (Angle * (180.0 / PIGRECO))
#define TO_RADIANTS(Angle)   (Angle * (PIGRECO / 180.0))

#define DISPLAY_WIDTH	128
#define DISPLAY_HIGH	 64

#define STR_WIDTH(Str)      u8g2.getStrWidth(Str)
#define STR_HIGH         	u8g2.getAscent()

#define LEFT_ALIGN 	        0
#define RIGHT_ALIGN(Str) 	(DISPLAY_WIDTH - u8g2.getStrWidth(Str))
#define CENTER_ALIGN(Str) 	((DISPLAY_WIDTH - u8g2.getStrWidth(Str)) / 2)

#define TOP_POS				(1 + u8g2.getAscent())
#define CENTER_POS			((DISPLAY_HIGH/2) + (u8g2.getAscent() / 2))
#define BOTTOM_POS			(DISPLAY_HIGH )

#define TITLE_FONT			 u8g2_font_6x13_mf   
#define TITLE_Y_POS			 (9 + u8g2.getAscent())

#define TOP_BOTTOM_FONT	     u8g2_font_5x7_mf

#define ICON_X_START		  8
#define ICON_Y_START		 26
#define ICON_SCREEN_WIDHT	 20
#define ICON_SCREEN_HIGHT	 12
#define ICON_SCREEN_DELTA_X  10
#define ICON_SCREEN_DELTA_Y  10

#define ICON_BMP_WIDHT	     19
#define ICON_BMP_HIGH	     11

#define ARROW_UP              0
#define ARROW_DOWN            1
#define ARROW_LEFT            2
#define ARROW_RIGHT           3

#define PWM_ICON_X_POS      42 
#define PWM_ICON_Y_POS       0
#define PWM_ICON_WIDHT      10
#define PWM_ICON_HIGH        7

#define LOG_ICON_X_POS      54 
#define LOG_ICON_Y_POS       0
#define LOG_ICON_WIDHT       7
#define LOG_ICON_HIGH        7


#define CLOCK_RADIUS 		  30

#if (CLOCK_RADIUS > (DISPLAY_HIGH / 2))
#error Diametro orologio maggiore dello schermo
#endif

#define CLOCK_X_CENTER    (CLOCK_RADIUS + 1)
#define CLOCK_Y_CENTER    (DISPLAY_HIGH / 2)


#define ROUND_FIRST_DECIMAL(Float)	     (roundf(Float * 10)/10)
#define ROUND_SECOND_DECIMAL(Float)	     (roundf(Float * 100)/100)

typedef enum
{
	MAIN_MENU = 0,
	POWER_CURRENT,
	DIGITAL_IN_OUT,
	PWM,
	ANALOG,
	LOG,
	CHRONOMETER,
	SETUP,
	MAX_SCREENS,
	SINGLE_PAGE
}ICON_SCREENS_NUMBER;

typedef enum
{
	POWER = 0,
	CURRENT,
	MAX_P_I_ELEMENTS
}POWER_CURRENT_SCREEN;

typedef enum
{
	DIGITAL_INPUT = 0,
	DIGITAL_OUTPUT,
	MAX_DIGITAL_ELEMENTS
}DIGITAL_SCREEN;

typedef enum
{
	PWM_HF = 0,
	PWM_LF,
	MAX_PWM_ELEMENTS
}PWM_SCREEN;

typedef enum
{
	ANALOG_12V = 0,
	ANALOG_5V,
	MAX_ANALOG_ELEMENTS
}ANALOG_SCREEN;

typedef enum
{
	LOG_LIST = 0,
	LOG_GRAPHIC,
	RESET_LOG,
	MAX_LOG_ELEMENTS
}LOG_SCREEN;

typedef enum
{
	CHRONO = 0,
	TIMER,
	MAX_CHRONO_ELEMENTS
}CHRONO_SCREEN;

typedef enum
{
	SET_TIME = 0,
	SET_DATE,
	SET_MOSFET_HZ,
	SET_TIME_LOG,
	SET_ADC_TIME,
	SET_LOG_UDM,
	SET_SCREEN_SAVER_TIME,
	MAX_SETUP_ELEMENTS
}SETUP_SCREEN;

typedef enum
{
	POWER_CURRENT_SINGLE,
	DIGITAL_SINGLE,
	PULSE_W_SINGLE,
	ANALOGIC_SINGLE,
	LOG_SINGLE,
	CHRONO_SINGLE,
	SETUP_SCREEN_SINGLE,
}SINGLE_PAGES;


typedef struct
{
	char *ScreenTitle;
	uint8_t ScreenNumber; 
	bool HaveTitle;
	uint8_t IconNumber;
	const char **IconsName;
	const unsigned char **IconsBmp;
}ICON_SCREEN;

typedef struct
{
	uint8_t WichSinglePage;
	uint8_t ItemSelected;	
}SINGLE_PAGE_DEF;

extern U8G2_ST7920_128X64_F_HW_SPI u8g2;
extern uint8_t ScreenNumber;
extern SINGLE_PAGE_DEF SinglePage;

void WriteFloatToStr(float FValue, char *Str);
void DrawArrow(uint8_t XPos, uint8_t YPos, uint8_t Direction);
void ShowMessage(String Message);
void DrawTopInfo(void);
void DrawIconMenu(uint8_t WichMenu);

#endif