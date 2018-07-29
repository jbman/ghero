#include <ghero.h>

/**
 * This sketch helps with analyzing the data bits of the controller.
 * It prints the six data bytes whenever the data differs from the last data which was read.
 */

uint8_t data[6];
uint8_t lastData[6];

void setup()
{
    Serial.begin(9600);

    Guitar.begin();
    Serial.println("Identified Controller: " + Guitar.identifyController());
}

void loop()
{
    Guitar.read();
    if (isDataChanged())
    {
        printGuitarValues();
        printChangedData();
    }
    delay(500); 
}

boolean isDataChanged() 
{
    for (int i=0; i<=5; i++) 
    {
        if (lastData[i] != Guitar.data[i]) 
        {
            return true;
        }
    }
    return false;
}

void printGuitarValues() 
{
    Serial.print("Fret: ");
    Serial.print(buttonState(fretBits()));

    Serial.print(" Touch: ");
    Serial.print(buttonState(touchBits()));
    
    Serial.print(" Other: ");
    Serial.print(plusMinusState());

    Serial.print(" Strum: ");
    Serial.print(strumState());

    Serial.print(" Whammy: ");
    Serial.print(Guitar.whammyValue());

    Serial.print(" X: ");
    Serial.print(Guitar.joystickX());

    Serial.print(" Y: ");
    Serial.print(Guitar.joystickY());

    Serial.println();
}

String buttonState(uint8_t x) 
{ 
    String text = "";
    text+= ((x & 0x80) > 0) ? "g" : ".";
    x <<= 1;
    text+= ((x & 0x80) > 0) ? "r" : ".";
    x <<= 1;
    text+= ((x & 0x80) > 0) ? "y" : ".";
    x <<= 1;
    text+= ((x & 0x80) > 0) ? "b" : ".";
    x <<= 1;
    text+= ((x & 0x80) > 0) ? "o" : ".";
    return text;
}

String plusMinusState() 
{ 
    String text = "";
    text+= Guitar.plus() ? "+" : ".";
    text+= Guitar.minus() ? "-" : ".";
    return text;
}

String strumState() 
{ 
    if (Guitar.strumUp()) {
        return "u";
    }
    else if (Guitar.strumDown()) {
        return "d";
    } 
    else {
        return ".";
    }
}

// Returns a byte with a bit set for each button pressed:
// 1st most significant bit is 1 => Green pressed
// 2nd most significant bit is 1 => Red pressed
// ...
// Example: 1001 0000 => Green and Blue pressed
uint8_t fretBits() {
    uint8_t bits;
    bits |= Guitar.fretGreen() << 7;
    bits |= Guitar.fretRed() << 6;
    bits |= Guitar.fretYellow() << 5;
    bits |= Guitar.fretBlue() << 4;
    bits |= Guitar.fretOrange() << 3;
    return bits;
}

uint8_t touchBits() {
    uint8_t bits;
    bits |= Guitar.touchGreen() << 7;
    bits |= Guitar.touchRed() << 6;
    bits |= Guitar.touchYellow() << 5;
    bits |= Guitar.touchBlue() << 4;
    bits |= Guitar.touchOrange() << 3;
    return bits;
}

// Prints the bits and hex value of each byte which changed since last read.
void printChangedData() 
{
    for (int i=0; i<=5; i++) 
    {
        Serial.print(" ["); Serial.print(i); Serial.print("] ");
        if (lastData[i] != Guitar.data[i]) 
        {
            printBits(Guitar.data[i]);
        }
        else 
        {
            Serial.print("         ");
        }
        Serial.print(" 0x");
        Serial.print((Guitar.data[i] & 0xF0) >> 4, HEX);
        Serial.print(Guitar.data[i] & 0x0F, HEX);
        lastData[i] =Guitar.data[i];
    }
    Serial.println();   

}

// Prints the bits of a byte. Example: printBits(0x0F) prints "0000 1111"
void printBits(uint8_t x) 
{ 
    for(int n=0; n<4; n++) 
    { 
        Serial.print((x & 0x80) > 0); x = x<<1; 
    } 
    Serial.print(" ");
    for(int n=0; n<4; n++) 
    { 
        Serial.print((x & 0x80) > 0); x = x<<1; 
    } 
}
