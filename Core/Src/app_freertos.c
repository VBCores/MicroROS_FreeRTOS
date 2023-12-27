/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <uxr/client/transport.h>
#include <rmw_microxrcedds_c/config.h>
#include <rmw_microros/rmw_microros.h>

#include <rmw_microros/rmw_microros.h>

#include <type_utilities.h>

#include <config_service/srv/config_service.h>
#include <command_service/srv/command_service.h>
#include <diag_service/srv/diag_service.h>
#include <std_srvs/srv/empty.h>


#include <std_msgs/msg/int32.h>
#include <std_msgs/msg/u_int32.h>
#include <std_msgs/msg/int16_multi_array.h>
#include <std_msgs/msg/float32_multi_array.h>
#include <std_msgs/msg/string.h>
#include <sensor_msgs/msg/joint_state.h>

#include <tmc5160.h>
#include <as50xx.h>
#include <at24_eeprom.h>

#include <joint_config.h>
#include <state_of_joint.h>
#include <joint_diagnostics.h>

#include <math.h>
#include "utility.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */


/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 3000 * 4
};
/* Definitions for quadEncoderRead */
osThreadId_t quadEncoderReadHandle;
const osThreadAttr_t quadEncoderRead_attributes = {
  .name = "quadEncoderRead",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 64 * 4
};
/* Definitions for absEncoderRead */
osThreadId_t absEncoderReadHandle;
const osThreadAttr_t absEncoderRead_attributes = {
  .name = "absEncoderRead",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 64 * 4
};
/* Definitions for configService */
osThreadId_t configServiceHandle;
const osThreadAttr_t configService_attributes = {
  .name = "configService",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 500 * 4
};
/* Definitions for commandService */
osThreadId_t commandServiceHandle;
const osThreadAttr_t commandService_attributes = {
  .name = "commandService",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 500 * 4
};
/* Definitions for jcPublisher */
osThreadId_t jcPublisherHandle;
const osThreadAttr_t jcPublisher_attributes = {
  .name = "jcPublisher",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 500 * 4
};
/* Definitions for jcSubscriber */
osThreadId_t jcSubscriberHandle;
const osThreadAttr_t jcSubscriber_attributes = {
  .name = "jcSubscriber",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 500 * 4
};
/* Definitions for motorRead */
osThreadId_t motorReadHandle;
const osThreadAttr_t motorRead_attributes = {
  .name = "motorRead",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 64 * 4
};
/* Definitions for calibrateTask */
osThreadId_t calibrateTaskHandle;
const osThreadAttr_t calibrateTask_attributes = {
  .name = "calibrateTask",
  .priority = (osPriority_t) osPriorityLow,
  .stack_size = 64 * 4
};
/* Definitions for PublisherSemaphore */
osSemaphoreId_t PublisherSemaphoreHandle;
osStaticSemaphoreDef_t PublisherSemaphoreControlBlock;
const osSemaphoreAttr_t PublisherSemaphore_attributes = {
  .name = "PublisherSemaphore",
  .cb_mem = &PublisherSemaphoreControlBlock,
  .cb_size = sizeof(PublisherSemaphoreControlBlock),
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void config_init();
void jc_sub_cb(const void * msg_input);
float steps_to_rads(int32_t steps);
int32_t rad_to_steps(float rads);

void config_service_callback(const void * request_msg, void * response_msg);
void command_service_callback(const void * request_msg, void * response_msg);
void diag_service_callback(const void * request_msg, void * response_msg);

bool cubemx_transport_open(struct uxrCustomTransport * transport);
bool cubemx_transport_close(struct uxrCustomTransport * transport);
size_t cubemx_transport_write(struct uxrCustomTransport* transport, const uint8_t * buf, size_t len, uint8_t * err);
size_t cubemx_transport_read(struct uxrCustomTransport* transport, uint8_t* buf, size_t len, int timeout, uint8_t* err);

void * microros_allocate(size_t size, void * state);
void microros_deallocate(void * pointer, void * state);
void * microros_reallocate(void * pointer, size_t size, void * state);
void * microros_zero_allocate(size_t number_of_elements, size_t size_of_element, void * state);


rcl_publisher_t jc_publisher;
rcl_subscription_t jc_subscriber;
rclc_executor_t jc_executor, config_executor, command_service_executor, diag_service_executor;
rclc_support_t support;
rcl_allocator_t allocator;
//rcl_allocator_t new_allocator; //with custom memory allocators
rcl_node_t node;
rcl_init_options_t init_options;
rcl_ret_t rc;

rcl_service_t config_service, command_service, diag_service;
config_service__srv__ConfigService_Response response_msg;
config_service__srv__ConfigService_Request request_msg;

command_service__srv__CommandService_Response command_service_response_msg;
command_service__srv__CommandService_Request command_service_request_msg;

diag_service__srv__DiagService_Response diag_service_response_msg;
diag_service__srv__DiagService_Request diag_service_request_msg;

std_msgs__msg__String msg;
sensor_msgs__msg__JointState js_out, js_in;

std_msgs__msg__Int32 msg_out, msg_in;


float angle = 0; //Position in rad. read from TMC5160
float vel = 0; //Vel read from TMC5160
float effort = 0; //Effort for TMC5160
uint16_t AbsEncAngle = 0; //Abs encoder value
uint16_t QuadEncData = 0;//QUAD Encoder value

//test purpose
float dev_angle = 0;
int Calibrate = 0;
//test purpose

extern uint32_t read_fault; //soft WD
extern IWDG_HandleTypeDef hiwdg; //hard WD

state_of_joint state_o_j = {0,0,0,0,0,0,0,0};
joint_diagnostics j_diagnostics;
extern joint_config jc;
extern motor_config motor_cfg;

extern FDCAN_HandleTypeDef hfdcan1;
extern TIM_HandleTypeDef htim8;

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask02(void *argument);
void StartTask03(void *argument);
void StartTask04(void *argument);
void StartTask06(void *argument);
void StartTask09(void *argument);
void StartTask10(void *argument);
void StartTask05(void *argument);
void StartTask01(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of PublisherSemaphore */
  PublisherSemaphoreHandle = osSemaphoreNew(1, 0, &PublisherSemaphore_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of quadEncoderRead */
  quadEncoderReadHandle = osThreadNew(StartTask02, NULL, &quadEncoderRead_attributes);

  /* creation of absEncoderRead */
  absEncoderReadHandle = osThreadNew(StartTask03, NULL, &absEncoderRead_attributes);

  /* creation of configService */
  configServiceHandle = osThreadNew(StartTask04, NULL, &configService_attributes);

  /* creation of commandService */
  commandServiceHandle = osThreadNew(StartTask06, NULL, &commandService_attributes);

  /* creation of jcPublisher */
  jcPublisherHandle = osThreadNew(StartTask09, NULL, &jcPublisher_attributes);

  /* creation of jcSubscriber */
  jcSubscriberHandle = osThreadNew(StartTask10, NULL, &jcSubscriber_attributes);

  /* creation of motorRead */
  motorReadHandle = osThreadNew(StartTask05, NULL, &motorRead_attributes);

  /* creation of calibrateTask */
  calibrateTaskHandle = osThreadNew(StartTask01, NULL, &calibrateTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */

		//Joint config
		joint_config_freertos_init(&jc); //TODO see the function description
		//Motor config
		tmc5160_motor_config(jc.motor_type, jc.direction, jc.full_steps, jc.gear_ratio, jc.upper_limit_effort, &motor_cfg);
		//Disable config service at start
		vTaskSuspend(configServiceHandle);
		//Disable calibration at start
		vTaskSuspend(calibrateTaskHandle);


		//RCL init

		rmw_uros_set_custom_transport(
						true,
						(void *) &hfdcan1,
						cubemx_transport_open,
						cubemx_transport_close,
						cubemx_transport_write,
						cubemx_transport_read);

		rcl_allocator_t freeRTOS_allocator = rcutils_get_zero_initialized_allocator();
		freeRTOS_allocator.allocate = microros_allocate;
		freeRTOS_allocator.deallocate = microros_deallocate;
		freeRTOS_allocator.reallocate = microros_reallocate;
		freeRTOS_allocator.zero_allocate =  microros_zero_allocate;

		rcutils_set_default_allocator(&freeRTOS_allocator);

		allocator = rcl_get_default_allocator();
		init_options = rcl_get_zero_initialized_init_options();

		rcl_init_options_set_domain_id(&init_options, jc.domain_id); //setting the domain ID

		rcl_init_options_init(&init_options, allocator);

		rmw_init_options_t* rmw_options = rcl_init_options_get_rmw_init_options(&init_options);

		uint32_t client_key;
		client_key = 10 + jc.joint_number;
		rmw_uros_options_set_client_key(client_key, rmw_options);

		rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator);

		//Control part
		char node_name[13];
		sprintf(node_name,"joint_node_%d", jc.joint_number);

		rclc_node_init_default(&node, node_name, "", &support);

		char joint_name[8];
		sprintf(joint_name,"joint_%d", jc.joint_number);

		//JC publisher
		//memory allocation for JS vars
		js_out.position.size=1;
		js_out.position.capacity=1;
		js_out.position.data = malloc(js_out.position.capacity*sizeof(double));
		js_out.position.data[0] = 0;
		js_out.velocity.size=1;
		js_out.velocity.capacity=1;
		js_out.velocity.data = malloc(js_out.velocity.capacity*sizeof(double));
		js_out.velocity.data[0] = 0;
		js_out.effort.size=1;
		js_out.effort.capacity=1;
		js_out.effort.data = malloc(js_out.effort.capacity*sizeof(double));
		js_out.effort.data[0] = 0;
		js_out.name.capacity = 1;
		js_out.name.size = 1;
		js_out.name.data = (std_msgs__msg__String*) malloc(js_out.name.capacity*sizeof(std_msgs__msg__String));
		js_out.name.data[0].data = joint_name;

		char topic_pub_name[18];
		sprintf(topic_pub_name,"joint_state_pub_%d", jc.joint_number);

		rclc_publisher_init_default(
			&jc_publisher,
			&node,
			ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, JointState),
			topic_pub_name);

		//JC control Subscriber
		//memory allocation for JS vars
		js_in.position.size=1;
		js_in.position.capacity=1;
		js_in.position.data = malloc(js_in.position.capacity*sizeof(double));
		js_in.position.data[0] = 0;
		js_in.velocity.size=1;
		js_in.velocity.capacity=1;
		js_in.velocity.data = malloc(js_in.velocity.capacity*sizeof(double));
		js_in.velocity.data[0] = 0;
		js_in.effort.size=1;
		js_in.effort.capacity=1;
		js_in.effort.data = malloc(js_in.effort.capacity*sizeof(double));
		js_in.effort.data[0] = 0;
		js_in.name.capacity = 1;
		js_in.name.size = 1;
		js_in.name.data = (std_msgs__msg__String*) malloc(js_in.name.capacity*sizeof(std_msgs__msg__String));

		char topic_sub_name[18];
		sprintf(topic_sub_name,"joint_state_sub_%d", jc.joint_number);

		rc = rclc_subscription_init_default(
			&jc_subscriber, &node,
			ROSIDL_GET_MSG_TYPE_SUPPORT(sensor_msgs, msg, JointState),
			topic_sub_name);

		jc_executor = rclc_executor_get_zero_initialized_executor();
		rc = rclc_executor_init(&jc_executor, &support.context, 1, &allocator);

		rc = rclc_executor_add_subscription(
			&jc_executor, &jc_subscriber, &js_in,
			&jc_sub_cb, ON_NEW_DATA);

		//Services
		//Config Service
		//for some reason it require its own executor
		config_executor = rclc_executor_get_zero_initialized_executor();
		rc = rclc_executor_init(&config_executor, &support.context, 1, &allocator);

		char config_service_name[17];
		sprintf(config_service_name,"/config_service_%d", jc.joint_number);
		//msg init
		const rosidl_message_type_support_t * config_support_service =
		  ROSIDL_GET_SRV_TYPE_SUPPORT(config_service, srv, ConfigService);

		//service init
		rcl_ret_t rc = rclc_service_init_default(
		  &config_service, &node,
		  config_support_service, config_service_name);
		rclc_executor_add_service(
		  &config_executor, &config_service, &request_msg,
		  &response_msg, config_service_callback);


		//Command Service
		//for some reason it require its own executor
		command_service_executor = rclc_executor_get_zero_initialized_executor();
		rclc_executor_init(&command_service_executor, &support.context, 1, &allocator);

		char command_service_name[17];
		sprintf(command_service_name,"/command_service_%d", jc.joint_number);
		//msg init
		const rosidl_message_type_support_t * command_service_support_service =
		  ROSIDL_GET_SRV_TYPE_SUPPORT(command_service, srv, CommandService);
		//service init
		rclc_service_init_default(
		  &command_service, &node,
		  command_service_support_service, command_service_name);
		rclc_executor_add_service(
		  &command_service_executor, &command_service, &command_service_request_msg,
		  &command_service_response_msg, command_service_callback);


		//Diagnostics service
		diag_service_executor = rclc_executor_get_zero_initialized_executor();
		rclc_executor_init(&diag_service_executor, &support.context, 1, &allocator);

		char diag_service_name[14];
		sprintf(diag_service_name,"/diag_service_%d", jc.joint_number);
		//msg init
		const rosidl_message_type_support_t * diag_service_support_service =
		  ROSIDL_GET_SRV_TYPE_SUPPORT(diag_service, srv, DiagService);
		//service init
		rclc_service_init_default(
		  &diag_service, &node,
		  diag_service_support_service, diag_service_name);
		rclc_executor_add_service(
		  &diag_service_executor, &diag_service, &diag_service_request_msg,
		  &diag_service_response_msg, diag_service_callback);


		//ALERT regulator
	for(;;)
	{
	if (read_fault < 5)
	{
	HAL_IWDG_Refresh(&hiwdg);
	}

	//some things to add

	vTaskDelay(50);
	}

  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartTask02 */
/**
* @brief Function implementing the quadEncoderRead thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask02 */
void StartTask02(void *argument)
{
  /* USER CODE BEGIN StartTask02 */
	//quadEncoderRead
  /* Infinite loop */
	for(;;)
	{
	  QuadEncData = __HAL_TIM_GET_COUNTER(&htim8);
	  osDelay(50);
	}
  /* USER CODE END StartTask02 */
}

/* USER CODE BEGIN Header_StartTask03 */
/**
* @brief Function implementing the absEncoderRead thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask03 */
void StartTask03(void *argument)
{
  /* USER CODE BEGIN StartTask03 */
	//absEncoderRead
  /* Infinite loop */
	for(;;)
	{
	as50_readAngle(&AbsEncAngle, 100);
	osDelay(50);
	}
  /* USER CODE END StartTask03 */
}

/* USER CODE BEGIN Header_StartTask04 */
/**
* @brief Function implementing the diagnosticPubli thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask04 */
void StartTask04(void *argument)
{
  /* USER CODE BEGIN StartTask04 */
	//configService
		  /* Infinite loop */
	for(;;)
	{
	rclc_executor_spin_some(&config_executor, 1);
	vTaskDelay(1000);
	}

  /* USER CODE END StartTask04 */
}

/* USER CODE BEGIN Header_StartTask06 */
/**
* @brief Function implementing the configPub thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask06 */
void StartTask06(void *argument)
{
  /* USER CODE BEGIN StartTask06 */
	//Command service
  /* Infinite loop */
  for(;;)
  {
	rclc_executor_spin_some(&command_service_executor, 1);
    osDelay(1000);
  }
  /* USER CODE END StartTask06 */
}

/* USER CODE BEGIN Header_StartTask09 */
/**
* @brief Function implementing the jcPublisher thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask09 */
void StartTask09(void *argument)
{
  /* USER CODE BEGIN StartTask09 */
	//jcPublisher
  /* Infinite loop */
  for(;;)
  {
	js_out.position.data[0] = angle; //kalman_angle;
	js_out.velocity.data[0] = vel;   //QuadEncData; //vel; //velocity_to_go;
	js_out.effort.data[0] = 0.0; //prev_effort;

	//osSemaphoreAcquire(PublisherSemaphoreHandle, 0);
	rcl_publish(&jc_publisher, &js_out, NULL);
	//osSemaphoreRelease(PublisherSemaphoreHandle);

    osDelay(1000);
  }
  /* USER CODE END StartTask09 */
}

/* USER CODE BEGIN Header_StartTask10 */
/**
* @brief Function implementing the jcSubscriber thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask10 */
void StartTask10(void *argument)
{
  /* USER CODE BEGIN StartTask10 */
	//jcSubscriber
  /* Infinite loop */
  for(;;)
  {
	rclc_executor_spin_some(&jc_executor, 1);
    osDelay(1000);
  }
  /* USER CODE END StartTask10 */
}

/* USER CODE BEGIN Header_StartTask05 */
/**
* @brief Function implementing the motorRead thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask05 */
void StartTask05(void *argument)
{
  /* USER CODE BEGIN StartTask05 */
  //Motor driver read
  /* Infinite loop */
  for(;;)
  {
	angle = steps_to_rads(tmc5160_position_read());
	vel = steps_to_rads(tmc5160_velocity_read());
    osDelay(20);
  }
  /* USER CODE END StartTask05 */
}

/* USER CODE BEGIN Header_StartTask01 */
/**
* @brief Function implementing the calibrateTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask01 */
void StartTask01(void *argument)
{
  /* USER CODE BEGIN StartTask01 */
	int8_t Kp = 20;
	uint32_t epsilon = 10;
	uint32_t deviation = 0;
	//Calibrate task
  /* Infinite loop */
  for(;;)
  {
	if (state_o_j.CALIBRATE)
	{
	deviation = jc.zero_enc - AbsEncAngle;
	tmc5160_move(deviation * Kp);
	if (deviation<epsilon)
	{
		tmc5160_stop();
		tmc5160_set_zero();
		state_o_j.CALIBRATE = 0;
	}
	}
    osDelay(10);
  }
  /* USER CODE END StartTask01 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void jc_sub_cb(const void * msg_input)
{
	// Cast messages to expected types
	const sensor_msgs__msg__JointState * js_in = (const sensor_msgs__msg__JointState *)msg_input;

	float effort = clamp_value_noref(0, js_in->effort.data[0] , motor_cfg.max_effort_by_default);
	float pos = clamp_value_noref(jc.lower_limit_ticks, rad_to_steps(js_in->position.data[0]), jc.upper_limit_ticks);

	tmc5160_effort(effort, jc.upper_limit_effort);
	tmc5160_velocity(rad_to_steps(js_in->velocity.data[0]));
	tmc5160_position(pos);
	state_o_j.RUN = 1;
	state_o_j.STOP = 0;
}


void config_service_callback(const void * request_msg, void * response_msg){
  // Cast messages to expected types
  config_service__srv__ConfigService_Request * req_in =
    (config_service__srv__ConfigService_Request *) request_msg;
  config_service__srv__ConfigService_Response * res_out =
    (config_service__srv__ConfigService_Response *) response_msg;

  if (req_in->config_set)
  {
	jc.upper_limit_enc = req_in->upper_limit_enc;
	jc.lower_limit_enc = req_in->lower_limit_enc;
	jc.upper_limit_ticks = req_in->upper_limit_ticks * req_in->gear_ratio;
	jc.lower_limit_ticks = req_in->lower_limit_ticks * req_in->gear_ratio;
	jc.domain_id = req_in->domain_id;
	jc.zero_enc = req_in->zero_enc;
	jc.full_steps = req_in->full_steps * req_in->gear_ratio;
	jc.gear_ratio = req_in->gear_ratio;
	jc.motor_type = req_in->motor_type;
	jc.direction = req_in->direction;
	jc.upper_limit_effort = req_in->upper_limit_effort;
	jc.joint_number = req_in->joint_number;
	jc.first_run = 7;

	joint_config_write(&jc);

	res_out->domain_id =jc.domain_id;
	res_out->full_steps=jc.full_steps;
	res_out->gear_ratio=jc.gear_ratio;
	res_out->joint_number=jc.joint_number;
	res_out->lower_limit_enc=jc.lower_limit_enc;
	res_out->lower_limit_ticks=jc.lower_limit_ticks;
	res_out->motor_type=jc.motor_type;
	res_out->upper_limit_effort=jc.upper_limit_effort;
	res_out->upper_limit_enc=jc.upper_limit_enc;
	res_out->upper_limit_ticks=jc.upper_limit_ticks;
	res_out->zero_enc=jc.zero_enc;
	res_out->direction = jc.direction;

	//NVIC_SystemReset();
  }
  else if (req_in->config_get)
  {
	  res_out->domain_id =jc.domain_id;
	  res_out->full_steps=jc.full_steps;
	  res_out->gear_ratio=jc.gear_ratio;
	  res_out->joint_number=jc.joint_number;
	  res_out->lower_limit_enc=jc.lower_limit_enc;
	  res_out->lower_limit_ticks=jc.lower_limit_ticks;
	  res_out->motor_type=jc.motor_type;
	  res_out->upper_limit_effort=jc.upper_limit_effort;
	  res_out->upper_limit_enc=jc.upper_limit_enc;
	  res_out->upper_limit_ticks=jc.upper_limit_ticks;
	  res_out->zero_enc=jc.zero_enc;
	  res_out->direction = jc.direction;
  }
}

void command_service_callback(const void * request_msg, void * response_msg)
{
  // Cast messages to expected type
	command_service__srv__CommandService_Request * req_in =
	(command_service__srv__CommandService_Request *) request_msg;
	command_service__srv__CommandService_Response * res_out =
	(command_service__srv__CommandService_Response *) response_msg;


	//TODO STATE MACHINE LOGIC SHOULD BE ADDED TO AVIOD INTERFERE

	if (req_in->enable_config == 0)
	{
		vTaskSuspend(configServiceHandle);
		//vTaskResume(jcPublisherHandle); //TODO
		vTaskResume(jcSubscriberHandle);
	}
	else if (req_in->enable_config)
	{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, 1);
		vTaskResume(configServiceHandle);
		//vTaskSuspend(jcPublisherHandle); //TODO to check why this is hanging MCU
		vTaskSuspend(jcSubscriberHandle);
	}
		if (req_in->set_zero)
		{
			tmc5160_set_zero();
		}
		if (req_in->arm)
		{
			tmc5160_arm();
		}
		if (req_in->disarm)
		{
			tmc5160_disarm();
		}
		if (req_in->calibrate)
		{
			state_o_j.CALIBRATE = 1;
			vTaskResume(calibrateTaskHandle);
		}
		else if(!req_in->calibrate)
		{
			state_o_j.CALIBRATE = 0;
			vTaskSuspend(calibrateTaskHandle);
		}
		if (req_in->get_position)
		{
			res_out->angle = AbsEncAngle;
			res_out->position_in_ticks = tmc5160_position_read();
		}
		if (req_in->stop)
		{
			tmc5160_stop();
		}
		if(req_in->position)
		{
			tmc5160_set_default_vel();
			tmc5160_position(req_in->position);
			res_out->success=1;
			res_out->position_in_ticks = req_in->position;
		}
		if(req_in->set_lower_limit)
		{
			jc.lower_limit_ticks = tmc5160_position_read();
			joint_config_write(&jc);
		}
		if(req_in->set_upper_limit)
		{
			jc.upper_limit_ticks = tmc5160_position_read();
			joint_config_write(&jc);
		}
}

void diag_service_callback(const void * request_msg, void * response_msg)
{
  // Cast messages to expected type
	diag_service__srv__DiagService_Request * req_in =
	(diag_service__srv__DiagService_Request *) request_msg;
	diag_service__srv__DiagService_Response * res_out =
	(diag_service__srv__DiagService_Response *) response_msg;
}


float steps_to_rads(int32_t steps)
{
	float rads = 0;
	rads = ((float)steps / (float)jc.full_steps) * (M_PI * 2);
	return rads;
}

int32_t rad_to_steps(float rads)
{
	int32_t steps = 0;
	steps = (int32_t)((jc.full_steps * rads)/(M_PI * 2));
	return steps;
}

/* USER CODE END Application */

