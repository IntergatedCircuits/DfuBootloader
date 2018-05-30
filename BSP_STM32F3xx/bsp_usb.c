/**
  ******************************************************************************
  * @file    bsp_usb.c
  * @author  Benedek Kupper
  * @version 0.1
  * @date    2018-05-21
  * @brief   DfuBootloader BSP for USB communication
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
#include <bsp_usb.h>
#include <bsp_io.h>

static void usbConnect(FunctionalState state)
{
    GPIO_vWritePin(USB_CONNECT_PIN, state);
}

void BSP_USB_Bind(USB_HandleType *dev)
{
    GPIO_vInitPin(USB_DM_PIN, USB_DM_CFG);
    GPIO_vInitPin(USB_DP_PIN, USB_DP_CFG);
    USB_INST2HANDLE(dev, USB);
    dev->Callbacks.ConnectCtrl = usbConnect;
}
