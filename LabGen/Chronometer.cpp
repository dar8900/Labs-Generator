#include "LabGen.h"
#include "TimeEvents.h"
#include "Display.h"
#include "Keyboard.h"
#include "Setup.h"
#include "EepromSaves.h"
#include "Chronometer.h"

enum
{
	ALL_ZERO = 0,
	JUST_ONE_ZERO,
	NONE_ZERO
};

static bool TakeSecondDelayTime(uint32_t *Cnt, uint16_t DelaymSec)
{
	uint32_t Delaymsec = DelaymSec;
	if(*Cnt == 0)
		*Cnt = millis();
	if((millis() - *Cnt) >= Delaymsec)
	{
		*Cnt = 0;
		return true;
	}
	else
		return false;
}


static uint8_t CheckZeros(TIME_DATE_VAR *VarToChange)
{
	uint8_t Ret = 0;
	if(VarToChange->Centesimi == 0)
		Ret++;
	if(VarToChange->Decimi == 0)
		Ret++;
	if(VarToChange->Second == 0)
		Ret++;
	if(VarToChange->Minute == 0)
		Ret++;
	if(VarToChange->Hour == 0)
		Ret++;
	if(Ret == 5)
		Ret = ALL_ZERO;
	else if(Ret > 0 && Ret < 5)
		Ret = JUST_ONE_ZERO;
	else
		Ret = NONE_ZERO;
	return Ret;
}

static void In_De_CrementTimeVar(TIME_DATE_VAR *VarToChange, uint8_t IncrementOrDecrement)
{
	if(IncrementOrDecrement == CHRONO)
	{
		VarToChange->Decimi++;
		if(VarToChange->Decimi == 10)
		{
			VarToChange->Decimi = 0;
			VarToChange->Second++;			
		}
		if(VarToChange->Second == 60)
		{
			VarToChange->Second = 0;
			VarToChange->Minute++;			
		}
		if(VarToChange->Minute == 60)
		{
			VarToChange->Minute = 0;
			VarToChange->Hour++;			
		}
		if(VarToChange->Hour == 24)
		{
			VarToChange->Hour = 0;			
		}
	}
	else 
	{		
		if(VarToChange->Decimi == 0 && (VarToChange->Second != 0 || VarToChange->Minute != 0 || VarToChange->Hour != 0)) 
			VarToChange->Decimi = 9;
		else
			VarToChange->Decimi--;
		
		if(VarToChange->Second != 0 && VarToChange->Decimi == 9)
			VarToChange->Second--;
		else if(VarToChange->Second == 0 && VarToChange->Decimi == 9 && (VarToChange->Minute != 0 || VarToChange->Hour != 0)) 
			VarToChange->Second = 59;	
		
		if(VarToChange->Minute != 0 && VarToChange->Second == 59)
			VarToChange->Minute--;
		else if(VarToChange->Minute == 0 && VarToChange->Second == 59 && VarToChange->Hour != 0) 
			VarToChange->Minute = 59;
		
		if(VarToChange->Hour != 0 && VarToChange->Minute == 59)
			VarToChange->Hour--;	
	}
}

static void ResetTimeVar(TIME_DATE_VAR *VarToChange)
{
	VarToChange->Centesimi = 0;
	VarToChange->Decimi    = 0;
	VarToChange->Second    = 0;
	VarToChange->Minute    = 0;
	VarToChange->Hour      = 0;			
}




static void DrawChrono(TIME_DATE_VAR *TimeToDraw)
{
	char ChronoStr[16];
	snprintf(ChronoStr, 15, "%02d:%02d:%02d:%0d", TimeToDraw->Hour, TimeToDraw->Minute, TimeToDraw->Second, TimeToDraw->Decimi);
	u8g2.setFont(u8g2_font_t0_12b_me); // Max 21 caratteri w 6 h 11
	u8g2.drawStr(CENTER_ALIGN(ChronoStr), CENTER_POS, ChronoStr);
}

