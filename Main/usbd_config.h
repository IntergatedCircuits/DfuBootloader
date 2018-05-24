/**
  ******************************************************************************
  * @file    usbd_config.h
  * @author  Benedek Kupper
  * @version 0.1
  * @date    2018-05-21
  * @brief   DfuBootloader USB Device library configuration
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
#ifndef __USBD_CONFIG_H_
#define __USBD_CONFIG_H_

#ifdef __cplusplus
extern "C"
{
#endif

/** @addtogroup USBD
 * @{ */

/** @addtogroup USBD_Exported_Macros USBD Exported Macros
 * @{ */

#define USBD_MAX_IF_COUNT           8 /* TODO must always match with application's setting */

#define USBD_EP0_BUFFER_SIZE        1024

#define USBD_DFU_ALTSETTINGS        0

#define USBD_DFU_MANIFEST_TOLERANT  0

/** @} */

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CONFIG_H_ */
