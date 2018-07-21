# DFU Bootloader

This repository contains a generic USB device bootloader firmware for STM32 controllers.

## Features

* Fully-featured DFU class (with support to STMicroelectronics protocol Extension) USB device
* Small footprint: consumes only 8kB flash with `-Os` (50% smaller than the *STM32Cube* solution)
* The DFU interface is allocated to a fixed address, so it can be mounted on the application's USB device,
  allowing convenient entry to firmware update mode
* Easy to port on virtually any STM32 device that is supported by [STM32_XPD][STM32_XPD]

## Necessary build defines

| Symbol Name                     | [DFUSE][DFUSE] only | Use
| :------------------------------ | :-: | :------------------------
| `STM32_TARGET_HEADER`           |     | String of the device-specific *STM32_XPD* header, e.g. `"stm32f042x6.h"`
| `FLASH_APP_ADDRESS`             |     | Value of the application's flash start address
| `FLASH_APP_SIZE`                |     | The total available flash space for the application in bytes
| `FLASH_BYTE_PROGRAM_TIME_us`    |     | Average flash programming time of a byte
| `FLASH_ERASE_TIME_ms`           | x   | Average flash block erase time
| `FLASH_TOTAL_ERASE_TIME_ms`     |     | Average erase time of the application flash area
| `USBD_VID`                      |     | Vendor ID of the USB device
| `USBD_PID`                      |     | Product ID of the USB device
| `USBD_DFU_ST_EXTENSION`         | x   | Set to `1` if [DFUSE][DFUSE] protocol is desired 
| `VDD_VALUE_mV`                  |     | Power supply voltage value in mV
| `SE_FLASH_DESC_STR`             | x   | DFUSE specific flash layout descriptor string

Built with GCC ARM tools.

[STM32_XPD]: https://github.com/IntergatedCircuits/STM32_XPD
[USBDevice]: https://github.com/IntergatedCircuits/USBDevice
[DFUSE]: www.st.com/resource/en/application_note/cd00264379.pdf