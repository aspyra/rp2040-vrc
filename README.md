# RP2040 based RC-to-USB adapter

This project is an adapter allowing use of an RC transmitter as PC input device, i.e. to play racing games.\
It reads the PWM signal from 2-4ch RC receiver and presents it to the PC as joystick inputs.\
The signal precision is ±0.0833μs:
- 0-12000 range for standard 1-2ms signal
- 0-24000 range for extended 0.5-2.5ms signal
- 0-4800 for SSR 100-500us signal

## How to program / update
Use one of the [precompiled binaries](https://github.com/aspyra/rp2040_vrc/tree/develop/precompiled_binaries).\
While holding the boot select button on your board, plug it into your PC and simply drag the binary into the newly mounted *RPI-RP2* flash drive.\
<img src="https://github.com/aspyra/rp2040_vrc/blob/develop/pictures/programming/programming_1.gif" alt="programming GIF" style="width: 320px;"/>
<img src="https://github.com/aspyra/rp2040_vrc/blob/develop/pictures/programming/programming_2.gif" alt="programming GIF" style="width: 480px;"/>

## How to use

### First use - learning PWM ranges
- Connect the adapter to your RC receiver and PC.
- Reset your trim and D/R settings on the transmitter.
- Press the boot button for >2 seconds, until the LED turns blue.
- Move your steering/throttle/AUX to minimum, maximum, then leave it in the middle.
- Press the boot button shortly (<2s) to save the new signal ranges.
Not moving the inputs before saving will load default range (1-2ms).

### Normal use
- Connect the adapter to your RC receiver and PC.
- Press the boot button shortly (<2s) to advance to next game. The game number is flashed with the LED.
If the LED flashes red, then no signal is being read from the receiver. 

## List of modes

### 1. VRC (universal)
This mode simply translates each of the 4 channels into joystick axis.

### 2. Circuit Superstars (split throttle and brake)
This mode splits throttle input into separate throttle and brake axis. Steering is translated normally.

## Hardware
It uses widely available Waveshare RP2040-Zero development board and a couple of servo/jumper wires.\
Note that the RP2040 inputs are not 5V tolerant, but all the RC receivers I tested had a 3.3V push-pull output.

Hardware preparation:\
<img src="https://github.com/aspyra/rp2040_vrc/blob/develop/pictures/hardware/hardware_v1_1.jpg" alt="hardware example" style="width: 400px;"/>
<img src="https://github.com/aspyra/rp2040_vrc/blob/develop/pictures/hardware/hardware_v1_2.jpg" alt="hardware example" style="width: 400px;"/>\
<img src="https://github.com/aspyra/rp2040_vrc/blob/develop/pictures/hardware/hardware_v1_3.jpg" alt="hardware example" style="width: 400px;"/>
<img src="https://github.com/aspyra/rp2040_vrc/blob/develop/pictures/hardware/hardware_v1_4.jpg" alt="hardware example" style="width: 400px;"/>\
<img src="https://github.com/aspyra/rp2040_vrc/blob/develop/pictures/hardware/hardware_v1_5.jpg" alt="hardware example" style="width: 400px;"/>
<img src="https://github.com/aspyra/rp2040_vrc/blob/develop/pictures/hardware/hardware_v1_6.jpg" alt="hardware example" style="width: 400px;"/>\
<img src="https://github.com/aspyra/rp2040_vrc/blob/develop/pictures/hardware/hardware_v1_7.jpg" alt="hardware example" style="width: 400px;"/>
<img src="https://github.com/aspyra/rp2040_vrc/blob/develop/pictures/hardware/hardware_v1_8.jpg" alt="hardware example" style="width: 400px;"/>

## OPTIONAL: How to compile
Uses:\
https://github.com/earlephilhower/arduino-pico release 4.3.1\
https://github.com/adafruit/Adafruit_TinyUSB_Arduino release 3.4.1\
https://github.com/FastLED/FastLED release 3.9.8

Compiled in Arduino IDE 2.3.4\
<img src="https://github.com/aspyra/rp2040_vrc/blob/develop/pictures/compiling/compile_settings.PNG" alt="compiler options"/>
