/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    app_usbx_host.c
 * @author  MCD Application Team
 * @brief   USBX host applicative file
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
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
#include "app_usbx_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

uint8_t STORAGE_MEDIA =0;
UX_HOST_CLASS_STORAGE       *storage;

#ifndef USBH_UsrLog
#define USBH_UsrLog printf
#endif

#include "fx_stm32_custom_driver.h"
FX_MEDIA usb_disk= {};

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN UX_Host_Memory_Buffer */

/* USER CODE END UX_Host_Memory_Buffer */
#if defined ( __ICCARM__ )
#pragma data_alignment=4
#endif
__ALIGN_BEGIN static UCHAR ux_host_byte_pool_buffer[UX_HOST_APP_MEM_POOL_SIZE] __ALIGN_END;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS *current_class, VOID *current_instance);
static VOID ux_host_error_callback(UINT system_level, UINT system_context, UINT error_code);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
 * @brief  Application USBX Host Initialization.
 * @param  none
 * @retval status
 */
UINT MX_USBX_Host_Init(VOID)
{
	UINT ret = UX_SUCCESS;
	UCHAR *pointer;

	/* USER CODE BEGIN MX_USBX_Host_Init0 */

	/* USER CODE END MX_USBX_Host_Init0 */

	pointer = ux_host_byte_pool_buffer;
	/* Initialize USBX Memory */
	if (ux_system_initialize(pointer, USBX_HOST_MEMORY_STACK_SIZE, UX_NULL, 0) != UX_SUCCESS)
	{
		/* USER CODE BEGIN USBX_SYSTEM_INITIALIZE_ERROR */
		return UX_ERROR;
		/* USER CODE END USBX_SYSTEM_INITIALIZE_ERROR */
	}
	/* Install the host portion of USBX */
	if (ux_host_stack_initialize(ux_host_event_callback) != UX_SUCCESS)
	{
		/* USER CODE BEGIN USBX_HOST_INITIALIZE_ERROR */
		return UX_ERROR;
		/* USER CODE END USBX_HOST_INITIALIZE_ERROR */
	}

	/* Register a callback error function */
	ux_utility_error_callback_register(&ux_host_error_callback);

	/* Initialize the host storage class */
	if (ux_host_stack_class_register(_ux_system_host_class_storage_name,
			ux_host_class_storage_entry) != UX_SUCCESS)
	{
		/* USER CODE BEGIN USBX_HOST_STORAGE_REGISTER_ERROR */
		return UX_ERROR;
		/* USER CODE END USBX_HOST_STORAGE_REGISTER_ERROR */
	}

	/* USER CODE BEGIN MX_USBX_Host_Init1 */

	/* USER CODE END MX_USBX_Host_Init1 */

	return ret;
}

/**
 * @brief  _ux_utility_interrupt_disable
 *         USB utility interrupt disable.
 * @param  none
 * @retval none
 */
ALIGN_TYPE _ux_utility_interrupt_disable(VOID)
{
	/* USER CODE BEGIN _ux_utility_interrupt_disable */
	return(0);
	/* USER CODE END _ux_utility_interrupt_disable */
}

/**
 * @brief  _ux_utility_interrupt_restore
 *         USB utility interrupt restore.
 * @param  flags
 * @retval none
 */
VOID _ux_utility_interrupt_restore(ALIGN_TYPE flags)
{
	/* USER CODE BEGIN _ux_utility_interrupt_restore */
	UX_PARAMETER_NOT_USED(flags);
	/* USER CODE END _ux_utility_interrupt_restore */
}

/**
 * @brief  _ux_utility_time_get
 *         Get Time Tick for host timing.
 * @param  none
 * @retval time tick
 */
ULONG _ux_utility_time_get(VOID)
{
	ULONG time_tick = 0U;

	/* USER CODE BEGIN _ux_utility_time_get */
	time_tick = HAL_GetTick();
	/* USER CODE END _ux_utility_time_get */

	return time_tick;
}

/**
 * @brief  ux_host_event_callback
 *         This callback is invoked to notify application of instance changes.
 * @param  event: event code.
 * @param  current_class: Pointer to class.
 * @param  current_instance: Pointer to class instance.
 * @retval status
 */


