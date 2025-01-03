/*
 * dc_motor.c
 *
 *  Created on: Oct 9, 2022
 *      Author: AbdElRuhman
 */


#include "dc_motor.h"
#include "../GPIO/gpio.h"
#include "../PWM/pwm.h"
void DcMotor_Init(uint8 speed)
{
	PWM_Timer0_Start(speed);

	GPIO_setupPinDirection(PORTB_ID,PIN0_ID,PIN_OUTPUT);
	GPIO_setupPinDirection(PORTB_ID,PIN1_ID,PIN_OUTPUT);

}


void DcMotor_Rotate(DcMotor_State state)
{

	if(state == clockwise)
	{
		GPIO_writePin(PORTB_ID,PIN0_ID,LOGIC_LOW);
		GPIO_writePin(PORTB_ID,PIN1_ID,LOGIC_HIGH);
	}
	else if(state == A_clockwise)
	{
		GPIO_writePin(PORTB_ID,PIN0_ID,LOGIC_HIGH);
		GPIO_writePin(PORTB_ID,PIN1_ID,LOGIC_LOW);
	}
	else if(state == off)
	{
		GPIO_writePin(PORTB_ID,PIN0_ID,LOGIC_LOW);
		GPIO_writePin(PORTB_ID,PIN1_ID,LOGIC_LOW);
	}

}
