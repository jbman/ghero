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
GuitarNotifier notifier;

const int NOTES[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_E5};

void setup()
{
    Serial.begin(9600);

    // Control built-in LED. It is switched on when a tone playes.
    pinMode(13, OUTPUT);

    Guitar.begin();
    delay(200);
    Guitar.read();

    // notifier.onPressed(printPressed);
    // notifier.onReleased(printReleased);
    
    notifier.onPressed(playTone);
    notifier.onReleased(stopTone);

    beginTones();
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

void beginTones() {
    playTone(0);
    delay(50);
    playTone(1);
    delay(50);
    playTone(2);
    delay(50);
    playTone(3);
    delay(50);
    stopTone(0);
}

void loop()
{
    Guitar.read();
    notifier.notifyHandlers(Guitar);

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

void playTone(int position)
{
    digitalWrite(13, HIGH);
    tone(SPEAKER_PIN, (NOTES[position] + Guitar.whammyValue()) * base / 4);
}

void stopTone(int position)
{
    // Stop tone if no button pressed at all
    if (Guitar.buttonBitset() == 0) 
    {
        digitalWrite(13, LOW);
        noTone(SPEAKER_PIN);
    }
}