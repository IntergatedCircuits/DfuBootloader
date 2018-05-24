/**
  ******************************************************************************
  * @file    main.c
  * @author  Benedek Kupper
  * @version 0.1
  * @date    2018-05-21
  * @brief   DfuBootloader main function
  *
  * Copyright (c) 2018 Benedek Kupper
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */
#include <usbd_dfu.h>
#include <usbd.h>
#include <flash_if.h>

#include <xpd_flash.h>
#include <xpd_rcc.h>
#include <xpd_utils.h>

#include <bsp_system.h>
#include <bsp_usb.h>

const USBD_DescriptionType hdev_cfg = {
    .Vendor = {
            .Name           = "IntergatedCircuits",
            .ID             = USBD_VID,
    },
    .Product = {
            .Name           = "DFU Bootloader",
            .ID             = USBD_PID,
            .Version        = {{ 0,1 }},
    },
    .SerialNumber = (USBD_SerialNumberType*)DEVICE_ID_REG,
    .Config = {
            .Name           = "DFU Bootloader",
            .MaxCurrent_mA  = 100,
            .RemoteWakeup   = 0,
            .SelfPowered    = 0,
    },
}, *const dev_cfg = &hdev_cfg;

uint32_t SystemCoreClock;
USBD_HandleType husbd, *const usbd = &husbd;

USBD_DFU_IfHandleType __attribute__((section (".dfuSharedSection"))) hdfu_if;
USBD_DFU_IfHandleType *const dfu_if = &hdfu_if;

/**
 * @brief This function resets the device to known initial state,
 *        determines and starts the boot target (bootloader / application)
 * @return It doesn't
 */
int main(void)
{
    /* Reset hardware to default state */
    XPD_vDeinit();
    RCC_vDeinit();

    /* As the DFU interface is used by the application as well,
     * initialize it before going anywhere */
    USBD_DFU_BootInit(dfu_if, (USBD_DFU_RebootCbkType)NVIC_SystemReset, flash_if, 1);

    /* Enter App when DFU isn't requested and App is present */
    if (!USBD_DFU_IsRequested(dfu_if) && FlashIf_ApplicationPresent())
    {
        void (*runApplication)(void) = *((const void **)(FLASH_APP_ADDRESS + 4));

        /* Set the main stack pointer */
        __set_MSP(*((uint32_t *)FLASH_APP_ADDRESS));

        /* Jump to application */
        runApplication();
    }

    /* Otherwise start bootloader */
    XPD_vInit();

    FLASH_vPrefetchBuffer(ENABLE);

    BSP_USB_Bind(usbd);

    SystemClock_Config();

    /* Mount the interface to the device */
    USBD_DFU_MountInterface(dfu_if, usbd);

    /* Initialize the device */
    USBD_Init(usbd, dev_cfg);

    USBD_Connect(usbd);

    while(1)
    {
        /* No interrupts are used, USB events are handled from here */
        USB_vIRQHandler(usbd);
    }
}
