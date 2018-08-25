/*
    ghero.h - Library for connecting a Wii Guitar Hero controller to the Arduino

    This library decodes the data send by a Wii Guitar Hero controller. 
    It was developed with a Logitech Guitar Hero controller. It may also work for other 
    Wii guitar hero controllers, but it isn't tested.

    The guitar is connected to the arduino with the help of a small passive adapter ("nunchuk adapter für arduino").
    The releavant output pins of this adapter are '+', '-', 'd' and 'c':
    - is connected to GND on arduino
    + is connected to +3,3V on arduino
    d is connected to SDA pin (A4 on arduino nano)
    c is connected to SCL pin (A5 on arduino nano)

    Created by Johannes Bergmann,
    Copyright 2018 License: GNU GPL v3 http://www.gnu.org/licenses/gpl.html
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

        // Check if a button is pressed. 0 = lowest green button, 9 = highest orange button
        bool pressed(unsigned int button);

        bool nonePressed(); // Returns true, if no fret or touch button is pessed.

        bool strumDown();
        bool strumUp();
        bool plus();
        bool minus();

        // Whammy bar position: 0 if not used, 15 if whammy bar is pushed fully down
        int whammy();

        // Joystick x value. Range: -32 (leftmost position), 0 (neutral position), 31 (rightmost position).
        int joystick_x();

        // Joystick y value. Range :32 (top position), 0 (neutral position), -31 (bottom position).
        int joystick_y();

        String toString();
        String dataToString();

        // Plain data which was read from the controller.
        uint8_t data[6];
       


    private:
        // A bitset which has a 1 for each pressed button.
        // 0..4 (green..orange fret button) 
        // 5..9 (green..orange touch button)
        unsigned int buttonBitset;

        bool fretPressed(uint8_t fretMask);
        bool touchGreen();
        bool touchRed();
        bool touchYellow();
        bool touchBlue();
        bool touchOrange();
};

/*
 * This class reads data from the guitar controller.
 * It provides methods which provide the current state (e.g. if a button is pressed 
 * or a value for the whammy bar position).
 * 
 * An instance of this class is exported as "Guitar" by this header file.
 * So just use Guitar.begin() in the Arduino setup function and Guitar.read() 
 * inside your Arduino loop function
 */
class GuitarC
{
public:
    GuitarC();

    //Connect to the controller. Call Guitar.begin() inside your Arduino setup function.
    void begin();

    /**
     * Read data from the controller. Call Guitar.read() inside your Arduino loop function.
     */
    State read();

    /** 
     * Returns a string which describes the controller or a String starting with "Unknown", if the controller is not known to this library.
     */
    String identifyController();

private:
    short controllerType[3];  
    bool sixBytesEqual(short a[], short b01, short b23, short b45);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_GUITAR)
extern GuitarC Guitar;
#endif

#endif // ghero_h