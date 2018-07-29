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

class GuitarC
{
public:
    GuitarC();

    /**
     * Connect to the controller. Call Guitar.begin() inside your Arduino setuo function.
     */
    void begin();

    /**
     * Read data from the controller. Call Guitar.read() inside your Arduino loop function.
     */
    void read();

    boolean strumDown();
    boolean strumUp();
    boolean plus();
    boolean minus();

    /**
     * Enumeration of the fret buttons.
     */
    enum Fret {
      FRET_GREEN = 1 << 4, 
      FRET_RED = 1 << 6, 
      FRET_YELLOW = 1 << 3, 
      FRET_BLUE = 1 << 5, 
      FRET_ORANGE = 1 << 7
    };

    /**
     * Check if one fret button is pressed. 
     * Can also be used to check if at least one of multiple fret buttons are pressed.
     * 
     * Example:
     *   Check if GREEN is pressed: Guitar.fret(Guitar.FRET_GREEN)
     *   Check if GREEN or RED is pressed: Guitar.fret(Guitar.FRET_GREEN | Guitar.FRET_RED)) 
     */
    boolean fret(Fret fretMask);
    
    /**
     * Check if all of mutiple provided fret buttons are pressed.
     * 
     * Example:
     *   Check if GREEN and RED is pressed: Guitar.fretAll(Guitar.FRET_GREEN | Guitar.FRET_RED))
     */
    boolean fretAll(Fret fretMask);
  
    /**
     * Returns the highest fret, in which a button which is pressed (green = 1, orange = 5). 
     * If no fret button is pressed, the result is 0.
     * 
     * Example: 
     *   Check if any fret button is pressed: fret() > 0
     *   If yellow is pressed: fret() == 3.
     *   If green and yellow are pressed: fret() == 3.
     */
    int fret();
    boolean fretGreen();
    boolean fretRed();
    boolean fretYellow();
    boolean fretBlue();
    boolean fretOrange();

    /**
     * Enumeration of the touch fields.
     */
    enum Touch {
      TOUCH_GREEN, TOUCH_RED, TOUCH_YELLOW, TOUCH_BLUE, TOUCH_ORANGE
    };

    /**
     * Returns the highest touch field, which is pressed (green = 1, orange = 5). 
     * If no touch field is pressed, the result is 0.
     * 
     * Example:
     *   Check if any touch field is pressed: touch() > 0 
     *   If green is pressed: touch() == 1.
     *   If green and red are pressed: touch() == 2.
     *   If green and yellow are pressed: touch() == 3. 
     */
    int touch();    
    boolean touch(Touch touchField);
    boolean touchGreen();
    boolean touchRed();
    boolean touchYellow();
    boolean touchBlue();
    boolean touchOrange();

    /** 
     * Returns 0 is the whammy bar is not used and 15 if whammy bar is pushed fully down.
     */
    int whammyValue();

    /** 
     * Returns a range from -32 (leftmost position) to 31 (rightmost position).
     * Returns 0 if the joystick is in neutral position.
     */
    int joystickX();

    /** 
     * Returns a range from 32 (top position) to -31 (bottom position).
     * Returns 0 if the joystick is in neutral position.
     */    
    int joystickY();

    /** 
     * Returns a string which describes the controller or a String starting with "Unknown", if the controller is not known to this library.
     */
    String identifyController();

    /**
     * Data which was read via the read() function. This is only required for debugging purpose, as the other functions expose
     * the data in a more usable way.
     */
    uint8_t data[6];

private:
    short controllerType[3];  
    boolean sixBytesEqual(short a[], short b01, short b23, short b45);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_GUITAR)
extern GuitarC Guitar;
#endif

#endif