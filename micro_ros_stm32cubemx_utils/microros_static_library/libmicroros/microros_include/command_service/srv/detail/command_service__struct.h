// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from command_service:srv/CommandService.idl
// generated code does not contain a copyright notice

#ifndef COMMAND_SERVICE__SRV__DETAIL__COMMAND_SERVICE__STRUCT_H_
#define COMMAND_SERVICE__SRV__DETAIL__COMMAND_SERVICE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/CommandService in the package command_service.
typedef struct command_service__srv__CommandService_Request
{
  bool enable_config;
  bool disarm;
  bool arm;
  bool stop;
  bool hold;
  bool calibrate;
  bool get_position;
  bool set_zero;
  bool set_upper_limit;
  bool set_lower_limit;
  int32_t position;
} command_service__srv__CommandService_Request;

// Struct for a sequence of command_service__srv__CommandService_Request.
typedef struct command_service__srv__CommandService_Request__Sequence
{
  command_service__srv__CommandService_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} command_service__srv__CommandService_Request__Sequence;


// Constants defined in the message

/// Struct defined in srv/CommandService in the package command_service.
typedef struct command_service__srv__CommandService_Response
{
  bool success;
  int16_t angle;
  int32_t position_in_ticks;
} command_service__srv__CommandService_Response;

// Struct for a sequence of command_service__srv__CommandService_Response.
typedef struct command_service__srv__CommandService_Response__Sequence
{
  command_service__srv__CommandService_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} command_service__srv__CommandService_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // COMMAND_SERVICE__SRV__DETAIL__COMMAND_SERVICE__STRUCT_H_
