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
#include <std_msgs/msg/string.h>


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
double steps_to_rads(int32_t steps);
int32_t rad_to_steps(double rads);

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
rclc_executor_t jc_executor;
rclc_support_t support;
rcl_allocator_t allocator;
//rcl_allocator_t new_allocator; //with custom memory allocators
rcl_node_t node;
rcl_init_options_t init_options;
rcl_ret_t rc;

std_msgs__msg__String msg;
std_msgs__msg__Int32 msg_out, msg_in;


extern uint32_t read_fault; //soft WD
extern IWDG_HandleTypeDef hiwdg; //hard WD


extern FDCAN_HandleTypeDef hfdcan1;
extern TIM_HandleTypeDef htim8;

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartTask09(void *argument);
void StartTask10(void *argument);

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

  /* creation of jcPublisher */
  jcPublisherHandle = osThreadNew(StartTask09, NULL, &jcPublisher_attributes);

  /* creation of jcSubscriber */
  jcSubscriberHandle = osThreadNew(StartTask10, NULL, &jcSubscriber_attributes);

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

		rcl_init_options_set_domain_id(&init_options, 0); //setting the domain ID

		rcl_init_options_init(&init_options, allocator);

		rmw_init_options_t* rmw_options = rcl_init_options_get_rmw_init_options(&init_options);

		uint32_t client_key;
		client_key = 10 + JOINTN;
		rmw_uros_options_set_client_key(client_key, rmw_options);

		rclc_support_init_with_options(&support, 0, NULL, &init_options, &allocator);

		//Control part
		char node_name[12];
		sprintf(node_name,"sonic_node_%d", JOINTN);

		rclc_node_init_default(&node, node_name, "", &support);

		char topic_pub_name[18];
		sprintf(topic_pub_name,"sonic_pub_%d", JOINTN);

		rclc_publisher_init_default(
			&jc_publisher,
			&node,
			ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Int32),
			topic_pub_name);

/*
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
*/


		//ALERT regulator
	for(;;)
	{
	if (read_fault < 25)
	{
	HAL_IWDG_Refresh(&hiwdg);
	}

	//some things to add

	vTaskDelay(100);
	}

  /* USER CODE END StartDefaultTask */
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
	int32_t i = 0;
  /* Infinite loop */
  for(;;)
  {

	msg_out.data = i;
	//osSemaphoreAcquire(PublisherSemaphoreHandle, 0);
	rcl_publish(&jc_publisher, &msg_out, NULL);
	//osSemaphoreRelease(PublisherSemaphoreHandle);
	i++;
    osDelay(1);
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
	//rclc_executor_spin_some(&jc_executor, 1);
    osDelay(10000);
  }
  /* USER CODE END StartTask10 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void jc_sub_cb(const void * msg_input)
{
	// Cast messages to expected types
	//const sensor_msgs__msg__JointState * js_in = (const sensor_msgs__msg__JointState *)msg_input;

}


/* USER CODE END Application */

