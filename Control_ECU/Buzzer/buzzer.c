/*
 * buzzer.c
 *
 *  Created on: Nov 3, 2022
 *      Author: AbdElRuhman
 */

#include "buzzer.h"
#include "../GPIO/gpio.h"
#include "../GPIO/common_macros.h"

#define BuzzerPort  PORTD_ID
#define BuzzerPin  PIN3_ID



void Buzzer_init(void)
{
	GPIO_setupPinDirection(BuzzerPort,BuzzerPin,PIN_OUTPUT);
}

void Buzzer_on(void)
{
	GPIO_writePin(BuzzerPort,BuzzerPin,LOGIC_HIGH);

}
void Buzzer_off(void)
{
	GPIO_writePin(BuzzerPort,BuzzerPin,LOGIC_LOW);
}
