/**
  ******************************************************************************
  * @file    flash_if.c
  * @author  Benedek Kupper
  * @version 0.1
  * @date    2018-05-21
  * @brief   DfuBootloader flash memory interface
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
#include <flash_if.h>
#include <xpd_flash.h>

static const struct
{
    uint32_t dummy; /* Write double word for STM32L4 support */
    uint32_t symbol;
}manifSign = { 0xFFFFFFFF, FLASH_VALID_SYMBOL };

#if (USBD_DFU_ST_EXTENSION != 0)
/**
 * @brief DFUSE layout descriptor strings
 *
 * DFUSE string descriptors structure:
 * @Name /StartAddress/Count*SizeProperty,Count*SizeProperty
 *
 * Property options:
 * @arg a: R
 * @arg g: REW
 * @arg e: RW
 *
 * Example strings:
 * @arg "@Internal Flash /0x08000000/4*2Ka,12*2Kg"
 * @arg "@OTP Memory /0x1FFF7800/1*512e,1*16e"
 * @arg "@Option Bytes /0x1FFFC000/1*16e"
 * @arg "@Device Feature /0xFFFF0000/1*4e"
 */

/**
 * @brief Erase one block at the specified address.
 * @param addr: start address of the block to erase
 * @return Result of the flash erase operation
 */
static USBD_DFU_StatusType SE_FlashIf_Erase(uint8_t *addr)
{
    /* Erase flash memory block from the start address */
    return (FLASH_eErase(addr, 1) == XPD_OK) ?
            DFU_ERROR_NONE : DFU_ERROR_ERASE;
}

/**
 * @brief Get approximating timeout for the upcoming flash operation.
 * @param addr: start address of the following operation
 * @param len: (byte) length of the operation
 * @return Expected time of next operation
 */
static uint16_t SE_FlashIf_GetTimeout_ms(uint8_t *addr, uint32_t len)
{
    if (len == 5) /* Erase one block */
    {
        return FLASH_ERASE_TIME_ms;
    }
    else /* Write single block */
    {
        return ((FLASH_BYTE_PROGRAM_TIME_us * len) + 1000) / 1000;
    }
}

#else
#define FLASH_ERASE_SIZE_kB       /* Entire application (in kB) */\
    (DEVICE_FLASH_SIZE_kB - ((FLASH_APP_ADDRESS - FLASH_BASE) >> 10))

/**
 * @brief Erase entire application firmware
 * @param addr: start address of application
 * @return Result of the flash erase operation
 */
static USBD_DFU_StatusType FlashIf_Erase(uint8_t *addr)
{
    return (FLASH_eErase(addr, FLASH_ERASE_SIZE_kB) == XPD_OK) ?
            DFU_ERROR_NONE : DFU_ERROR_ERASE;
}

/**
 * @brief Get approximating timeout for the upcoming flash operation(s).
 * @param addr: start address of the following operation(s)
 * @param len: (byte) length of the operation(s)
 * @return Expected time of next operation(s)
 */
static uint16_t FlashIf_GetTimeout_ms(uint8_t *addr, uint32_t len)
{
    /* Erase whole FW, write first block */
    if (addr == (uint8_t*)FLASH_APP_ADDRESS)
    {
        return FLASH_TOTAL_ERASE_TIME_ms + FLASH_BYTE_PROGRAM_TIME_us;
    }
#if 0
    /* Manifestation */
    else if (!(addr < ((uint8_t*)FLASH_APP_ADDRESS + FLASH_APP_SIZE)))
    {
        return ((FLASH_BYTE_PROGRAM_TIME_us * len) + 1000) / 1000;
    }
#endif
    /* Write single block */
    else
    {
        return ((FLASH_BYTE_PROGRAM_TIME_us * len) + 1000) / 1000;
    }
}
#endif

/**
 * @brief Writes the passed data to the specified flash address.
 * @param addr: target address to write to
 * @param data: flash contents to write
 * @param len: amount of bytes to write
 * @return Result of the flash program operation
 */
static USBD_DFU_StatusType FlashIf_Write(uint8_t *addr, uint8_t *data, uint32_t len)
{
    return (FLASH_eProgram(addr, data, len) == XPD_OK) ?
            DFU_ERROR_NONE : DFU_ERROR_WRITE;
}

/**
 * @brief Reads from the flash memory.
 * @param addr: source pointer
 * @param data: destination pointer
 * @param len: amount of bytes to read
 */
static void FlashIf_Read(uint8_t *addr, uint8_t *data, uint32_t len)
{
    while (len-- > 0)
        *data++ = *addr++;
}

/**
 * @brief Check if the application header is correct
 * (word 0 = end of stack, word 1 = reset vector).
 * @return TRUE if application header is valid, FALSE otherwise
 */
static int FlashIf_AppHeaderValid(void)
{
    /* End of stack is the first word of the application,
     * it has a limited valid range */
    uint32_t stackEnd    = *( (uint32_t*)FLASH_APP_ADDRESS);
    /* The reset vector should point to a valid flash area */
    uint32_t resetVector = *(((uint32_t*)FLASH_APP_ADDRESS) + 1);

    return (((stackEnd & 0xFFF007FF) == 0x20000000) &&
            (resetVector > (FLASH_APP_ADDRESS)) &&
            (resetVector < (FLASH_BASE + (DEVICE_FLASH_SIZE_kB << 10) - sizeof(manifSign))));
}

/**
 * @brief Writes a validity signature to the end of the flash to indicate upgrade success.
 * @return Result of the flash program operation
 */
static USBD_DFU_StatusType FlashIf_Manifest(void)
{
    USBD_DFU_StatusType retval = DFU_ERROR_NONE;
    uint32_t* addr = (uint32_t*)(FLASH_BASE
            + (DEVICE_FLASH_SIZE_kB << 10) - sizeof(manifSign));

    if (!FlashIf_AppHeaderValid())
    {
        retval = DFU_ERROR_VERIFY;
    }
    else if (XPD_OK != FLASH_eProgram(addr,
            (const uint8_t*)&manifSign, sizeof(manifSign)))
    {
        retval = DFU_ERROR_PROG;
    }
    return retval;
}

/**
 * @brief Determines if the application is present in the flash medium.
 * @return TRUE if application is available for execution, FALSE otherwise
 */
int FlashIf_ApplicationPresent(void)
{
    /* The last word of the application is set to fixed value during manifestation */
    uint32_t* flashEnd = ((uint32_t*)(FLASH_BASE
            + (DEVICE_FLASH_SIZE_kB << 10) - sizeof(FLASH_VALID_SYMBOL)));

    return (FlashIf_AppHeaderValid() &&
            (*flashEnd == FLASH_VALID_SYMBOL));
}

const USBD_DFU_AppType hflash_if = {
    .Firmware.Address   = FLASH_APP_ADDRESS,
    .Firmware.TotalSize = FLASH_APP_SIZE,

    .Init               = FLASH_vUnlock,
    .Deinit             = FLASH_vLock,
    .Write              = FlashIf_Write,
    .Read               = FlashIf_Read,
    .Manifest           = FlashIf_Manifest,

#if (USBD_DFU_ST_EXTENSION != 0)
    .Erase              = SE_FlashIf_Erase,
    .GetTimeout_ms      = SE_FlashIf_GetTimeout_ms,
    .Name               = SE_FLASH_DESC_STR,
#else
    .Erase              = FlashIf_Erase,
    .GetTimeout_ms      = FlashIf_GetTimeout_ms,
    .Name               = "Device Flash DFU",
#endif
}, *const flash_if = &hflash_if;
