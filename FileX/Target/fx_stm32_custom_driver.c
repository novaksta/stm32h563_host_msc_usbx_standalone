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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
// - ux_host_class_storage_media_get : get instance of UX_HOST_CLASS_STORAGE_MEDIA
// - ux_host_class_storage_media_lock : lock specific media for further read/write
// - ux_host_class_storage_media_read : read sectors on locked media
// - ux_host_class_storage_media_write : write sectors on locked media
// - ux_host_class_storage_media_unlock : unlock media

// extern UX_HOST_CLASS_STORAGE *storage;
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
  UX_HOST_CLASS_STORAGE           *storage;
  UX_HOST_CLASS_STORAGE_MEDIA     *storage_media;
  ULONG                           partition_start;

  storage_media = (UX_HOST_CLASS_STORAGE_MEDIA *) media -> fx_media_driver_info;
  storage = storage_media -> ux_host_class_storage_media_storage;
  partition_start = (ULONG) media -> fx_media_reserved_for_user;
  /* USER CODE END USER_CODE_SECTION_1 */

  switch (media_ptr->fx_media_driver_request)
  {
    case FX_DRIVER_INIT:
    {

     /* USER CODE BEGIN DRIVER_INIT */
    /* Poll status.  */
    _ux_host_class_storage_media_check(storage);
     /* USER CODE END DRIVER_INIT */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

     /* USER CODE BEGIN POST_DRIVER_INIT */

     /* USER CODE END POST_DRIVER_INIT */
      break;
    }

    case FX_DRIVER_UNINIT:
    {
     /* USER CODE BEGIN DRIVER_UNINIT */

     /* USER CODE END DRIVER_UNINIT */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_UNINIT */

     /* USER CODE END POST_DRIVER_UNINIT */
      break;
    }

    case FX_DRIVER_BOOT_READ:
    {
    /* USER CODE BEGIN DRIVER_BOOT_READ */
      if(storage==NULL){
        media_ptr->fx_media_driver_status =  FX_IO_ERROR;
        break;
      }
      /* the boot sector is the sector zero */
      status = ux_host_class_storage_media_read(storage ,0,media_ptr->fx_media_driver_sectors,media_ptr->fx_media_driver_buffer);

      if (status != UX_SUCCESS)
      {
        media_ptr->fx_media_driver_status = FX_IO_ERROR;
        break;
      }

      /* Check if the sector 0 is the actual boot sector, otherwise calculate the offset into it.
      Please note that this should belong to higher level of MW to do this check and it is here
      as a temporary work solution */

      partition_start =  0;

      status =  _fx_partition_offset_calculate(media_ptr -> fx_media_driver_buffer, 0, &partition_start, &partition_size);

      /* Check partition read error.  */
      if (status)
      {
        /* Unsuccessful driver request.  */
        media_ptr -> fx_media_driver_status =  FX_IO_ERROR;
        break;
      }

      /* Now determine if there is a partition...   */
      if (partition_start)
      {

        // if (check_sd_status(FX_STM32_SD_INSTANCE) != 0)
        // {
        //   media_ptr->fx_media_driver_status =  FX_IO_ERROR;
        //   break;
        // }

        /* Yes, now lets read the actual boot record.  */
        ux_host_class_storage_media_read(storage ,partition_start,media_ptr->fx_media_driver_sectors,media_ptr->fx_media_driver_buffer);

        if (status != FX_SUCCESS)
        {
          media_ptr->fx_media_driver_status = status;
          break;
        }
      }
     /* USER CODE END DRIVER_BOOT_READ */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_BOOT_READ */

     /* USER CODE END POST_DRIVER_BOOT_READ */
      break;
    }

    case FX_DRIVER_READ:
    {
    /* USER CODE BEGIN DRIVER_READ */
    // UINT  _ux_host_class_storage_media_read(UX_HOST_CLASS_STORAGE *storage, ULONG sector_start, ULONG sector_count, UCHAR *data_pointer)
      if (storage != NULL){
        ux_host_class_storage_media_read(storage ,media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors,media_ptr->fx_media_driver_sectors,media_ptr->fx_media_driver_buffer);

     /* USER CODE END DRIVER_READ */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_READ */
      }else{
          media_ptr->fx_media_driver_status = FX_IO_ERROR;
      }
     /* USER CODE END POST_DRIVER_READ */
      break;
    }

    case FX_DRIVER_BOOT_WRITE:
    {
    /* USER CODE BEGIN DRIVER_BOOT_WRITE */
    if (storage != NULL){
      // UINT  _ux_host_class_storage_media_write(UX_HOST_CLASS_STORAGE *storage, ULONG sector_start,  ULONG sector_count, UCHAR *data_pointer)
      ux_host_class_storage_media_write(storage,media_ptr->fx_media_driver_logical_sector + media_ptr->fx_media_hidden_sectors,media_ptr->fx_media_driver_sectors,media_ptr->fx_media_driver_buffer);
     /* USER CODE END DRIVER_BOOT_WRITE */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_BOOT_WRITE */
  }else{
    media_ptr->fx_media_driver_status = FX_IO_ERROR;
  }
     /* USER CODE END POST_DRIVER_BOOT_WRITE */
      break;
    }

    case FX_DRIVER_WRITE:
    {

    /* USER CODE BEGIN DRIVER_WRITE */
      if (storage != NULL){
        // UINT  _ux_host_class_storage_media_write(UX_HOST_CLASS_STORAGE *storage, ULONG sector_start,  ULONG sector_count, UCHAR *data_pointer)
        ux_host_class_storage_media_write(storage,0,media_ptr->fx_media_driver_sectors,media_ptr->fx_media_driver_buffer);
     /* USER CODE END DRIVER_WRITE */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

     /* USER CODE BEGIN POST_DRIVER_WRITE */
      }else{
        media_ptr->fx_media_driver_status = FX_IO_ERROR;
      }
     /* USER CODE END POST_DRIVER_WRITE */
      break;
    }

    case FX_DRIVER_FLUSH:
    {
    /* USER CODE BEGIN DRIVER_FLUSH */

     /* USER CODE END DRIVER_FLUSH */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_FLUSH */

     /* USER CODE END POST_DRIVER_FLUSH */
      break;
    }

    case FX_DRIVER_ABORT:
    {

    /* USER CODE BEGIN DRIVER_ABORT */

     /* USER CODE END DRIVER_ABORT */

      media_ptr->fx_media_driver_status = FX_SUCCESS;

    /* USER CODE BEGIN POST_DRIVER_ABORT */

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

