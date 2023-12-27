/*
 * joint_diagnostics.h
 *
 *  Created on: Nov 30, 2023
 *      Author: VR
 */

#ifndef INC_JOINT_DIAGNOSTICS_H_
#define INC_JOINT_DIAGNOSTICS_H_

struct joint_diagnostics
{
	int motor_disabled;
	int abs_encoder_failed;
	int quad_encoder_failed;
	int deviation_detected;
};

typedef struct joint_diagnostics joint_diagnostics;

#endif /* INC_JOINT_DIAGNOSTICS_H_ */
