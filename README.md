# cm4-bootloader
```
+----------------------------------------------------------------------------------------------------------------+
|    _____  __  __  _  _      ____    ____    ____   _______  _        ____             _____   ______  _____    |
|   / ____||  \/  || || |    |  _ \  / __ \  / __ \ |__   __|| |      / __ \     /\    |  __ \ |  ____||  __ \   |
|  | |     | \  / || || |_   | |_) || |  | || |  | |   | |   | |     | |  | |   /  \   | |  | || |__   | |__) |  |
|  | |     | |\/| ||__   _|  | |_ < | |  | || |  | |   | |   | |     | |  | |  / /\ \  | |  | ||  __|  |  _  /   |
|  | |____ | |  | |   | |    | |_) || |__| || |__| |   | |   | |____ | |__| | /  __  \ | |__| || |____ | | \ \   |
|   \_____||_|  |_|   |_|    |____/  \____/  \____/    |_|   |______| \____/ /_/    \_\|_____/ |______||_|  \_\  |
|                                                                                                                |
+----------------------------------------------------------------------------------------------------------------+
```
An experimental bootloader implementation on ARM Cortex-M4 processor.
- Device: [NUCLEO-F303ZE](https://www.st.com/en/evaluation-tools/nucleo-f303ze.html)

## Build
`cm4-bootloader` dependents on some third-party packages to build code and burn into processor like [ARM GNU Toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain) and [stlink](https://github.com/stlink-org/stlink). Therefore, before using the project, we need to install these packages.

Install [ARM GNU Toolchain](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain):
```shell
$ sudo apt-get update
$ sudo apt-get -y install gcc-arm-none-eabi
```

Install [stlink](https://github.com/stlink-org/stlink):
```shell
$ sudo apt-get install git cmake libusb-1.0-0-dev
$ git clone https://github.com/stlink-org/stlink && cd stlink
$ cmake .
$ make
$ cd bin && sudo cp st-* /usr/local/bin
$ cd ../lib && sudo cp *.so* /lib32
$ cd ../config/udev/rules.d/ && sudo cp 49-stlinkv* /etc/udev/rules.d/
```

Install [OpenOCD](https://openocd.org/):
```shell
$ sudo apt install openocd
```

Build `cm4-bootloader`. It will compile bootloader and application respectively.
```shell
$ make
```

Burn the code into processor:
```shell
$ make upload
```

Execute the host program:
```shell
$ make host
```

## Debug
In `cm4-bootloader`, it uses [OpenOCD](https://openocd.org/) to open the debug mode of processor. We can use the following command:
```shell
$ make debug
```

After typing the command, the next step is opening gdb to connect to rhe openocd.
```shell
$ arm-none-eabi-gdb
```

Connect to the openocd:
```shell
$ target remote localhost:3333
```

## Memory layout
In `cm4-bootloader`, the memory layout is shown as follows:
```
+--------------+ 0x08000000
|              |
|  Bootloader  |
|              |
+--------------+ 0x08004000
|              |
|  Application |
|              |
+--------------+
```

## Bootloader commands
The following shows all commands provided by `cm4-bootloader`. It refers to [USART protocol used in the STM32 bootloader](https://www.st.com/resource/en/application_note/an3155-usart-protocol-used-in-the-stm32-bootloader-stmicroelectronics.pdf).

| command              | code | description |
| -------------------- | ---- | ----------- |
| BL_GET_CMD           | 0x00 | Get the version and the allowed commands supported by the current version of the protocol |
| BL_GET_VERSION       | 0x01 | Get the protocol version |
| BL_GET_ID            | 0x02 | Get the chip ID |
| BL_GET_PROTECT_LEVEL | 0x03 | Get the protection level status |
| BL_READ_MEM          | 0x11 | Read up to 256 bytes of memory starting from an address specified by the application |
| BL_JUMP_TO_APP       | 0x21 | Jump to user application code located in the internal flash memory or in the SRAM |
| BL_WRITE_MEM         | 0x31 | Write a binary file data to flash |
| BL_ERASE_MEM         | 0x43 | Erase from one to all the flash memory pages |
| BL_WRITE_PROTECT     | 0x63 | Enable the write protection for some sectors |
| BL_WRITE_UNPROTECT   | 0x73 | Disable the write protection for all flash memory sectors |
| BL_READ_PROTECT      | 0x82 | Enable the read protection |
| BL_READ_UNPROTECT    | 0x92 | Disable the read protection |
| BL_RELOAD_OPT_BYTES  | 0xA1 | Reload option bytes |

However, there are still some commands that have not been implemented yet.
- Done
  - BL_GET_CMD
  - BL_GET_VERSION
  - BL_GET_ID
  - BL_GET_PROTECT_LEVEL
  - BL_READ_MEM
  - BL_JUMP_TO_APP
  - BL_WRITE_MEM
  - BL_ERASE_MEM
  - BL_WRITE_PROTECT
  - BL_WRITE_UNPROTECT

- Not done
  - BL_READ_PROTECT
  - BL_READ_UNPROTECT
  - BL_RELOAD_OPT_BYTES

## Communicate with host
`cm4-bootloader` uses USART to send and receive data. The following picture shows how bootloader to communicate with host.
1. Host sends the command package to bootloader.
2. Bootloader receives the package and reply ACK or NACK to host based on cyclic redundancy check (CRC). If bootloader sends ACK, then it will send the length of reply data.
3. Finally, bootloader sends the reply data if it sends ACK at previous step. That is, the communication will stop if bootloader sends NACK.
```
 host                       bootloader
   |                             |
   |     1. command package      |
   | --------------------------> |
   |                             |
   |         2. ACK/NACK         |
   | <-------------------------- |
   |                             |
   |        3. reply data        |
   | <-------------------------- |
   |                             |
```

### Command package format
The following picture shows the command package format.
1. Command code defined at [bootloader.c](./bl/bootloader.c) uses to check the current command and its size is 1 byte.
2. Buffer size means the size of buffer and its size is 1 byte.
3. CRC data calculated from the previous data (command code + buffer size + buffer). It used to check the correctness of command and its size is 4 byte.
```
+--------------+-------------+--------+----------+
| command code | buffer size | buffer | CRC data |
+--------------+-------------+--------+----------+
```

#### Read memory command format
The following picture shows the command format of memory reading.
1. Buffer is composed of a combination of "base address" and "length of data".
2. Base addrress means the first address to read. (4 bytes)
3. Length of data means the number of data to read. (Up to 256 bytes)
```
+--------------+-------------+--------------+----------------+----------+
| command code | buffer size | base address | length of data | CRC data |
+--------------+-------------+--------------+----------------+----------+
```

#### Write memory command format
The following picture shows the command format of memory writing.
1. Buffer is composed of a combination of "base address", "payload length" and "payload".
2. Base addrress means the first address to write. (4 bytes)
3. Payload length means the length of payload. (1 byte)
4. Payload means the data to write to flash. (Up to 250 bytes)
```
+--------------+-------------+--------------+----------------+---------+----------+
| command code | buffer size | base address | payload length | payload | CRC data |
+--------------+-------------+--------------+----------------+---------+----------+
```

#### Erase memory command format
The following picture shows the command format of memory erasing.
1. Buffer is composed of a combination of "page number" and "number of page".
2. Both of "page number" and "number of page" are 1 byte. That is, buffer size is 2.
```
+--------------+-------------+-------------+----------------+----------+
| command code | buffer size | page number | number of page | CRC data |
+--------------+-------------+-------------+----------------+----------+
```

#### Write protection option bytes command format
Enable the write protection for some sectors
- One bit of the user option bytes is used to protect 2 pages of 2 Kbytes in
  the main memory block.
- Page 62 ~ 255 is protected by 1 bit.
```
+--------------+-------------+----------------+--------------+----------+
| command code | buffer size | number of page | page numbers | CRC data |
+--------------+-------------+----------------+--------------+----------+
```

#### Write unprotection option bytes command format
Disable the write protection for all flash memory sectors
```
+--------------+-------------+----------+
| command code | buffer size | CRC data |
+--------------+-------------+----------+
```

## Reference
- [How to write a bootloader from scratch](https://interrupt.memfault.com/blog/how-to-write-a-bootloader-from-scratch)
- [USART protocol used in the STM32 bootloader](https://www.st.com/resource/en/application_note/an3155-usart-protocol-used-in-the-stm32-bootloader-stmicroelectronics.pdf)
- [STM32F303xB/C/D/E, STM32F303x6/8, STM32F328x8, STM32F358xC, STM32F398xE Reference manual](https://www.st.com/resource/en/reference_manual/dm00043574-stm32f303xb-c-d-e-stm32f303x6-8-stm32f328x8-stm32f358xc-stm32f398xe-advanced-arm-based-mcus-stmicroelectronics.pdf)
- [Cortex-M4 Technical Reference Manual](https://developer.arm.com/documentation/100166/0001/)
- [ARM® v7-M Architecture Reference Manual](https://developer.arm.com/documentation/ddi0403/latest/)
