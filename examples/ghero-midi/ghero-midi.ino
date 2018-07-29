#include <ghero.h>
#include <MIDI.h>

#define MIDI_CHANNEL 1
#define TONE_VELOCITY 100

MIDI_CREATE_DEFAULT_INSTANCE();

const int MAJOR_SCALE[] = {0, 2, 4, 5, 7, 9, 11, 12, 14, 16};

typedef struct
{
    boolean frets[10] = {false, false, false, false, false, false, false, false, false, false};
    int whammy = 0;
} State;

void (*fretPressedHandler)(int);
void (*fretReleasedHandler)(int);

State lastState;
int lastFret = 0;

void setup()
{
    Serial.begin(9600);

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
    MIDI.sendNoteOn(42 + MAJOR_SCALE[fret], TONE_VELOCITY, MIDI_CHANNEL);
}

void stopTone(int fret)
{
    MIDI.sendNoteOff(42 + MAJOR_SCALE[fret], 0, MIDI_CHANNEL);
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

    if (state.whammy != lastState.whammy)
    {
        MIDI.sendPitchBend(state.whammy * 200, MIDI_CHANNEL);
    }

    lastState = state;
}