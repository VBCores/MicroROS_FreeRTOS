/*
 * joint_config.c
 *
 *  Created on: Dec 15, 2023
 *      Author: Sorokonog
 */

#include "cmsis_os.h"
#include "joint_config.h"
#include "at24_eeprom.h"
#include "main.h"

void joint_config_main_part_init(joint_config * jconf)
{
#ifdef NODIPSWITCH
	jconf->joint_number = JOINT_N;
	jconf->domain_id = 0;
#else
	  joint_number_selector(jconf);
	  domain_id_selector(jconf);
#endif
}


void joint_config_freertos_init(joint_config * jconf)
{
	//TODO to make clear why JC not transfered from main()
#ifdef NODIPSWITCH
	joint_config_read(jconf);
	if (jconf->first_run != JOINT_N)
	{
		joint_config_assembler(jconf);
		jconf->joint_number = JOINT_N; //TODO!!!!!!!!!!!!!!!!!!
		jconf->direction = 1;
		jconf->first_run = JOINT_N;
		joint_config_write(jconf);
	}
#else
	joint_number_selector(jconf);
	domain_id_selector(jconf);
	if (&jconf->joint_number == 0)
	{
		joint_config_assembler(jconf);
		joint_config_write(jconf);
		return;
	}
	joint_config_read(jconf);
#endif
}

void joint_config_assembler(joint_config * jconf)
{
	jconf->domain_id = 0;
	jconf->joint_number = 0;
	jconf->full_steps = 0;
	jconf->gear_ratio = 0.0;
	jconf->lower_limit_enc = 0.0;
	jconf->lower_limit_ticks = 0;
	jconf->motor_type = 0;
	jconf->upper_limit_enc = 0.0;
	jconf->upper_limit_ticks = 0;
	jconf->zero_enc = 0.0;
	jconf->upper_limit_effort = 0.0;
	jconf->direction = 1;
	jconf->first_run = 0;
}



void joint_number_selector(joint_config * jc)
{
	if(HAL_GPIO_ReadPin(DIP_1_GPIO_Port, DIP_1_Pin))
	{
		jc->joint_number = 1;
	}
	else if(HAL_GPIO_ReadPin(DIP_2_GPIO_Port, DIP_2_Pin))
	{
		jc->joint_number = 2;
	}
	else if (HAL_GPIO_ReadPin(DIP_3_GPIO_Port, DIP_3_Pin))
	{
		jc->joint_number = 3;
	}
	else if (HAL_GPIO_ReadPin(DIP_4_GPIO_Port, DIP_4_Pin))
	{
		jc->joint_number = 4;
	}
	else if (HAL_GPIO_ReadPin(DIP_5_GPIO_Port, DIP_5_Pin))
	{
		jc->joint_number = 5;
	}
	else if (HAL_GPIO_ReadPin(DIP_6_GPIO_Port, DIP_6_Pin))
	{
		jc->joint_number = 6;
	}
	else
	{
		jc->joint_number = 0;
	}
}

void domain_id_selector(joint_config * jc)
{
#ifdef NODIPSWITCH
	jc->domain_id = 0;
#else
	if(HAL_GPIO_ReadPin(DIP_8_GPIO_Port, DIP_8_Pin))
	{
		jc->domain_id = 0;
	}
#endif
}

