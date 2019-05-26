#ifndef SETUP_H
#define SETUP_H
#include <Arduino.h>

#define TIME_DATE_STR_FONT  	             u8g2_font_9x15B_mf 
#define TIME_DATE_STR_Y_POS 			 (32 + u8g2.getAscent())
	
#define TIME_DATE_BOX_Y_POS				 (32 - 2)
#define TIME_DATE_BOX_WIDTH               19
#define TIME_DATE_BOX_HIGH				 (u8g2.getAscent() + 3)  
#define TIME_SEPAR_WIDHT				   9 


#define VALUES_FONT 					 u8g2_font_9x18B_mn 
#define VALUES_UDM_FONT					 u8g2_font_6x10_tf   
#define VALUES_UDM_Y_POS                 (44 + u8g2.getAscent())
#define VALUES_STR_Y_POS				 (25 + u8g2.getAscent())
#define VALUES_BOX_Y_POS                 25
#define VALUES_BOX_WIDHT				 11
#define VALUES_BOX_HIGH				     19


#define ENUM_DESCR_Y_POS				(24 + STR_HIGH)
#define ENUM_DESCR_FONT				    u8g2_font_6x10_mf   //w6h10 21 caratteri

#define ENUM_VALUE_Y_POS				(40 + STR_HIGH)
#define ENUM_VALUE_FONT                 u8g2_font_6x13B_mf  //w6h13 21 caratteri

typedef struct
{
	uint8_t Value;
	char *EnumStr;
}ENUM_DEF;

typedef struct
{
	char *ParamTitle;
	uint32_t MaxValue;
	uint32_t MinValue;
	uint8_t ParamType;
	uint8_t ValueType;
	uint32_t *ParamValuePtr;
	char *ValuesUdm;
	const ENUM_DEF *EnumVar;
	char *EnumDescription;
}PARAMETERS_DEF;



typedef enum
{
	TIME_TYPE = 0,
	DATE_TYPE,
	VALUE_TYPE,
	ENUM_TYPE,
	MAX_TYPE
}PARAMETER_TYPE;

typedef enum
{
	UINT_TYPE = 0,
	INT_TYPE,
	FLOAT_TYPE,
	NO_TYPE,
	MAX_VALUE_TYPE
}VALUE_TYPES;

typedef enum
{
	TIME_ITEM = 0,
	DATE_ITEM,
	PWM_FREQ_ITEM,
	TIME_LOG_ITEM,
	ADC_TIME_ITEM,
	LOG_UDM_ITEM,
	SCR_SAVER_TIMER_ITEM,
	MAX_ITEM
}PARAMETERS_TAB_NUM;


typedef enum
{
	VOLTAGE,
	AMPERE,
	CELSIUS,
	METER,
	CENTIMETER,
	LUMEN,
	
	MAX_LOG_UDM
}LOG_UDM_VALUES_ENUM;


extern const PARAMETERS_DEF ParametersTab[];
extern const char *UdmLog[];

void EnterSetup(uint8_t WichSetup);

#endif