#include "Arduino.h"
#include "Wire.h"
#include "ghero.h"

GuitarC::GuitarC() {}

// Join the I2C bus on the fixed nunchuk device address 0x52
// See http://forum.arduino.cc/index.php/topic,45924.0.html
void GuitarC::begin()
{
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

// Get data from the controller and put it into the "data" byte array.
// Returns true if the data was read successfully, false otherwise.
void GuitarC::read()
{
    // Read 6 bytes from I2C device
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

// Strum
// =====
// Strum bits are 0 on strum. Without a strum the bits are 1.
// Strum Down: data[4] -0-- ----
// Strum Up:   data[5] ---- ---0
boolean GuitarC::strumDown()
{
    return ((data[4] ^ 0xFF) & 0x40) != 0;
}

boolean GuitarC::strumUp()
{
    return ((data[5] ^ 0xFF) & 0x01) != 0;
}

// +/- buttons
// =============
// + Button [4] ---- -0--
// - Button [4] ---0 ----
boolean GuitarC::plus()
{
    return ((data[4] ^ 0xFF) & 0x04) != 0;
}

boolean GuitarC::minus()
{
    return ((data[4] ^ 0xFF) & 0x10) != 0;
}

// Fret buttons
// ==============
// If no fret button is bitset all bits of data[5] are 1: 1111 1111
// If a button is   bitset, one bit gets the value 0
// Green  ---0 ----
// Red    -0-- ----
// Yellow ---- 0---
// Blue   --0- ----
// Orange 0--- ----

boolean GuitarC::fret(Fret fretMask)
{
    return ((data[5] ^ 0xFF) & fretMask) != 0;
}

boolean GuitarC::fretAll(Fret fretMask)
{
    return (data[5] ^ 0xFF) == fretMask;
}

int GuitarC::fret()
{
    if (data[5] == 0xFF)
    {
        return 0;
    }
    else if (fretOrange())
    {
        return 5;
    }
    else if (fretBlue())
    {
        return 4;
    }
    else if (fretYellow())
    {
        return 3;
    }
    else if (fretRed())
    {
        return 2;
    }
    else if (fretGreen())
    {
        return 1;
    }
    return 0;
}

boolean GuitarC::fretGreen()
{
    return fret(FRET_GREEN);
}

boolean GuitarC::fretRed()
{
    return fret(FRET_RED);
}

boolean GuitarC::fretYellow()
{
    return fret(FRET_YELLOW);
}

boolean GuitarC::fretBlue()
{
    return fret(FRET_BLUE);
}

boolean GuitarC::fretOrange()
{
    return fret(FRET_ORANGE);
}

// Touch buttons
// =============
// Touch on two buttons can only be detected if they are neighbours.
// If they are not neighbours, the lower touch doesn't affect the data bits.
// The bits of data[2] when no touch button is  bitset:    0000 1111
const uint8_t TOUCHBITS_GREEN = 0x04;       // Green       ---- 0-00
const uint8_t TOUCHBITS_RED = 0x0A;         // Red         ---- -0-0
const uint8_t TOUCHBITS_YELLOW = 0x12;      // Yellow      ---1 00-0
const uint8_t TOUCHBITS_BLUE = 0x17;        // Blue        ---1 0---
const uint8_t TOUCHBITS_ORANGE = 0x1F;      // Orange      ---1 ----
const uint8_t TOUCHBITS_GREEN_RED = 0x06;   // Green+Red   ---- 0--0
const uint8_t TOUCHBITS_RED_YELLOW = 0x0D;  // Red+Yellow  ---- --0-
const uint8_t TOUCHBITS_YELLOW_BLUE = 0x14; // Yellow+Blue ---1 0-00
const uint8_t TOUCHBITS_BLUE_ORANGE = 0x1A; // Blue+Orange ---1 -0-0

boolean GuitarC::touch(Touch touchField)
{
    switch (touchField)
    {
    case TOUCH_GREEN:
        return touchGreen();
    case TOUCH_RED:
        return touchRed();
    case TOUCH_YELLOW:
        return touchYellow();
    case TOUCH_BLUE:
        return touchBlue();
    case TOUCH_ORANGE:
        return touchOrange();
    default:
        return false;
    }
}

int GuitarC::touch()
{
    if (data[2] == 0x0F)
    {
        return 0;
    }
    else if (touchOrange())
    {
        return 5;
    }
    else if (touchBlue())
    {
        return 4;
    }
    else if (touchYellow())
    {
        return 3;
    }
    else if (touchRed())
    {
        return 2;
    }
    else if (touchGreen())
    {
        return 1;
    }
    return 0;
}

boolean GuitarC::touchGreen()
{
    return data[2] == TOUCHBITS_GREEN || data[2] == TOUCHBITS_GREEN_RED;
}

boolean GuitarC::touchRed()
{
    return data[2] == TOUCHBITS_GREEN_RED || data[2] == TOUCHBITS_RED || data[2] == TOUCHBITS_RED_YELLOW;
}

boolean GuitarC::touchYellow()
{
    return data[2] == TOUCHBITS_RED_YELLOW || data[2] == TOUCHBITS_YELLOW || data[2] == TOUCHBITS_YELLOW_BLUE;
}

boolean GuitarC::touchBlue()
{
    return data[2] == TOUCHBITS_YELLOW_BLUE || data[2] == TOUCHBITS_BLUE || data[2] == TOUCHBITS_BLUE_ORANGE;
}

boolean GuitarC::touchOrange()
{
    return data[2] == TOUCHBITS_BLUE_ORANGE || data[2] == TOUCHBITS_ORANGE;
}

unsigned int GuitarC::buttonBitset() 
{
    unsigned int bitset = 0;
    bitset |= fretGreen();
    bitset |= fretRed() << 1;
    bitset |= fretYellow() << 2;
    bitset |= fretBlue() << 3;
    bitset |= fretOrange() << 4;
    bitset |= touchGreen() << 5;
    bitset |= touchRed() << 6;
    bitset |= touchYellow() << 7;
    bitset |= touchBlue() << 8;
    bitset |= touchOrange() << 9;
    return bitset;
}


// If whammy is not used, data[3] is 0000 1111. If it is pushed the value is 0001 xxxx with x ranging from 0000 to 1111.
// Returns 0 if whammy bar is not used and 0x0F if whammy bar pushed fully down
int GuitarC::whammyValue()
{
    return ((data[3] >> 4) & 1) * (data[3] & 0x0F);
}

// Joystick
// =========
// For Joystick neutral position data[0] is 0010 0000
// When the Joystick is moved right, the bit values of data[0] go from 0010 0000 to 0011 1111 (right position)
// When the Joystick is moved left, the bit values of data[0] go from  0001 1111 to 0000 0000 (left position)
int GuitarC::joystickX()
{
    // Get the last 6 bits and subtract range from center to one side
    return (data[0] & 0x3F) - 32;
}

// For Joystick neutral position data[1] 0001 1111
// When the Joystick is moved to the top, the bit values of data[1] go from    0001 1111 to 0011 1111 (top position)
// When the Joystick is moved to the bottom, the bit values of data[1] go from 0001 1111 to 0000 1000 (bottom position)
int GuitarC::joystickY()
{
    return (data[1] & 0x3F) - 31;
}

// Define global Guitar object
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_GUITAR)
GuitarC Guitar;
#endif