# lawicel-slcan

Partial Lawicel/SLCAN implementation at 1 Mb/s CAN speed and 460800 UART baud rate

## Required parts

STM32F103C6 devboard, like Blue Pill or similar. STMF107 can also be used.
FT232RL based USB-USART converter.
SN65HVD230 based CAN bus transceiver (breadboard for simplicity).

Second FT232 converter can be hooked up to USART3 for debugging (if required).

## Implementation details

The code is optimized to be small and fast. DMA is employed to copy CAN messages to processing buffer and printing out to USART1 for communication with the PC. Lookup tables are employed for converstion to/from ASCII HEX.

The limitation of 460800 baud rate on UART is due to implementation of using software UART only. I.e. hardware pins CTS/RTS are not employed, they would add reliability. Also shortening wires could help.

*CAN1 pins are remapped from default ones to PB9 and PB8.* If you don't need remaping, commend out that part in the code.

JTAG and SWD is fully disabled. If you wish to use that, see first lines in __main__.

Check [my other pile of STM32 libraries](https://github.com/darauble/STM32-ARM-Libs) for dependencies.

## Hooking it up

Connect UART adapter to PA10 and PA9, flash using native bootloader.

Connect CAN transceiver to PB9 and PB8.

## Setting CAN on Linux

Linux has __slcan__ daemon and kernel module, which sets up adapter. Then __ip__ can bring it up. So when code is compiled and wires connected, issue the following commands:

```
sudo slcand -S 460800 -t sw /dev/ttyUSB0 can0
sudo ip link set up can0
```

Try __candump can0__ and __cansend__ utilities.

## Implemented parts of Lawicel/SLCAN protocol

Implementation was done according to CAN232 description: http://www.can232.com/docs/can232_v3.pdf

To this moment I have only implemented partial slcan support: the one I needed quickly. So this boils down to:
- Sending and receiving data and remote request frames, CAN2A and CAN2B ('r', 'R', 't' and 'T' commands)
- Setting on timestamp reporting with 'Z' command
- Reporting version to 'V' command
- Reporting serial number to 'N' command

So that's basically it. CAN bus is open when device starts up and is hardcoded to 1 Mb/s. Change configuration value of the prescaler if another value is required and better don't mess with TQs.

