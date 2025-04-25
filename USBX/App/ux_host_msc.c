/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_host_msc.c
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
#include "ux_host_msc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USER CODE BEGIN 1 */

#include "fx_api.h"
#include "app_filex.h"

#ifndef USBH_UsrLog
#define USBH_UsrLog printf
#endif

#ifndef USBH_ErrLog
#define USBH_ErrLog printf
#endif

extern FX_MEDIA                    *media;

/**
  * @brief  Function implementing msc_process_thread_entry.
  * @param  thread_input: Not used
  * @retval none
  */
void msc_process(){
	if (media != NULL)
	    {
	      /* Start File operations */
	      USBH_UsrLog("\n*** Start Files operations ***\n");

	      /* Create a file and check status */
	      if (App_File_Create(media) == UX_SUCCESS)
	      {
	        USBH_UsrLog("File TEST.TXT Created \n");

	        /* Start write File Operation */
	        USBH_UsrLog("Write Process ...... \n");

	        /* Check status */
	        if (App_File_Write(media) == UX_SUCCESS)
	        {
	          USBH_UsrLog("Write Process Success \n");

	          /* Start Read File Operation and comparison operation */
	          USBH_UsrLog("Read Process  ...... \n");

	          /* Check Read Operation */
	          if (App_File_Read(media) == UX_SUCCESS)
	          {
	            USBH_UsrLog("Read Process Success  \n");
	            USBH_UsrLog("File Closed \n");
	  			fx_media_close(media);
		        USBH_UsrLog("Media Closed \n");
	            USBH_UsrLog("*** End Files operations ***\n");
	          }
	          else
	          {
	            USBH_ErrLog("!! Read Process Fail !! \n");
	          }
	        }
	        else
	        {
	          USBH_ErrLog("!! Write Process Fail !! ");
	        }

	      }
	      else
	      {
	        USBH_ErrLog(" !! Could Not Create TEST.TXT File !! ");
	      }
	    }
}
/* USER CODE END 1 */
