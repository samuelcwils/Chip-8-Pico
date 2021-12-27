# Chip-8-Pico
This project plays Chip-8 games on the Raspberry Pi Pico. It outputs VGA.
It is preloaded with breakout.
![PXL_20211227_181348153_2 (1)](https://user-images.githubusercontent.com/92551719/147497820-0bcff9c3-be79-4b2e-a071-996bf2675fee.jpg)
![PXL_20211227_181808229 MP](https://user-images.githubusercontent.com/92551719/147497825-cd5a218e-8748-4d90-a6ba-3417688b4512.jpg)

# How to build
Clone this repositry into Panda381's PicoVGA SDK. https://github.com/Panda381/PicoVGA

Then run the c.bat file inside the Chip-8 folder. Drag and drop the UF2 file into your pico.

Notepad++ can be used to run .bat files on linux.

# How to add games
Use a program like srecord to convert a chip8 file into a c array. Then include it into the source and the makefile.

Download: 
https://sourceforge.net/projects/srecord/

Using srecord:
https://developer.arm.com/documentation/ka004285/latest

# VGA output
All wiring an documentation can be found on:

http://www.breatharian.eu/hw/picovga/index_en.html

and

https://github.com/Panda381/PicoVGA

# Note
I have not setup all inputs. GPIO 16 and 17 are used for 4 and 6 on the keypad.
