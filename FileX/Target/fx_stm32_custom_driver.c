/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

/* Private includes ----------------------------------------------------------*/
#include "fx_stm32_custom_driver.h"

/* USER CODE BEGIN Includes */
#include "app_usbx_host.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
extern UINT  _fx_partition_offset_calculate(void  *partition_sector, UINT partition,
        ULONG *partition_start, ULONG *partition_size);
extern UX_HOST_CLASS_STORAGE       *storage;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* USER CODE BEGIN USER_CODE_SECTION_0 */
/* USER CODE END USER_CODE_SECTION_0 */

VOID  fx_stm32_custom_driver(FX_MEDIA *media_ptr)
{
  /* USER CODE BEGIN USER_CODE_SECTION_1 */

	UINT                            status;
	//UX_HOST_CLASS_STORAGE           *storage;
	//UX_HOST_CLASS_STORAGE_MEDIA     *storage_media;
	ULONG                           partition_start;


	/* Get the pointers to the instances and partition start.  */
#if !defined(UX_HOST_CLASS_STORAGE_NO_FILEX)
	storage =  (UX_HOST_CLASS_STORAGE *) media_ptr -> fx_media_driver_info;
	storage_media =  (UX_HOST_CLASS_STORAGE_MEDIA *) media_ptr -> fx_media_reserved_for_user;
	partition_start = storage_media -> ux_host_class_storage_media_partition_start;
#else
	//storage_media = (UX_HOST_CLASS_STORAGE_MEDIA *) media_ptr -> fx_media_driver_info;
	//storage = storage_media -> ux_host_class_storage_media_storage;
	partition_start = (ULONG) media_ptr -> fx_media_reserved_for_user;
#endif

	/* Ensure the instance is valid.  */
	if ((storage -> ux_host_class_storage_state !=  UX_HOST_CLASS_INSTANCE_LIVE) &&
			(storage -> ux_host_class_storage_state !=  UX_HOST_CLASS_INSTANCE_MOUNTING))
	{

		/* Class instance is invalid. Return an error!  */
		media_ptr -> fx_media_driver_status =  FX_PTR_ERROR;
		return;
	}

#if defined(UX_HOST_CLASS_STORAGE_NO_FILEX)

	/* Ensure the media is valid.  */
	//if ((storage_media -> ux_host_class_storage_media_storage != storage) ||
	//		(storage_media -> ux_host_class_storage_media_status != UX_USED))
	//{

		/* Media instance is invalid.  */
		//media_ptr -> fx_media_driver_status =  FX_PTR_ERROR;
		//return;
	//}
#endif

	/* Protect Thread reentry to this instance.  */
	status = _ux_host_class_storage_lock(storage, UX_WAIT_FOREVER);
	if (status != UX_SUCCESS)
	{

		/* Unable to lock, return an error.  */
		media_ptr -> fx_media_driver_status =  FX_INVALID_STATE;
		return;
	}

	/* Restore the LUN number from the media instance.  */
	//storage -> ux_host_class_storage_lun =  storage_media -> ux_host_class_storage_media_lun;

	/* And the sector size.  */
	//storage -> ux_host_class_storage_sector_size =
	//		storage_media -> ux_host_class_storage_media_sector_size;

#if defined(UX_HOST_CLASS_STORAGE_NO_FILEX)

	/* Restore current used last sector number.  */
	//storage -> ux_host_class_storage_last_sector_number =
			//storage_media -> ux_host_class_storage_media_number_sectors - 1;
#endif

	/* Look at the request specified by the FileX caller.  */
	//first handle error state -- as there is no user code section in default
	if(media_ptr->fx_media_driver_request>8 || media_ptr->fx_media_driver_request<0){
		media_ptr->fx_media_driver_status = FX_IO_ERROR;
		_ux_host_class_storage_unlock(storage);
		return;
	}
  /* USER CODE END USER_CODE_SECTION_1 */

  switch (media_ptr->fx_media_driver_request)
  {
    case FX_DRIVER_INIT:
    {

     /* USER CODE BEGIN DRIVER_INIT */
#if defined(UX_HOST_STANDALONE)

		/* Poll status.  */
		_ux_host_class_storage_media_check(storage);
#endif

		/* Check for media protection.  We must do this operation here because FileX clears all the
                   media fields before init.  */
		if (storage -> ux_host_class_storage_write_protected_media ==  UX_TRUE)

			/* The media is Write Protected. We tell FileX.  */
			media_ptr -> fx_media_driver_write_protect = UX_TRUE;

		/* This function always succeeds.  */
     /* USER CODE END DRIVER_INIT */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

     /* USER CODE BEGIN POST_DRIVER_INIT */
		_ux_host_class_storage_unlock(storage);
     /* USER CODE END POST_DRIVER_INIT */
      break;
    }

    case FX_DRIVER_UNINIT:
    {
     /* USER CODE BEGIN DRIVER_UNINIT */

     /* USER CODE END DRIVER_UNINIT */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_UNINIT */
		_ux_host_class_storage_unlock(storage);
     /* USER CODE END POST_DRIVER_UNINIT */
      break;
    }

    case FX_DRIVER_BOOT_READ:
        {
        /* USER CODE BEGIN DRIVER_BOOT_READ */
    		/* Read the partition_start (0) sector.  */
    		status =  _ux_host_class_storage_media_read(storage,partition_start, 1, media_ptr -> fx_media_driver_buffer);
    		/* Decode sector content (get sector with FAT)  */
    		if(status == UX_SUCCESS){
    			media_ptr -> fx_media_driver_status = _fx_partition_offset_calculate(media_ptr -> fx_media_driver_buffer, 0, &(media_ptr -> fx_media_reserved_for_user), NULL);
    			/*check the error*/
    			if(media_ptr -> fx_media_driver_status != FX_SUCCESS){
            		_ux_host_class_storage_unlock(storage);
                    break;
    			}
    		}
    		/* Read the BOOT sector of FAT offset */
    		if(status == UX_SUCCESS) status = _ux_host_class_storage_media_read(storage, media_ptr -> fx_media_reserved_for_user, 1, media_ptr -> fx_media_driver_buffer);

    		/* Check completion status.  */
    		if (status == UX_SUCCESS)
         /* USER CODE END DRIVER_BOOT_READ */

          media_ptr->fx_media_driver_status = FX_SUCCESS;

        /* USER CODE BEGIN POST_DRIVER_BOOT_READ */
    		else
    		{

    #if defined(UX_HOST_STANDALONE)

    			/* Poll status.  */
    			_ux_host_class_storage_media_check(storage);
    #endif

    			media_ptr -> fx_media_driver_status =
    					_ux_host_class_storage_sense_code_translate(storage,status);
    		}
    		_ux_host_class_storage_unlock(storage);
         /* USER CODE END POST_DRIVER_BOOT_READ */
          break;
        }

    case FX_DRIVER_READ:
    {
    /* USER CODE BEGIN DRIVER_READ */
		/* Read one or more sectors.  */
		status =  _ux_host_class_storage_media_read(storage,
				media_ptr -> fx_media_driver_logical_sector + partition_start,
				media_ptr -> fx_media_driver_sectors,
				media_ptr -> fx_media_driver_buffer);

		/* Check completion status.  */
		if (status == UX_SUCCESS)
     /* USER CODE END DRIVER_READ */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_READ */
		else
		{

#if defined(UX_HOST_STANDALONE)

			/* Poll status.  */
			_ux_host_class_storage_media_check(storage);
#endif

			media_ptr -> fx_media_driver_status =
					_ux_host_class_storage_sense_code_translate(storage, status);
		}
		_ux_host_class_storage_unlock(storage);
     /* USER CODE END POST_DRIVER_READ */
      break;
    }

    case FX_DRIVER_BOOT_WRITE:
    {
    /* USER CODE BEGIN DRIVER_BOOT_WRITE */
		/* Write the boot sector.  */
		status =  _ux_host_class_storage_media_write(storage,
				partition_start, 1, media_ptr -> fx_media_driver_buffer);

		/* Check completion status.  */
		if (status == UX_SUCCESS)

     /* USER CODE END DRIVER_BOOT_WRITE */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_BOOT_WRITE */
		else
		{

#if defined(UX_HOST_STANDALONE)

			/* Poll status.  */
			_ux_host_class_storage_media_check(storage);
#endif

			media_ptr -> fx_media_driver_status =
					_ux_host_class_storage_sense_code_translate(storage,status);
		}
		_ux_host_class_storage_unlock(storage);
     /* USER CODE END POST_DRIVER_BOOT_WRITE */
      break;
    }

    case FX_DRIVER_WRITE:
    {

    /* USER CODE BEGIN DRIVER_WRITE */
		/* Write one or more sectors.  */
		status =  _ux_host_class_storage_media_write(storage,
				media_ptr -> fx_media_driver_logical_sector + partition_start,
				media_ptr -> fx_media_driver_sectors,
				media_ptr -> fx_media_driver_buffer);

		/* Check completion status.  */
		if (status == UX_SUCCESS)
     /* USER CODE END DRIVER_WRITE */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

     /* USER CODE BEGIN POST_DRIVER_WRITE */
		else
		{

#if defined(UX_HOST_STANDALONE)

			/* Poll status.  */
			_ux_host_class_storage_media_check(storage);
#endif

			media_ptr -> fx_media_driver_status =
					_ux_host_class_storage_sense_code_translate(storage,status);
		}
		_ux_host_class_storage_unlock(storage);
     /* USER CODE END POST_DRIVER_WRITE */
      break;
    }

    case FX_DRIVER_FLUSH:
    {
    /* USER CODE BEGIN DRIVER_FLUSH */
		/* Nothing to do. Just return a good status!  */
     /* USER CODE END DRIVER_FLUSH */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_FLUSH */
		_ux_host_class_storage_unlock(storage);
     /* USER CODE END POST_DRIVER_FLUSH */
      break;
    }

    case FX_DRIVER_ABORT:
    {

    /* USER CODE BEGIN DRIVER_ABORT */
		/* Nothing to do. Just return a good status!  */
     /* USER CODE END DRIVER_ABORT */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_ABORT */
		_ux_host_class_storage_unlock(storage);
     /* USER CODE END POST_DRIVER_ABORT */
      break;
    }

    default:
    {
        media_ptr->fx_media_driver_status = FX_IO_ERROR;
        break;
    }
  }
}

/* USER CODE BEGIN USER_CODE_SECTION_2 */

/* USER CODE END USER_CODE_SECTION_2 */

