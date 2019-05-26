#ifndef TIME_EVENTS_H
#define TIME_EVENTS_H
#include <Arduino.h>
#include <Chrono.h>
#include <RTClib.h>

typedef struct
{
	uint8_t 	Hour;
	uint8_t 	Minute;
	uint8_t 	Second;
	uint8_t     Decimi;
	uint8_t     Centesimi;
	uint8_t 	Day;
	uint8_t 	Month;
	uint16_t    Year;
	
}TIME_DATE_VAR;



#define DFLT_HOUR	      0
#define DFLT_MINUTE       0
#define DFLT_DAY          0
#define DFLT_MONTH        1
#define DFLT_YEAR      2019

extern uint8_t DayInMonth[];
extern TIME_DATE_VAR ActualTimeDate;
extern DateTime TimeDate;

void RtcInit(void);
extern void CheckEvents(void);
void SetTime(uint8_t Hour, uint8_t Minute);
void SetDate(uint8_t Day, uint8_t Month, uint16_t Year);

#endif