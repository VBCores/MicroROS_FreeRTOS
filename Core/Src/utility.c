/*
 * utility.c
 *
 *  Created on: Dec 21, 2023
 *      Author: VR
 */

#include "utility.h"


//TODO make sure that is works for less than 24 bit values
int32_t sign_extend_bits_to_32(int32_t x, uint8_t bits) {

	uint32_t sign_mask = 0;
	//getting value of sign bit
	sign_mask = 1u << (bits - 1);
	uint32_t sign_bit = 0;
	sign_bit = x & sign_mask;
	if(sign_bit) //if value < 0 therefore sign_bit == 1, fill first 8 bits with 1
	{
		int32_t res = 0;
		int32_t mask = 0b11111111;
		res |= x;
		res |= (mask << (bits));
		return res;
	}
    return x; //else return value itself
}

double clamp_value_noref(double min_value, double value, double max_value)
{
	value = (((min_value < value)? value : min_value) > max_value)? max_value: value;
	return value;
}

void clamp_value(double *min_value, double *value, double *max_value)
{
	*value = (((*min_value < *value)? *value : *min_value) > *max_value)? *max_value: *value;
}

