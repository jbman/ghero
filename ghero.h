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

/**
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

    /**
     * Connect to the controller. Call Guitar.begin() inside your Arduino setup function.
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
    * Returns a number where the button state of the Guitar is represented by bits.
    * If a button is pressed, the bit is set to 1. The lowest bit is for the first button (green).
    * 
    * The n-th button bit can be sesolved with:
    * b = bitset() 
    * nthButtonPressed = ((b >> n) & 1) == 1;
    */
    unsigned int buttonBitset();

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

class GuitarState {
    public:
        /**
         * Stores the current state provided by the given guitar reference.
         */
        GuitarState(const GuitarC &guitar);

        /**
        * Returns true the fret or touch button at given position is pressed.
        * position: The button number. Nzmbers from 0 (green fret button) up to 9 (orange touch button) are valid.
        */
        bool isPressed(unsigned int position);

    private:
        // The bits of this field are used to store which buttons are pressed.
        unsigned int buttonBitset = 0;
        int whammy = 0;
};

/**
 * This class allows to register handlers which are notified in case of a state change.
 * It uses the Guitar instance to read the current state, compares it against the last state and 
 * invokes the registered handler functions.
 */
class GuitarNotifier
{
  public:
    /**
    * Registers a handler function which is notified when a fret or touch button 
    * changes state from released to pressed.
    * 
    * The argument of the handler function is a number from 0 (green fret button) up to 
    * 9 (orange touch button).
    */
    void onPressed(void (*handler)(int));

    /**
    * Registers a handler function which is notified when a fret or touch button 
    * changes state from pressed to released.
    * 
    * The argument of the handler function is a number from 0 (green fret button) up to 
    * 9 (orange touch button).
    */
    void onReleased(void (*handler)(int));

    void notifyHandlers(const GuitarC &guitar);

  private:
    GuitarState *state;
    GuitarState *lastState;

    void (*pressedHandler)(int) = NULL;
    void (*releasedHandler)(int) = NULL;
};

#endif