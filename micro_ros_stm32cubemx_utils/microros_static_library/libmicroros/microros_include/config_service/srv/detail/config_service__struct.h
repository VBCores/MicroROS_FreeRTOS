// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from config_service:srv/ConfigService.idl
// generated code does not contain a copyright notice

#ifndef CONFIG_SERVICE__SRV__DETAIL__CONFIG_SERVICE__STRUCT_H_
#define CONFIG_SERVICE__SRV__DETAIL__CONFIG_SERVICE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/ConfigService in the package config_service.
typedef struct config_service__srv__ConfigService_Request
{
  int8_t config_set;
  int8_t config_get;
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
} config_service__srv__ConfigService_Request;

// Struct for a sequence of config_service__srv__ConfigService_Request.
typedef struct config_service__srv__ConfigService_Request__Sequence
{
  config_service__srv__ConfigService_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} config_service__srv__ConfigService_Request__Sequence;


// Constants defined in the message

/// Struct defined in srv/ConfigService in the package config_service.
typedef struct config_service__srv__ConfigService_Response
{
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
} config_service__srv__ConfigService_Response;

// Struct for a sequence of config_service__srv__ConfigService_Response.
typedef struct config_service__srv__ConfigService_Response__Sequence
{
  config_service__srv__ConfigService_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} config_service__srv__ConfigService_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // CONFIG_SERVICE__SRV__DETAIL__CONFIG_SERVICE__STRUCT_H_
