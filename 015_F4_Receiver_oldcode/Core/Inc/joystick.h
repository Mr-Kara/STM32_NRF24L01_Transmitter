
#include "main.h"

typedef struct
{
	uint16_t Raw_Data[100];
	double Scale_Data[100];
	uint8_t button;
	
}joystickParams_typedef;

extern joystickParams_typedef jParams;

float Read_Joystick_X(void);
float Read_Joystick_Y(void);