void joint_config_write(joint_config * jc)
{
	uint16_t address_of_domain_id = 0x00;
	uint16_t address_of_upper_limit_enc = sizeof(&jc->domain_id);
	uint16_t address_of_lower_limit_enc = address_of_upper_limit_enc + sizeof(&jc->upper_limit_enc);
	uint16_t address_of_upper_limit_ticks = address_of_lower_limit_enc + sizeof(&jc->lower_limit_enc);
	uint16_t address_of_lower_limit_ticks = address_of_upper_limit_ticks + sizeof(&jc->upper_limit_ticks);
	uint16_t address_of_zero_enc = address_of_lower_limit_ticks + sizeof(&jc->lower_limit_ticks);
	uint16_t address_of_joint_number = address_of_zero_enc + sizeof(&jc->zero_enc);
	uint16_t address_of_motor_type = address_of_joint_number + sizeof(&jc->joint_number);
	uint16_t address_of_full_steps = address_of_motor_type + sizeof(&jc->motor_type);
	uint16_t address_of_gear_ratio = address_of_full_steps + sizeof(&jc->full_steps);
	uint16_t address_of_upper_limit_effort = address_of_gear_ratio + sizeof(&jc->gear_ratio);
	uint16_t address_of_direction = address_of_upper_limit_effort + sizeof(&jc->upper_limit_effort);
	uint16_t address_of_first_run = address_of_direction + sizeof(&jc->direction);

	int timeout = 100;
	if (at24_isConnected())
	{

		at24_write(address_of_domain_id, &jc->domain_id, sizeof(&jc->domain_id), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_write(address_of_upper_limit_enc, &jc->upper_limit_enc, sizeof(&jc->upper_limit_enc), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_write(address_of_lower_limit_enc, &jc->lower_limit_enc, sizeof(&jc->lower_limit_enc), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_write(address_of_upper_limit_ticks, &jc->upper_limit_ticks, sizeof(&jc->upper_limit_ticks), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_write(address_of_lower_limit_ticks, &jc->lower_limit_ticks, sizeof(&jc->lower_limit_ticks), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_write(address_of_zero_enc, &jc->zero_enc, sizeof(&jc->zero_enc), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_write(address_of_joint_number, &jc->joint_number, sizeof(&jc->joint_number), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_write(address_of_motor_type, &jc->motor_type, sizeof(&jc->motor_type), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_write(address_of_full_steps, &jc->full_steps, sizeof(&jc->full_steps), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_write(address_of_gear_ratio, &jc->gear_ratio, sizeof(&jc->gear_ratio), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_write(address_of_upper_limit_effort, &jc->upper_limit_effort, sizeof(&jc->upper_limit_effort), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_write(address_of_direction, &jc->direction, sizeof(&jc->direction), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_write(address_of_first_run, &jc->first_run, sizeof(&jc->first_run), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);

	}
}

void joint_config_read(joint_config * jc)
{
	uint16_t address_of_domain_id = 0x00;
	uint16_t address_of_upper_limit_enc = sizeof(&jc->domain_id);
	uint16_t address_of_lower_limit_enc = address_of_upper_limit_enc + sizeof(&jc->upper_limit_enc);
	uint16_t address_of_upper_limit_ticks = address_of_lower_limit_enc + sizeof(&jc->lower_limit_enc);
	uint16_t address_of_lower_limit_ticks = address_of_upper_limit_ticks + sizeof(&jc->upper_limit_ticks);
	uint16_t address_of_zero_enc = address_of_lower_limit_ticks + sizeof(&jc->lower_limit_ticks);
	uint16_t address_of_joint_number = address_of_zero_enc + sizeof(&jc->zero_enc);
	uint16_t address_of_motor_type = address_of_joint_number + sizeof(&jc->joint_number);
	uint16_t address_of_full_steps = address_of_motor_type + sizeof(&jc->motor_type);
	uint16_t address_of_gear_ratio = address_of_full_steps + sizeof(&jc->full_steps);
	uint16_t address_of_upper_limit_effort = address_of_gear_ratio + sizeof(&jc->gear_ratio);
	uint16_t address_of_direction = address_of_upper_limit_effort + sizeof(&jc->upper_limit_effort);
	uint16_t address_of_first_run = address_of_direction + sizeof(&jc->direction);

	int timeout = 100;
	if (at24_isConnected())
		{
		at24_read(address_of_domain_id, &jc->domain_id, sizeof(&jc->domain_id), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_read(address_of_upper_limit_enc, &jc->upper_limit_enc, sizeof(&jc->upper_limit_enc), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_read(address_of_lower_limit_enc, &jc->lower_limit_enc, sizeof(&jc->lower_limit_enc), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_read(address_of_upper_limit_ticks, &jc->upper_limit_ticks, sizeof(&jc->upper_limit_ticks), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_read(address_of_lower_limit_ticks, &jc->lower_limit_ticks, sizeof(&jc->lower_limit_ticks), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_read(address_of_zero_enc, &jc->zero_enc, sizeof(&jc->zero_enc), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_read(address_of_joint_number, &jc->joint_number, sizeof(&jc->joint_number), timeout);
		vTaskDelay(1000/ portTICK_PERIOD_MS);
		at24_read(address_of_motor_type, &jc->motor_type, sizeof(&jc->motor_type), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_read(address_of_full_steps, &jc->full_steps, sizeof(&jc->full_steps), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_read(address_of_gear_ratio, &jc->gear_ratio, sizeof(&jc->gear_ratio), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_read(address_of_upper_limit_effort, &jc->upper_limit_effort, sizeof(&jc->upper_limit_effort), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_read(address_of_direction, &jc->direction, sizeof(&jc->direction), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		at24_read(address_of_first_run, &jc->first_run, sizeof(&jc->first_run), timeout);
		vTaskDelay(1/ portTICK_PERIOD_MS);
		}
}

