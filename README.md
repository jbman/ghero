# ghero

## Description

Ghero is an Arduino library for using a Wii Guitar Hero controller as music controller.

The library calls dedicated functions when a button is pressed, the strum bar is pushed or if the whammy bar is used. 
Use it to play a sound from a speaker or send notes to a MIDI out jack.

## Examples 

Find example sketches at folder [examples](examples):

* [ghero-example](examples/ghero-example): Simple example which can be used without any additonal hardware connected. Prints to serial output when buttons are pressed
* [ghero-speaker](examples/ghero-speaker): Output of squareqve sound through a speaker
* [ghero-midi](examples/ghero-midi): Output of MIDI notes when buttons are pressed
* [ghero-develop](examples/ghero-develop): Output of bytes sent by the controller.

## Hardware

* A Wii Guitar Hero (GH) Controller. I used the fantastic white GH controller from Logitech.
* An Arduino nano with Atmega 328 chip or other Arduino
* A WiiChuck Nunchuck Adapter to connect the Arduino to the GH Controller
* Optional: A Speaker for driect sound output
* Optional: A MIDI Jack

## Devlopment

Visual Studio Code with Arduino extension was used for development.

## License

MIT License, check license.txt for more information
All text above must be included in any redistribution.
