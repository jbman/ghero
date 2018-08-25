#include "Arduino.h"
#include "Wire.h"
#include "ghero.h"

// Fret buttons
// ==============
// If no fret button is pressed all bits of data[5] are 1: 1111 1111
// If a button is pressed, one bit gets the value 0
const uint8_t FRET_GREEN =  1 << 4;  // Green ---0 ----
const uint8_t FRET_RED =    1 << 6;  // Red    -0-- ----
const uint8_t FRET_YELLOW = 1 << 3;  // Yellow ---- 0---
const uint8_t FRET_BLUE =   1 << 5;  // Blue   --0- ----
const uint8_t FRET_ORANGE = 1 << 7;  // Orange 0--- ----

State::State() { }

State::State(uint8_t controller_data[6])
{
    for(int i=0; i<6; i++) {
        data[i] = controller_data[i];
    }

    buttonBitset = 0;
    buttonBitset |= fretPressed(FRET_GREEN);
    buttonBitset |= fretPressed(FRET_RED) << 1;
    buttonBitset |= fretPressed(FRET_YELLOW) << 2;
    buttonBitset |= fretPressed(FRET_BLUE) << 3;
    buttonBitset |= fretPressed(FRET_ORANGE) << 4;
    buttonBitset |= touchGreen() << 5;
    buttonBitset |= touchRed() << 6;
    buttonBitset |= touchYellow() << 7;
    buttonBitset |= touchBlue() << 8;
    buttonBitset |= touchOrange() << 9;
    buttonBitset |= touchOrange() << 9;
}

bool State::pressed(unsigned int position) 
{
    return ((buttonBitset >> position) & 1) == 1;
}

bool State::nonePressed() 
{
    return buttonBitset == 0;
}

// Strum
// =====
// Strum bits are 0 on strum. Without a strum the bits are 1.
// Strum Down: data[4] -0-- ----
// Strum Up:   data[5] ---- ---0
boolean State::strumDown()
{
    return ((data[4] ^ 0xFF) & 0x40) != 0;
}

boolean State::strumUp()
{
    return ((data[5] ^ 0xFF) & 0x01) != 0;
}

// +/- buttons
// =============
// + Button [4] ---- -0--
// - Button [4] ---0 ----
boolean State::plus()
{
    return ((data[4] ^ 0xFF) & 0x04) != 0;
}

boolean State::minus()
{
    return ((data[4] ^ 0xFF) & 0x10) != 0;
}

// If whammy is not used, data[3] is 0000 1111. If it is pushed the value is 0001 xxxx with x ranging from 0000 to 1111.
// Returns 0 if whammy bar is not used and 0x0F if whammy bar pushed fully down
int State::whammy()
{
    return ((data[3] >> 4) & 1) * (data[3] & 0x0F);
}

// Joystick
// =========
// For Joystick neutral position data[0] is 0010 0000
// When the Joystick is moved right, the bit values of data[0] go from 0010 0000 to 0011 1111 (right position)
// When the Joystick is moved left, the bit values of data[0] go from  0001 1111 to 0000 0000 (left position)
int State::joystick_x()
{
    // Get the last 6 bits and subtract range from center to one side
    return (data[0] & 0x3F) - 32;
}

// For Joystick neutral position data[1] 0001 1111
// When the Joystick is moved to the top, the bit values of data[1] go from    0001 1111 to 0011 1111 (top position)
// When the Joystick is moved to the bottom, the bit values of data[1] go from 0001 1111 to 0000 1000 (bottom position)
int State::joystick_y()
{
    return (data[1] & 0x3F) - 31;
}

String State::toString()
{
    String text = "";
    text += "Buttons: ";
    for(int i=0; i<5; i++) {
        text += pressed(i) ? "o" : ".";    
    }
    text += " ";
    for(int i=5; i<10; i++) {
        text += pressed(i) ? "o" : ".";    
    }
    text += " ";
    text += plus() ? "+" : ".";
    text += minus() ? "-" : ".";
    text += " Strum: ";    
    text += strumUp() ? "up  " : (strumDown() ? "down" : "-   ");
    text += " Whammy: ";
    text += whammy();
    text += " X: ";
    text += joystick_x();
    text += " Y: ";
    text += joystick_y();
    return text;
}

String State::dataToString()
{
    String text = "";
    for (int i=0; i<6; i++) 
    {
        text += "[" + String(i) + "] ";
        uint8_t x = data[i];
        for(int n=0; n<8; n++) 
        { 
            text += ((x & 0x80) > 0);
            text += (n==3 ? " " : "");
            x = x<<1; 
        } 
        text += " 0x";
        text += String((data[i] & 0xF0) >> 4, HEX);
        text += String((data[i] & 0x0F), HEX); 
        text += " ";
    }
    return text;
}

// ***** private *****

