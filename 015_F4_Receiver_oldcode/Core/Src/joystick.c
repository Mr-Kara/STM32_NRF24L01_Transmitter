
#include "joystick.h"
#include "ads1115.h"

joystickParams_typedef jParams;

//These adjustments were made cause i was using cheap joystick sorry guys 
float Read_Joystick_X(void)
{
	float joystick_x;
	
	joystick_x = ((Read_Ads1115(2))-1632)*(200.0/1632.0); // set to initial position's value 0 
	HAL_Delay(50);
	
	return ((joystick_x)); 
	
}

float Read_Joystick_Y(void)
{
	float joystick_y;
	
	joystick_y = ((Read_Ads1115(1))-1632)*(200.0/1632.0);
	HAL_Delay(50);
	
	return ((joystick_y));
	
}


