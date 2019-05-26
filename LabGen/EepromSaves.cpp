#include "EepromSaves.h"
#include "EepromAddress.h"
#include "LabGen.h"
#include "TimeEvents.h"
#include "Setup.h"

EEPROM_FLAG EeFlags;
Chrono LogTimer(Chrono::SECONDS);

uint8_t LogArrayIndex;
LOG_DEF LogArray[LOG_ARRAY_SIZE];

EEPARAMETERS EepParameters[PARAMETERS_ADDRESSES_MAX_LIMIT] = 
{
	{P_I_FREQ_ADDR  , 0,     1000},
	{LOG_TIME_ADDR  , 0,      900},
	{ADC_TIME_ADDR  , 0,      100},
	{LOG_UDM_ADDR   , 0,  VOLTAGE},
	{SCR_SAVER_ADDR , 0,       30},
};

void EepromInit()
{
	if(EEPROM.read(CHECK_EEPROM_ADDR) != INVALID_EEPROM_VALUE)
	{
		for(int i = PARAMETERS_ADDRESSES_MIN_LIMIT; i < PARAMETERS_ADDRESSES_MAX_LIMIT+1; i++)
		{
			uint32_t TempValue = 0;
			EEPROM.get(EepParameters[i - 1].Address, TempValue);
			if(TempValue == 4294967295)
			{
				EEPROM.put(EepParameters[i - 1].Address, EepParameters[i - 1].ParamDefaultVal);
			}
			EEPROM.get(EepParameters[i - 1].Address, EepParameters[i - 1].ParamValue);
		}
#ifdef DEBUG_SERIAL
		// for(int i = PARAMETERS_ADDRESSES_MIN_LIMIT; i < PARAMETERS_ADDRESSES_MAX_LIMIT+1; i++)
		// {
			// DBG(EepParameters[i - 1].ParamValue);
		// }
#endif		
		if(EEPROM.read(LOG_ARRAY_INDEX_ADDR) == INVALID_EEPROM_VALUE)
		{
			EEPROM.update(LOG_ARRAY_INDEX_ADDR, 0);
			for(int i = 0; i < LOG_ARRAY_SIZE; i++)
			{
				LogArray[i].LogValue = 0.0;
				LogArray[i].Timestamp = 0;
				EEPROM.put(LOG_ADDRESSES_MIN_LIMIT + (i * LOG_SIZE), LogArray[i]);
			}
		}
		else
		{
			LogArrayIndex = EEPROM.read(LOG_ARRAY_INDEX_ADDR);
			for(int i = 0; i < LOG_ARRAY_SIZE; i++)
			{
				EEPROM.get(LOG_ADDRESSES_MIN_LIMIT + (i * LOG_SIZE), LogArray[i]);
			}
			if(LogArray[LOG_ARRAY_SIZE - 1].Timestamp != 0)
				Flags.LogFull = true;
#ifdef DEBUG_SERIAL			
			// for(int i = 0; i < LOG_ARRAY_SIZE; i++)
			// {
				// DBG(LogArray[i].LogValue);
				// DBG(LogArray[i].Timestamp);
			// }
#endif			
		}
	}
	else
	{
		EEPROM.write(CHECK_EEPROM_ADDR, 0);
		for(int i = PARAMETERS_ADDRESSES_MIN_LIMIT; i < PARAMETERS_ADDRESSES_MAX_LIMIT+1; i++)
		{
			EEPROM.put(EepParameters[i - 1].Address, EepParameters[i - 1].ParamDefaultVal);
		}		
		for(int i = PARAMETERS_ADDRESSES_MIN_LIMIT; i < PARAMETERS_ADDRESSES_MAX_LIMIT+1; i++)
		{
			EEPROM.get(EepParameters[i - 1].Address, EepParameters[i - 1].ParamValue);
		}
		if(EEPROM.read(LOG_ARRAY_INDEX_ADDR) == INVALID_EEPROM_VALUE)
		{
			EEPROM.update(LOG_ARRAY_INDEX_ADDR, 0);
			for(int i = 0; i < LOG_ARRAY_SIZE; i++)
			{
				LogArray[i].LogValue = 0.0;
				LogArray[i].Timestamp = 0;
				EEPROM.put(LOG_ADDRESSES_MIN_LIMIT + (i * LOG_SIZE), LogArray[i]);
			}
		}			
	}
}

void WriteToEeprom()
{
	uint32_t TmpValue = 0;
	if(EeFlags.SaveParameters)
	{
		EeFlags.SaveParameters = false;
		for(int i = PARAMETERS_ADDRESSES_MIN_LIMIT; i < PARAMETERS_ADDRESSES_MAX_LIMIT+1; i++)
		{
			EEPROM.get(EepParameters[i - 1].Address, TmpValue);
			if(TmpValue != EepParameters[i - 1].ParamValue)
				EEPROM.put(EepParameters[i - 1].Address, EepParameters[i - 1].ParamValue);
		}			
	}
}


void LogAdcValue(LOG_DEF ValueToLog)
{
	if(Flags.LogActive)
	{
		if(LogTimer.hasPassed(EepParameters[LOG_TIME_EEP].ParamValue, true))
		{
			LogArray[LogArrayIndex] = ValueToLog;
			EEPROM.put(LOG_ADDRESSES_MIN_LIMIT + (LogArrayIndex * LOG_SIZE), LogArray[LogArrayIndex]);
			LogArrayIndex++;
			if(LogArrayIndex >= LOG_ARRAY_SIZE)
			{
				LogArrayIndex = 0;
				Flags.LogFull = true;
			}
			EEPROM.update(LOG_ARRAY_INDEX_ADDR, LogArrayIndex);
		}
	}
}

void ResetLogRecords()
{
	EEPROM.update(LOG_ARRAY_INDEX_ADDR, 0);
	for(int i = 0; i < LOG_ARRAY_SIZE; i++)
	{
		LogArray[i].LogValue = 0.0;
		LogArray[i].Timestamp = 0;
		EEPROM.put(LOG_ADDRESSES_MIN_LIMIT + (i * LOG_SIZE), LogArray[i]);
	}	
	Flags.LogFull = false;
}






