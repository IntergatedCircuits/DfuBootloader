/**
  ******************************************************************************
  * @file    flash_if.h
  * @author  Benedek Kupper
  * @version 0.1
  * @date    2018-05-21
  * @brief   DfuBootloader flash memory interface header
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
#ifndef __FLASH_IF_H_
#define __FLASH_IF_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <usbd_dfu.h>

#define FLASH_VALID_SYMBOL      ((uint32_t)0xCA11AB1E) /* Callable */

extern const USBD_DFU_AppType* const flash_if;

int FlashIf_ApplicationPresent(void);

#ifdef __cplusplus
}
#endif

#endif /* __FLASH_IF_H_ */
