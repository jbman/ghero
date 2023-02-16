#include <ghero.h>
#include <MIDI.h>

/**
    Example: Wii Guitar Controller as MIDI controller.

    The Midi jack is conneted according to the Arduino MIDI tutorial (https://www.arduino.cc/en/Tutorial/Midi)
    - MIDI jack pin 5 connected to Digital pin 1 through a 220 ohm resistor
    - MIDI jack pin 2 connected to ground
    - MIDI jack pin 4 connected to +5V through a 220 ohm resistor 

    Created by Johannes Bergmann,
    MIT license, all text above must be included in any redistribution
*/

#define MIDI_CHANNEL 1
#define TONE_VELOCITY 100

MIDI_CREATE_DEFAULT_INSTANCE();

const int MAJOR_SCALE[] = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16};

int transpose = 0;

typedef struct
{
    boolean frets[10] = {false, false, false, false, false, false, false, false, false, false};
    int whammy = 0;
    boolean plus = false;
    boolean minus = false;
} State;

void (*fretPressedHandler)(int);
void (*fretReleasedHandler)(int);

State lastState;

void setup()
{
    Serial.begin(9600);

    // Built-in LED
    pinMode(13, OUTPUT);

    // Send MIDI messages on channel 1
    MIDI.begin(MIDI_CHANNEL);

    onFretPressed(playTone);
    onFretReleased(stopTone);

    Guitar.begin();
}

void onFretPressed(void (*handler)(int))
{
    fretPressedHandler = handler;
}

void onFretReleased(void (*handler)(int))
{
    fretReleasedHandler = handler;
}

void printPressed(int fret)
{
    Serial.print("Pressed ");
    Serial.println(fret);
}

void printReleased(int fret)
{
    Serial.print("Released ");
    Serial.println(fret);
}

void playTone(int fret)
{
    digitalWrite(13, HIGH);
    MIDI.sendNoteOn(42 + transpose + MAJOR_SCALE[fret], TONE_VELOCITY, MIDI_CHANNEL);
}

void stopTone(int fret)
{
    digitalWrite(13, LOW);
    MIDI.sendNoteOff(42 + transpose + MAJOR_SCALE[fret], 0, MIDI_CHANNEL);
}

void loop()
{
    Guitar.read();
    State state;
    state.frets[0] = Guitar.fretGreen();
    state.frets[1] = Guitar.fretRed();
    state.frets[2] = Guitar.fretYellow();
    state.frets[3] = Guitar.fretBlue();
    state.frets[4] = Guitar.fretOrange();
    state.frets[5] = Guitar.touchGreen();
    state.frets[6] = Guitar.touchRed();
    state.frets[7] = Guitar.touchYellow();
    state.frets[8] = Guitar.touchBlue();
    state.frets[9] = Guitar.touchOrange();
    state.whammy = Guitar.whammyValue();
    state.plus = Guitar.plus();
    state.minus = Guitar.minus();

    for (int i = 0; i < 10; i++)
    {
        if (state.frets[i] && !lastState.frets[i])
        {
            fretPressedHandler(i);
        }
        if (!state.frets[i] && lastState.frets[i])
        {
            fretReleasedHandler(i);
        }
    }

    if (state.plus && !lastState.plus)
    {
        transpose++;
        transpose = min(transpose, 16);
    }
    if (state.minus && !lastState.minus)
    {
        transpose--;
        transpose = max(transpose, -16);
    }     

    if (state.whammy != lastState.whammy)
    {
        MIDI.sendPitchBend(state.whammy * 200, MIDI_CHANNEL);
    }

    lastState = state;
}
