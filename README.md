# An STM32F746G Discovery Network Stack
---
All code was created with inspiration from cpq/bare-metal-programming-guide  
but mostly from reading the STM32F746NG datasheet and STM32F746G Discovery  
Reference Manual.

## Hardware Used
STM32F746G Discovery with the STM32F746NGH6U microprocessor unit.  

## Structure
link.ld - the "linker script" that tell the linker where to put the binary compiled code on the MCU.  
main.c - the reset handler, which runs when the MCU boots, and initializes the network stack.  
lib - files with network stack code.  
board_config - MCU network peripheral configuration for right after reset pin is asserted.  

## Compiling and Using
`make`
### Flash to the Device - Must be Connected via ST-Link device
`make flash`

## Some Notes for Beginners/Learners
Here are some resources I used to learn about the various aspects of writing baremetal embedded device code:  
- Interrupt Blog by Memfault. Especially the Baremetal programming series.
- `ld` binutils documentation for in-depth explanation of Linker Scripts.
- cpq/bare-metal-programming-guide here on github for a quick and beginner friendly explanation of how to program on baremetal.
- STM32F74xxx Reference Manual from the official STM32 website.
- `make` documentation for in-depth explanation of Makefile syntax and use.
- Linux kernel source drivers/net for an idea of 
- The TCP/IP Guide online free edition for high-level understanding of each layer of the TCP/IP model and how they fit together.

## Why LGPLv2?
This project was made to help me understand the TCP/IP protocols and stack work.  
The project should be governed in the spirit or sharing for the sake of mutual learning.  
The LGPL allows you to link this library to your proprietary software but you can't make  
changes and hide them because that goes against the spirit and purpose of this repository.