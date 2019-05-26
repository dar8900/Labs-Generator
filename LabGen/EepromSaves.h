#ifndef EEPROM_SAVES_H
#define EEPROM_SAVES_H
#include <Arduino.h>

#define INVALID_EEPROM_VALUE         255
#define UINT32_INVALID_VALUE  4294967295

#define LOG_ARRAY_SIZE 	 126

typedef struct
{
	bool SaveParameters;
}EEPROM_FLAG;

typedef enum
{
	P_I_FREQ_EEP = 0,
	LOG_TIME_EEP,
	ADC_TIME_EEP,
	LOG_UDM_EEP,
	SCR_SAVER_EEP,
	MAX_EEP_POS
}PARAMETERS_ARRAY_POS;


typedef struct 
{
	const uint8_t Address;
	uint32_t ParamValue;
	const uint32_t ParamDefaultVal;
}EEPARAMETERS;

typedef struct
{
	float LogValue;
	uint32_t Timestamp;
}LOG_DEF;

extern EEPROM_FLAG EeFlags;
extern EEPARAMETERS EepParameters[];
extern LOG_DEF LogArray[LOG_ARRAY_SIZE];
extern uint8_t LogArrayIndex;

void EepromInit(void);
void WriteToEeprom(void);
void LogAdcValue(LOG_DEF ValueToLog);
void ResetLogRecords(void);

#endif