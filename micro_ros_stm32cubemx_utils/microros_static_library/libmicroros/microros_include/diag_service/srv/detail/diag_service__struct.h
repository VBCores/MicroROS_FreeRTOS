// generated from rosidl_generator_c/resource/idl__struct.h.em
// with input from diag_service:srv/DiagService.idl
// generated code does not contain a copyright notice

#ifndef DIAG_SERVICE__SRV__DETAIL__DIAG_SERVICE__STRUCT_H_
#define DIAG_SERVICE__SRV__DETAIL__DIAG_SERVICE__STRUCT_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


// Constants defined in the message

/// Struct defined in srv/DiagService in the package diag_service.
typedef struct diag_service__srv__DiagService_Request
{
  int8_t diag_get;
} diag_service__srv__DiagService_Request;

// Struct for a sequence of diag_service__srv__DiagService_Request.
typedef struct diag_service__srv__DiagService_Request__Sequence
{
  diag_service__srv__DiagService_Request * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} diag_service__srv__DiagService_Request__Sequence;


// Constants defined in the message

/// Struct defined in srv/DiagService in the package diag_service.
typedef struct diag_service__srv__DiagService_Response
{
  bool abs_encoder;
  bool quad_encoder;
  bool motor;
  bool power_supply;
  uint32_t deviation_in_ticks;
} diag_service__srv__DiagService_Response;

// Struct for a sequence of diag_service__srv__DiagService_Response.
typedef struct diag_service__srv__DiagService_Response__Sequence
{
  diag_service__srv__DiagService_Response * data;
  /// The number of valid items in data
  size_t size;
  /// The number of allocated items in data
  size_t capacity;
} diag_service__srv__DiagService_Response__Sequence;

#ifdef __cplusplus
}
#endif

#endif  // DIAG_SERVICE__SRV__DETAIL__DIAG_SERVICE__STRUCT_H_
