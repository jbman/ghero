#include <ghero.h>

/**
 * This sketch helps with analyzing the data bits of the controller.
 * It prints the data bytes whenever the data differs from the last read.
 */
State state;
State lastState;

void setup()
{
    Serial.begin(9600);

    Guitar.begin();
    Serial.println("Identified Controller: " + Guitar.identifyController());
}

void loop()
{
    lastState = state;
    state = Guitar.read();
    if (isDataChanged())
    {
        Serial.println(state.toString());
        Serial.println(state.dataToString());
        Serial.println();
    }
    delay(500); 
}

boolean isDataChanged() 
{
    bool changed = false;
    for (int i=0; i<6; i++) 
    {
        if (lastState.data[i] != state.data[i]) 
        {
            Serial.println("Byte [" + String(i) + "] changed from " + toHex(lastState.data[i]) + " to " + toHex(state.data[i]));
            changed = true;
        }
    }
    return changed;
}

String toHex(uint8_t dataByte) 
{
    String text = "";
    text += " 0x";
    text += String((dataByte & 0xF0) >> 4, HEX);
    text += String((dataByte & 0x0F), HEX); 
    return text;
}