UINT ux_host_event_callback(ULONG event, UX_HOST_CLASS *current_class, VOID *current_instance)
{
	UINT status = UX_SUCCESS;

	/* USER CODE BEGIN ux_host_event_callback0 */
	UX_PARAMETER_NOT_USED(current_class);
	UX_PARAMETER_NOT_USED(current_instance);
	/* USER CODE END ux_host_event_callback0 */

	switch (event)
	{
	case UX_DEVICE_INSERTION:

		/* USER CODE BEGIN UX_DEVICE_INSERTION */
		/* Get current Storage Class */
		if (current_class -> ux_host_class_entry_function == ux_host_class_storage_entry)
		{
			if (storage == UX_NULL)
			{
				/* Get current Storage Instance */
				storage = (UX_HOST_CLASS_STORAGE *)current_instance;

				USBH_UsrLog("\nUSB Mass Storage Device Plugged");
				USBH_UsrLog("PID: %#x ", (UINT)storage -> ux_host_class_storage_device -> ux_device_descriptor.idProduct);
				USBH_UsrLog("VID: %#x ", (UINT)storage -> ux_host_class_storage_device -> ux_device_descriptor.idVendor);
				USBH_UsrLog("\n");

				/* Get the storage media */
				//storage_media = (UX_HOST_CLASS_STORAGE_MEDIA *)current_class -> ux_host_class_media;

				if (((UX_HOST_CLASS_STORAGE_MEDIA *)current_class -> ux_host_class_media) -> ux_host_class_storage_media_lun != 0)
				{
					//storage_media = UX_NULL;
					storage = UX_NULL;
				}
				else
				{
					STORAGE_MEDIA = 1;

				}
			}
		}
		/* USER CODE END UX_DEVICE_INSERTION */

		break;

	case UX_DEVICE_REMOVAL:

		/* USER CODE BEGIN UX_DEVICE_REMOVAL */
		if ((VOID*)storage == current_instance)
		{
			/* Clear storage instance */
			storage = UX_NULL;
			USBH_UsrLog("\nUSB Mass Storage Device Unplugged\n");
		}
		/* USER CODE END UX_DEVICE_REMOVAL */

		break;

	case UX_DEVICE_CONNECTION:

		/* USER CODE BEGIN UX_DEVICE_CONNECTION */

		/* USER CODE END UX_DEVICE_CONNECTION */

		break;

	case UX_DEVICE_DISCONNECTION:

		/* USER CODE BEGIN UX_DEVICE_DISCONNECTION */

		/* USER CODE END UX_DEVICE_DISCONNECTION */

		break;

	default:

		/* USER CODE BEGIN EVENT_DEFAULT */

		/* USER CODE END EVENT_DEFAULT */

		break;
	}

	/* USER CODE BEGIN ux_host_event_callback1 */

	/* USER CODE END ux_host_event_callback1 */

	return status;
}

/**
 * @brief ux_host_error_callback
 *         This callback is invoked to notify application of error changes.
 * @param  system_level: system level parameter.
 * @param  system_context: system context code.
 * @param  error_code: error event code.
 * @retval Status
 */
VOID ux_host_error_callback(UINT system_level, UINT system_context, UINT error_code)
{
	/* USER CODE BEGIN ux_host_error_callback0 */
	UX_PARAMETER_NOT_USED(system_level);
	UX_PARAMETER_NOT_USED(system_context);
	/* USER CODE END ux_host_error_callback0 */

	switch (error_code)
	{
	case UX_DEVICE_ENUMERATION_FAILURE:

		/* USER CODE BEGIN UX_DEVICE_ENUMERATION_FAILURE */

		/* USER CODE END UX_DEVICE_ENUMERATION_FAILURE */

		break;

	case  UX_NO_DEVICE_CONNECTED:

		/* USER CODE BEGIN UX_NO_DEVICE_CONNECTED */

		/* USER CODE END UX_NO_DEVICE_CONNECTED */

		break;

	default:

		/* USER CODE BEGIN ERROR_DEFAULT */

		/* USER CODE END ERROR_DEFAULT */

		break;
	}

	/* USER CODE BEGIN ux_host_error_callback1 */

	/* USER CODE END ux_host_error_callback1 */
}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