static void DrawTimer(TIME_DATE_VAR *TimeToDraw, bool TimerStarted, uint8_t BoxSel)
{
	char ChronoStr[16];
	snprintf(ChronoStr, 15 ,"%02d:%02d:%02d:%0d", TimeToDraw->Hour, TimeToDraw->Minute, TimeToDraw->Second, TimeToDraw->Decimi);
	u8g2.setFont(u8g2_font_t0_12b_me); // Max 21 caratteri w 6 h 11
	u8g2.drawStr(CENTER_ALIGN(ChronoStr), CENTER_POS, ChronoStr);
	if(!TimerStarted)
	{
		u8g2.drawFrame(CENTER_ALIGN(ChronoStr) - 1 + (BoxSel * 18), CENTER_POS - 1 - 9, 12, 12);	
	}
}


void ChronoTimerFunction(uint8_t WichFunc)
{
	bool ExitChronoTimer = false, StartChronoTimer = false;
	uint8_t Button = NO_PRESS, BoxSel = 0;
	uint32_t ChronoTimerCnt = 0;
	TIME_DATE_VAR ChronoTimerVar;
	memset(&ChronoTimerVar, 0x00, sizeof(TIME_DATE_VAR));
	ResetTimeVar(&ChronoTimerVar);
	Chrono ChronoTimerTime(Chrono::MICROS);
	while(!ExitChronoTimer)
	{
		if(TakeSecondDelayTime(&ChronoTimerCnt, 50) && StartChronoTimer)
		{
			In_De_CrementTimeVar(&ChronoTimerVar, WichFunc);
		}
		
		u8g2.clearBuffer();
		DrawTopInfo();
		if(WichFunc == CHRONO)
			DrawChrono(&ChronoTimerVar);
		else
			DrawTimer(&ChronoTimerVar, StartChronoTimer, BoxSel);
		u8g2.sendBuffer();
		CheckEvents();
		Button = ButtonPress();
		switch(Button)
		{
			case UP:
				if(WichFunc == TIMER)
				{					
					switch(BoxSel)
					{
						case 0:
							if(ChronoTimerVar.Hour > 0)
								ChronoTimerVar.Hour--;
							else
								ChronoTimerVar.Hour = 23;
							break;
						case 1:
							if(ChronoTimerVar.Minute > 0)
								ChronoTimerVar.Minute--;
							else
								ChronoTimerVar.Minute = 59;
							break;
						case 2:
							if(ChronoTimerVar.Second > 0)
								ChronoTimerVar.Second--;
							else
								ChronoTimerVar.Second = 59;						
							break;
						case 3:
							if(ChronoTimerVar.Decimi > 0)
								ChronoTimerVar.Decimi--;
							else
								ChronoTimerVar.Decimi = 9;						
							break;
						default:
							break;							
					}
				}
				break;
			case DOWN:			
				if(WichFunc == TIMER)
				{					
					switch(BoxSel)
					{
						case 0:
							if(ChronoTimerVar.Hour < 23)
								ChronoTimerVar.Hour++;
							else
								ChronoTimerVar.Hour = 0;
							break;
						case 1:
							if(ChronoTimerVar.Minute < 59)
								ChronoTimerVar.Minute++;
							else
								ChronoTimerVar.Minute = 0;
							break;
						case 2:
							if(ChronoTimerVar.Second < 59)
								ChronoTimerVar.Second++;
							else
								ChronoTimerVar.Second = 0;				
							break;
						case 3:
							if(ChronoTimerVar.Decimi < 9)
								ChronoTimerVar.Decimi++;
							else
								ChronoTimerVar.Decimi = 0;				
							break;
						default:
							break;							
					}
				}
				break;
			case LEFT:
				if(WichFunc == TIMER)
				{					
					if(BoxSel < 3)
						BoxSel++;
					else 
						BoxSel = 0;
				}
				break;
			case OK:
				StartChronoTimer = !StartChronoTimer;
				break;
			case EXIT:
				if(StartChronoTimer)
				{
					StartChronoTimer = false;
					ResetTimeVar(&ChronoTimerVar);
				}
				else
					ExitChronoTimer = true;
				break;
			default:
				break;			
		}
		if(CheckZeros(&ChronoTimerVar) == ALL_ZERO && WichFunc == TIMER && StartChronoTimer)
		{
			StartChronoTimer = false;
			ShowMessage("Timer finito!");
		}
	}
}










