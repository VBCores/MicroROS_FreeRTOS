/*
 * joint_config.h
 *
 *  Created on: Nov 22, 2023
 *      Author: VR
 */
#pragma once
#ifndef INC_JOINT_CONFIG_H_
#define INC_JOINT_CONFIG_H_


struct joint_config
{
	int8_t first_run;
	int8_t domain_id;
	int8_t joint_number;
	int8_t motor_type;
	int8_t direction;
	uint32_t full_steps;
	float gear_ratio;
	int32_t upper_limit_ticks;
	int32_t lower_limit_ticks;
	float upper_limit_enc;
	float lower_limit_enc;
	float zero_enc;
	float upper_limit_effort;
};

typedef struct joint_config joint_config;

void joint_config_main_part_init(joint_config * jc);
void joint_config_freertos_init(joint_config * jc);
void joint_config_assembler(joint_config *jc);
void joint_number_selector(joint_config * jc);
void domain_id_selector(joint_config * jc);
void joint_config_write(joint_config * jc);
void joint_config_read(joint_config * jc);

#endif /* INC_JOINT_CONFIG_H_ */
