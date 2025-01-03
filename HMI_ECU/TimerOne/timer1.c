/*
 * timer.c
 *
 *  Created on: Oct 29, 2022
 *      Author: AbdElRuhman
 */

#include "timer1.h"
#include "../GPIO/gpio.h"
#include "avr/interrupt.h"

static volatile void (*ptr_funCallBabk)(void)=NULL_PTR;



ISR(TIMER1_OVF_vect)
{
	if(ptr_funCallBabk != NULL_PTR)
	{
		(*ptr_funCallBabk)();
	}

}


ISR(TIMER1_COMPA_vect)
{
	if(ptr_funCallBabk != NULL_PTR)
	{
		(*ptr_funCallBabk)();
	}

}
void Timer1_init(const Timer1_ConfigType * Config_Ptr)
{


	TCNT1=Config_Ptr->initial_value;
	TCCR1A = (1<<FOC1A) | (1<<FOC1B);
	TCCR1B = (TCCR1B  & 0xF8) | (Config_Ptr->prescaler);

	if(Config_Ptr->mode==normal)
	{
		TIMSK |= (1<<TOIE1);
	}
	else if(Config_Ptr->mode==compare)
	{
		OCR1A=Config_Ptr->compare_value;
		TCCR1B |= (1<<WGM12) ;
		TIMSK |= (1<<OCIE1A);
	}
}




void Timer1_setCallBack(void(*a_ptr)(void))
{
	ptr_funCallBabk=a_ptr;
}


void Timer1_deInit(void)
{
	TCCR1A=0;
	TCCR1B=0;

	TCNT1=0;
	OCR1A=0;


	TIMSK &= ~(1<<TOIE1);
	TIMSK &= ~(1<<OCIE1A);

}
