/*
    ghero.h - Library for connecting a Wii Guitar Hero controller to the Arduino

    This library decodes the data send by a Wii Guitar Hero controller. 
    It was developed with a Logitech Guitar Hero controller. It may also work for other 
    Wii guitar hero controllers, but it isn't tested.

    The guitar is connected to the arduino with the help of a small passive adapter ("nunchuk adapter für arduino").
    The relevant output pins of this adapter are '+', '-', 'd' and 'c':
    - is connected to GND on arduino
    + is connected to +3,3V on arduino
    d is connected to SDA pin (A4 on arduino nano)
    c is connected to SCL pin (A5 on arduino nano)

    Created by Johannes Bergmann,
    MIT license, all text above must be included in any redistribution
*/
#ifndef ghero_h
#define ghero_h

#include "Arduino.h"

// The state of the guitar. 
// Provides information which button is pressed and the position of the whammy bar and joystick.
class State {
    public:
        State();
        State(uint8_t data[6]);

        bool pressed(unsigned int button); // Button presed? 0 = lowest green button, 9 = highest orange button
        bool nonePressed(); // Returns true, if no fret or touch button is pessed.
        bool strumDown();
        bool strumUp();
        bool plus();
        bool minus();
        int whammy(); // Whammy bar position: 0 if not used, 15 if whammy bar is pushed fully down
        int joystick_x(); // Joystick position: -32 (leftmost) .. 0 (neutral) .. 31 (rightmost).
        int joystick_y(); // Joystick position: 32 (top) .. 0 (neutral) -31 (bottom).

        String toString(); // State as String for debug printing
        String dataToString(); // Data bytes as String for debugging
        uint8_t data[6]; // Plain data from the controller. Don't use it, use the other methods.

    private:
        unsigned int buttonBitset;
 };

// This class reads data from the guitar controller.
// It provides the STate object which can be used to check the guitar state (e.g. if a button is pressed 
// or the position og the whammy bar.
// 
// An instance of this class is exported as "Guitar" by this header file.
// Use Guitar.begin() in the Arduino setup function and Guitar.read() inside your Arduino loop function
class GuitarC
{
public:
    GuitarC();
    void begin(); //Connect to the controller. Call it inside your Arduino setup function.
    State read(); // Read data from the controller. Call it inside your Arduino loop function.
    String identifyController(); // Describes the controller. Returns a String starting with "Unknown", if the controller is not known to this library.

private:
    short controllerType[3];  
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_GUITAR)
extern GuitarC Guitar;
#endif

#endif // ghero_h