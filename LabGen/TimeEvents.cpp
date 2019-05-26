#include <Wire.h>
#include "Display.h"
#include "TimeEvents.h"
#include "EepromSaves.h"


#define SECOND_DELAY	1000

static RTC_DS1307 rtc;
DateTime TimeDate;
Chrono ChekSaveEeprom; 
Chrono TakeTimeTimer;
TIME_DATE_VAR ActualTimeDate;


uint8_t DayInMonth[12] = 
{
	31,
	28,
	31,
	30,
	31,
	30,
	31,
	31,
	30,
	31,
	30,
	31,	
};

void RtcInit()
{
	if(! rtc.begin())
	{
		while(1)
		{
			u8g2.clearBuffer();
			u8g2.drawStr(0,0, "Rtc Off");	
			u8g2.sendBuffer();		
		}
	}
	else if(!rtc.isrunning()) 
	{
		rtc.adjust(DateTime(DFLT_YEAR, DFLT_MONTH, DFLT_DAY, DFLT_HOUR, DFLT_MINUTE, 0));
	}
}

static void TakeTime()
{
	if(TakeTimeTimer.hasPassed(SECOND_DELAY, true))
	{
		TimeDate = rtc.now();
		ActualTimeDate.Hour   = TimeDate.hour();
		ActualTimeDate.Minute = TimeDate.minute();
		ActualTimeDate.Second = TimeDate.second();
		ActualTimeDate.Day    = TimeDate.day();
		ActualTimeDate.Month  = TimeDate.month();
		ActualTimeDate.Year   = TimeDate.year();
	}
}

// void TimestampToTimeVar(TIME_DATE_VAR *RefreshVar, uint32_t t)
// {
	// SECONDS_FROM_1970_TO_2000
	// RefreshVar->Hour    = (t / 3600) % 24;
	// RefreshVar->Minute  = (t / 60) % 60;
	// RefreshVar->Second  = t % 60
	// RefreshVar->Day   
	// RefreshVar->Month 
	// RefreshVar->Year  
	
// }


void CheckEvents()
{
	TakeTime();
	if(ChekSaveEeprom.hasPassed(SECOND_DELAY, true))
	{
		WriteToEeprom();
	}
	// if(LogTimer.hasPassed(EepParameters[LOG_TIME_EEP].ParamValue, true))
	// {
		
	// }
}

void SetTime(uint8_t Hour, uint8_t Minute)
{
	rtc.adjust(DateTime(ActualTimeDate.Year, ActualTimeDate.Month, ActualTimeDate.Day, Hour, Minute, 0));
}

void SetDate(uint8_t Day, uint8_t Month, uint16_t Year)
{
	rtc.adjust(DateTime(Year, Month, Day, ActualTimeDate.Hour , ActualTimeDate.Minute, ActualTimeDate.Second));
}