// Checks if one fret button is pressed. 
// Can also be used to check if at least one of multiple fret buttons are pressed.
// Examples:
// - Check if GREEN is pressed: Guitar.fret(Guitar.FRET_GREEN)
// - Check if GREEN or RED is pressed: Guitar.fret(Guitar.FRET_GREEN | Guitar.FRET_RED)) 
bool State::fretPressed(uint8_t fretMask)
{
    return ((data[5] ^ 0xFF) & fretMask) != 0;
}


// Touch buttons
// =============
// Touch on two buttons can only be detected if they are neighbours.
// If they are not neighbours, the lower touch doesn't affect the data bits.
// The bits of data[2] when no touch button is pressed:    0000 1111
const uint8_t TOUCHBITS_GREEN = 0x04;       // Green       ---- 0-00
const uint8_t TOUCHBITS_RED = 0x0A;         // Red         ---- -0-0
const uint8_t TOUCHBITS_YELLOW = 0x12;      // Yellow      ---1 00-0
const uint8_t TOUCHBITS_BLUE = 0x17;        // Blue        ---1 0---
const uint8_t TOUCHBITS_ORANGE = 0x1F;      // Orange      ---1 ----
const uint8_t TOUCHBITS_GREEN_RED = 0x06;   // Green+Red   ---- 0--0
const uint8_t TOUCHBITS_RED_YELLOW = 0x0D;  // Red+Yellow  ---- --0-
const uint8_t TOUCHBITS_YELLOW_BLUE = 0x14; // Yellow+Blue ---1 0-00
const uint8_t TOUCHBITS_BLUE_ORANGE = 0x1A; // Blue+Orange ---1 -0-0

boolean State::touchGreen()
{
    return data[2] == TOUCHBITS_GREEN || data[2] == TOUCHBITS_GREEN_RED;
}

boolean State::touchRed()
{
    return data[2] == TOUCHBITS_GREEN_RED || data[2] == TOUCHBITS_RED || data[2] == TOUCHBITS_RED_YELLOW;
}

boolean State::touchYellow()
{
    return data[2] == TOUCHBITS_RED_YELLOW || data[2] == TOUCHBITS_YELLOW || data[2] == TOUCHBITS_YELLOW_BLUE;
}

boolean State::touchBlue()
{
    return data[2] == TOUCHBITS_YELLOW_BLUE || data[2] == TOUCHBITS_BLUE || data[2] == TOUCHBITS_BLUE_ORANGE;
}

boolean State::touchOrange()
{
    return data[2] == TOUCHBITS_BLUE_ORANGE || data[2] == TOUCHBITS_ORANGE;
}

// --- GuitarC implementation ---

GuitarC::GuitarC() {}

void GuitarC::begin()
{
    // Join the I2C bus on the fixed nunchuk device address 0x52
    // See http://forum.arduino.cc/index.php/topic,45924.0.html
    Wire.begin();
    delay(1);
    Wire.beginTransmission(0x52);
    // Initialize first register and value:
    Wire.write((uint8_t)0xF0);
    Wire.write((uint8_t)0x55);
    Wire.endTransmission();
    delay(1);
    Wire.beginTransmission(0x52);
    // Initialize second register and value:
    Wire.write((uint8_t)0xFB);
    Wire.write((uint8_t)0x00);
    Wire.endTransmission();
    delay(1);

    // Read the controller type from the register block
    Wire.beginTransmission(0x52);
    Wire.write(0xFA);
    Wire.endTransmission();
    Wire.beginTransmission(0x52);
    Wire.requestFrom(0x52, 6);
    for (int i = 0; i < 3; i++)
    {
        if (Wire.available())
        {
            controllerType[i] = (Wire.read() << 8) + Wire.read();
        }
    }
    Wire.endTransmission();
    delay(1);
}

State GuitarC::read()
{
    // Read 6 bytes from I2C device
    uint8_t data[6];
    int i = 0;
    Wire.requestFrom(0x52, 6);
    while (Wire.available())
    {
        data[i] = Wire.read();
        i++;
    }
    // Acknowledge the transmission
    Wire.beginTransmission(0x52);
    Wire.write((uint8_t)0x00);
    Wire.endTransmission();

    return State(data);
}

String GuitarC::identifyController()
{
    String result;
    if (sixBytesEqual(controllerType, 0x0000, 0xA420, 0x0103))
    {
        result = "LogitechGuitarHero";
    }
    else if (sixBytesEqual(controllerType, 0x0000, 0xA420, 0x0000))
    {
        result = "Nunchuk";
    }
    else
    {
        result = "Unknown";
    }
    for (int i = 0; i < 3; i++)
    {
        result += " 0x" + String(controllerType[i], HEX);
    }
    return result;
}

boolean GuitarC::sixBytesEqual(short a[], short b01, short b23, short b45)
{
    return a[0] == b01 && a[1] == b23 && a[2] == b45;
}

// Define global Guitar object
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_GUITAR)
GuitarC Guitar;
#endif