#ifndef EEPROM_ADDRESS_H
#define EEPROM_ADDRESS_H
#include "EepromSaves.h"

#define PARAMETERS_ADDRESSES_MIN_LIMIT	  1
#define PARAMETERS_ADDRESSES_MAX_LIMIT	  MAX_EEP_POS 
#define PARAMETERS_SIZE 				  4

#define LOG_ARRAY_INDEX_ADDR	  1999
#define LOG_ADDRESSES_MIN_LIMIT	  2000
#define LOG_ADDRESSES_MAX_LIMIT	  3008
#define LOG_SIZE 				  8


#define CHECK_EEPROM_ADDR	0

#define P_I_FREQ_ADDR		1
#define LOG_TIME_ADDR		(P_I_FREQ_ADDR + PARAMETERS_SIZE)
#define ADC_TIME_ADDR       (LOG_TIME_ADDR + PARAMETERS_SIZE)
#define LOG_UDM_ADDR        (ADC_TIME_ADDR + PARAMETERS_SIZE)
#define SCR_SAVER_ADDR		(LOG_UDM_ADDR + PARAMETERS_SIZE)

#endif