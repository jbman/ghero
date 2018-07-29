#include <ghero.h>

/**
    Example for sound output through a speaker.

    The passive speaker is connected to pin 8 of the Arduino and to Ground.

    Created by Johannes Bergmann,
    Copyright 2018 License: GNU GPL v3 http://www.gnu.org/licenses/gpl.html
*/

#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698

#define SPEAKER_PIN 8

unsigned int base = 4;
unsigned long lastBaseChange;
unsigned int playingPitch = 0;

void setup()
{
    Serial.begin(9600);

    // Control built-in LED. It is switched on when a tone playes.
    pinMode(13, OUTPUT);

    Guitar.begin();
    delay(100);

    playTone(NOTE_C4);
    delay(50);
    playTone(NOTE_D4);
    delay(50);
    playTone(NOTE_E4);
    delay(50);
    playTone(NOTE_F4);
    delay(50);
    stopTone();
}

void loop()
{
    Guitar.read();

    int newPitch = playingPitch;

    if (Guitar.strumDown())
    {
        if (Guitar.fretGreen())
        {
            newPitch = NOTE_C4;
        }
        if (Guitar.fretRed())
        {
            newPitch = NOTE_D4;
        }
        if (Guitar.fretYellow())
        {
            newPitch = NOTE_E4;
        }
        if (Guitar.fretBlue())
        {
            newPitch = NOTE_F4;
        }
        if (Guitar.fretOrange())
        {
            newPitch = NOTE_G4;
        }
        if (Guitar.touchGreen())
        {
            newPitch = NOTE_A4;
        }
        if (Guitar.touchRed())
        {
            newPitch = NOTE_B4;
        }
        if (Guitar.touchYellow())
        {
            newPitch = NOTE_C5;
        }
        if (Guitar.touchBlue())
        {
            newPitch = NOTE_D5;
        }
        if (Guitar.touchOrange())
        {
            newPitch = NOTE_E5;
        }
    }

    if (Guitar.strumUp())
    {
        if (Guitar.fretGreen())
        {
            newPitch = NOTE_CS4;
        }
        if (Guitar.fretRed())
        {
            newPitch = NOTE_DS4;
        }
        if (Guitar.fretYellow())
        {
            newPitch = NOTE_F4;
        }
        if (Guitar.fretBlue())
        {
            newPitch = NOTE_FS4;
        }
        if (Guitar.fretOrange())
        {
            newPitch = NOTE_GS4;
        }
        if (Guitar.touchGreen())
        {
            newPitch = NOTE_AS4;
        }
        if (Guitar.touchRed())
        {
           newPitch = NOTE_C5;
        }
        if (Guitar.touchYellow())
        {
            newPitch = NOTE_CS5;
        }
        if (Guitar.touchBlue())
        {
            newPitch = NOTE_DS5;
        }
        if (Guitar.touchOrange())
        {
            newPitch = NOTE_F5;
        }
    }

    if (newPitch != playingPitch)
    {
        playTone(newPitch);
        playingPitch = newPitch;
    }

    if (Guitar.fret() <= 0 && Guitar.touch() <= 0)
    {
        stopTone();
    }

    if (millis() - lastBaseChange > 500)
    {
        if (Guitar.plus() && base <= 16)
        {
            base = base + base;
            lastBaseChange = millis();
        }
        else if (Guitar.minus() && base > 1)
        {
            base = base / 2;
            lastBaseChange = millis();
        }
    }
}

// output pitch range (120 - 1500Hz)
void playTone(int pitch)
{
    digitalWrite(13, HIGH);
    tone(SPEAKER_PIN, pitch);
    tone(SPEAKER_PIN, (pitch + Guitar.whammyValue()) * base / 4);
}

void stopTone()
{
    digitalWrite(13, LOW);
    noTone(SPEAKER_PIN);
}