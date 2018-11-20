# DFU Bootloader [![Build Status](https://travis-ci.org/IntergatedCircuits/DfuBootloader.svg?branch=master)](https://travis-ci.org/IntergatedCircuits/DfuBootloader)

This repository contains a generic USB device bootloader firmware for STM32 controllers.

## Features

* Fully-featured DFU class (with support to STMicroelectronics protocol Extension) USB device
* Small footprint: consumes only 8kB flash with `-Os` (50% smaller than the *STM32Cube* solution)
* The DFU interface is allocated to a fixed address, so it can be mounted on the application's USB device,
  allowing convenient entry to firmware update mode
* Easy to port on virtually any STM32 device that is supported by [STM32_XPD][STM32_XPD]

## How to build

Simply run `make` with the following arguments:

| Symbol Name        | Use
| :----------------- | :------------------------
| `SERIES`           | The series of the STM32 device
| `TARGET_HEADER`    | String of the device-specific *STM32_XPD* header
| `APP_ADDRESS`      | Value of the application's flash start address
| `APP_SIZE`         | The total available flash space for the application in bytes
| `TOTAL_ERASE_MS`   | Average erase time of the application flash area
| `VID`              | Vendor ID of the USB device in hexadecimal format (no prefix)
| `PID`              | Product ID of the USB device in hexadecimal format (no prefix)
| `DFUSE`            | Set to `1` if [DFUSE][DFUSE] protocol is desired
| `DESC_STR`         | DFUSE specific flash layout descriptor string
| `BINPATH`          | Path to build toolchain binaries (/usr/bin by default)

An example for an STM32F042K6 device with (VID,PID)={FFFF,FFFF}:

`make TARGET_HEADER="\<stm32f042x6.h\>" SERIES=STM32F0 APP_ADDRESS=0x08002000 APP_SIZE=24568 TOTAL_ERASE_MS=480 VID=FFFF PID=FFFF`

Further examples can be found in `.travis.yml`.

Built with GCC.
(arm-atollic-eabi- is preferred over arm-none-eabi- as it provides better size optimization)

[STM32_XPD]: https://github.com/IntergatedCircuits/STM32_XPD
[USBDevice]: https://github.com/IntergatedCircuits/USBDevice
[DFUSE]: www.st.com/resource/en/application_note/cd00264379.pdf
