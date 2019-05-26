#include "LabGen.h"
#include "TimeEvents.h"
#include "Keyboard.h"
#include "Setup.h"
#include "EepromSaves.h"
#include "Pwm.h"
#include "Display.h"

enum
{
	SET_DUTY = 0,
	SET_FRQ,
	MAX_PWM_SETTING
};


typedef struct
{
	uint8_t DutyCycle;
	uint32_t MaxFrequency;
	uint32_t MinFrequency;
}PWM_DEF;

PWM_DEF PwmTab[MAX_PWM_ELEMENTS] = 
{
	{0 , 1000000, 1000},
	{0 ,    1000,    1},
};

static const char *BottomPwmBarStr[] =
{
	"Left per frq.",
	"Left per DC",
};

uint8_t PwmDutyCycle;
uint32_t PwmFrequency;

static void DrawBottomPwmBar(uint8_t WichSet)
{
	u8g2.setFont(TOP_BOTTOM_FONT);
	if(WichSet == SET_DUTY)
		u8g2.drawStr(LEFT_ALIGN, BOTTOM_POS, BottomPwmBarStr[SET_FRQ]);
	else
		u8g2.drawStr(LEFT_ALIGN, BOTTOM_POS, BottomPwmBarStr[SET_FRQ]);
	u8g2.drawStr(RIGHT_ALIGN("Ok pwm"), BOTTOM_POS, "Ok pwm");
}

static void DrawPwmScreen(uint8_t WichSet)
{
	String DutyCycleStr, FrequencyStr;
	DutyCycleStr = String(PwmDutyCycle) + "%";
	FrequencyStr = String(PwmFrequency) + "Hz";
	u8g2.setFont(PWM_SETTING_FONT);
	u8g2.drawStr(CENTER_ALIGN(DutyCycleStr.c_str()), PWM_DC_Y_POS, DutyCycleStr.c_str());
	u8g2.drawStr(CENTER_ALIGN(FrequencyStr.c_str()), PWM_FRQ_Y_POS, FrequencyStr.c_str());
	if(WichSet == SET_DUTY)
		u8g2.drawFrame(CENTER_ALIGN(DutyCycleStr.c_str()) - 1, PWM_DC_FRAME_Y_POS, STR_WIDTH(DutyCycleStr.c_str()) + 2, PWM_DC_FRAME_HIGH);
	else
		u8g2.drawFrame(CENTER_ALIGN(FrequencyStr.c_str()) - 1, PWM_FRQ_FRAME_Y_POS, STR_WIDTH(FrequencyStr.c_str()) + 2, PWM_FRQ_FRAME_HIGH);
	
}

static void PwmManage(uint8_t WichPwm, bool IsActive)
{
	uint16_t RealDuty = (uint16_t) PwmDutyCycle;
	RealDuty = (RealDuty * 1023) / 100;
	if(IsActive)
	{
		if(WichPwm == PWM_HF)
		{
			Timer1.setPeriod(HZ_TO_MICROSEC(PwmFrequency));
			Timer1.setPwmDuty(PWM_OUTPUT_PIN_HF, RealDuty);
		}
		else if(WichPwm == PWM_LF)
		{
			Timer3.setPeriod(HZ_TO_MICROSEC(PwmFrequency));
			Timer3.setPwmDuty(PWM_OUTPUT_PIN_LF, RealDuty);		
		}
	}
	else
	{
		if(WichPwm == PWM_HF)
		{
			Timer1.setPeriod(HZ_TO_MICROSEC(PwmFrequency));
			Timer1.setPwmDuty(PWM_OUTPUT_PIN_HF, 0);
		}
		else if(WichPwm == PWM_LF)
		{
			Timer3.setPeriod(HZ_TO_MICROSEC(PwmFrequency));
			Timer3.setPwmDuty(PWM_OUTPUT_PIN_LF, 0);		
		}		
	}
}

void PwmFunction(uint8_t WichPwm)
{
	bool ExitPwm = false, IsActive = false;
	uint8_t Button = NO_PRESS, WichSetting = SET_DUTY;
	while(!ExitPwm)
	{
		u8g2.clearBuffer();
		DrawTopInfo();
		DrawPwmScreen(WichSetting);
		DrawBottomPwmBar(WichSetting);
		u8g2.sendBuffer();
		CheckEvents();
		Button = ButtonPressContinue();
		switch(Button)
		{
			case UP:
				if(WichSetting == SET_DUTY)
				{
					if(PwmDutyCycle > 0)
						PwmDutyCycle--;
					else
						PwmDutyCycle = 100;
				}
				else
				{
					if(PwmFrequency > PwmTab[WichPwm].MinFrequency)
						PwmFrequency--;
					else
						PwmFrequency = PwmTab[WichPwm].MaxFrequency;					
				}
				break;
			case DOWN:
				if(WichSetting == SET_DUTY)
				{
					if(PwmDutyCycle < 100)
						PwmDutyCycle++;
					else
						PwmDutyCycle = 0;					
				}		
				else
				{
					if(PwmFrequency < PwmTab[WichPwm].MaxFrequency)
						PwmFrequency++;
					else
						PwmFrequency = PwmTab[WichPwm].MinFrequency;						
				}					
				break;
			case LEFT:
				if(WichSetting < MAX_PWM_SETTING - 1)
					WichSetting++;
				else
					WichSetting = 0;
				break;
			case OK:
				if(!IsActive)
					IsActive = true;
				else
					IsActive = false;
				break;
			case EXIT:
				ExitPwm = true;
				break;
			default:
				break;
		}
		if(IsActive)
			Flags.PwmActive = true;
		else
			Flags.PwmActive = false;
		PwmManage(WichPwm, IsActive);
	}
}