/*
 * state_of_joint.h
 *
 *  Created on: Nov 30, 2023
 *      Author: VR
 */

#ifndef INC_STATE_OF_JOINT_H_
#define INC_STATE_OF_JOINT_H_

struct state_of_joint
{
	int HOLD;
	int STOP;
	int RUN;
	int CALIBRATE;
	int FAIL;
	int ARM;
	int DISARM;
	int MOVE;
};

typedef struct state_of_joint state_of_joint;

#endif /* INC_STATE_OF_JOINT_H_ */
