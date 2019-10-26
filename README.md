# STM32
Hands on STM32 for the first time, F1- and F4-serie will be included.

## 1) Environment setup:
- Official IDE: mdk;
- Supporting packages of certain series could be downloaded through "Package Installer" of IDE, e.g. "STM32F1xx_DFP";
- Whatever tool for downloading program, such as: flymcu, mcuisp...;
- Drivers on your OS, for e.g. CH340;

## 2) Work-flow:
- All the coding should be done in mdk, and then BUILD them into a .hex file;
- In the case of my STM32F103C8T6 board, firstly set the switcher of "BOOT0" to HIGH, it'd enable the downloading of program;
- Run the software tool, flymcu in my case, and choose the settings(COM port/using RamIsp/without RTS and DTR...);
- Finally, click the button and download the program into the board and check the result.
