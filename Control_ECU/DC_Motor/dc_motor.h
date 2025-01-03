/*
 * dc_motor.h
 *
 *  Created on: Oct 9, 2022
 *      Author: AbdElRuhman
 */

#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_


#include "../GPIO/std_types.h"

typedef enum{
	clockwise,A_clockwise,off
}DcMotor_State;


void DcMotor_Init(uint8 speed);

void DcMotor_Rotate(DcMotor_State state);

#endif /* DC_MOTOR_H_ */
