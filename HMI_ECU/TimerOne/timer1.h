/*
 * timer.h
 *
 *  Created on: Oct 29, 2022
 *      Author: AbdElRuhman
 */

#ifndef TIMERONE_TIMER_H_
#define TIMERONE_TIMER_H_

#include "../GPIO/std_types.h"
typedef enum{
	noClock,noPre,pre8,pre64,pre256,pre1024
}Timer1_Prescaler;


typedef enum{
	normal,compare
}Timer1_Mode;



typedef struct {
uint16 initial_value;
uint16 compare_value; // it will be used in compare mode only.
Timer1_Prescaler prescaler;
Timer1_Mode mode;
} Timer1_ConfigType;


void Timer1_init(const Timer1_ConfigType * Config_Ptr);
void Timer1_deInit(void);
void Timer1_setCallBack(void(*a_ptr)(void));

#endif /* TIMERONE_TIMER_H_ */
