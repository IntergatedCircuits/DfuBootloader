/**
  ******************************************************************************
  * @file    bsp_io.h
  * @author  Benedek Kupper
  * @version 0.1
  * @date    2018-05-21
  * @brief   DfuBootloader BSP for I/O pins
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
#ifndef __BSP_IO_H_
#define __BSP_IO_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <xpd_gpio.h>

#define USB_DP_PIN          PA12
#define USB_DM_PIN          PA11
#define USB_DP_CFG          (&BSP_IOCfg[0])
#define USB_DM_CFG          (&BSP_IOCfg[0])

extern const GPIO_InitType BSP_IOCfg[];

#ifdef __cplusplus
}
#endif

#endif /* __BSP_IO_H_ */
