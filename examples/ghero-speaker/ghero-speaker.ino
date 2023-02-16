/**
    Example: Wii Guitar Controller plays sound through a speaker.

    The passive speaker is connected to pin 8 of the Arduino and to Ground.

    Created by Johannes Bergmann,
    MIT license, all text above must be included in any redistribution
*/
#include <ghero.h>
#include <ghero_events.h>

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
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988

#define SPEAKER_PIN 8

const int NOTES[] = {
    NOTE_C4, NOTE_CS4, NOTE_D4, NOTE_DS4, NOTE_E4, NOTE_F4, NOTE_FS4, NOTE_G4, NOTE_GS4, NOTE_A4, NOTE_AS4, NOTE_B4, 
    NOTE_C5, NOTE_CS5, NOTE_D5, NOTE_DS5, NOTE_E5, NOTE_F5, NOTE_FS5, NOTE_G5, NOTE_GS5, NOTE_A5, NOTE_AS5, NOTE_B5};

const int MAJOR[] = {0,2,4,5,7,9,11,12,14,16};

unsigned int freq = 0; // The frequency for tone output
int sweep; // Frequency deviation for chorus effect

unsigned int base = 4;
int currentButton = -1;
int transpose = 0;
int bend = 0;
int chorusDepth = 0; // 0..31
int chorusSpeed = 0; // 0..31

void setup()
{
    Serial.begin(9600);

    // Control built-in LED. It is switched on when a tone playes.
    pinMode(13, OUTPUT);

    Guitar.begin();
    delay(200);
    Guitar.read();
    
    GuitarEvents.onPressed(pressed);
    GuitarEvents.onReleased(released);
    GuitarEvents.onPlusMinus(plusMinus);
    GuitarEvents.onStrum(strum);
    GuitarEvents.onWhammy(whammy);
    GuitarEvents.onJoystick(joystick);

    playStartupTones();
}

void playStartupTones() {
    for(int i=0; i<4; i++) 
    {
        tone(SPEAKER_PIN, NOTES[MAJOR[i]]);
        delay(50);
    }
    noTone(SPEAKER_PIN);
}

void loop()
{
    State state = Guitar.read();
    GuitarEvents.newState(state);

    if(freq > 0) {
        chorusEffect();
        digitalWrite(13, HIGH);        
        tone(SPEAKER_PIN, freq + sweep);
    }
    else {
        digitalWrite(13, LOW);
        noTone(SPEAKER_PIN);
    }
}

void updateTone()
{
    if (currentButton >= 0) {
        digitalWrite(13, HIGH);
        int note = (MAJOR[currentButton] + transpose) % 24;
        freq = (NOTES[note] + bend) * base / 4;
    }
    else {
        freq = 0;
    }
}

void chorusEffect() 
{
    if (chorusDepth > 0 || chorusSpeed > 0)
    {
        int length = 190 - (chorusSpeed * 3);
        sweep = sin(radians(map(millis() % length, 0, length-1, 0, 360))) * (chorusDepth+3)/3;
    }
    else
    {
        sweep = 0;
    }
}

void pressed(int button, State state)
{
    Serial.print("Pressed ");
    Serial.println(button);
    currentButton = button;
    updateTone();
}

void released(int button, State state)
{
    Serial.print("Released "); 
    Serial.println(button);
    // Stop tone if all buttons are released
    if (state.nonePressed()) 
    {
        currentButton = -1;
        updateTone();
    }
}
void plusMinus(PlusMinusAction action, State state, State last) 
{
    if (action == PlusMinusAction::PLUS_PRESSED && base <= 16)
    {
        base = base + base;
        updateTone();
    }
    if (action == PlusMinusAction::MINUS_PRESSED && base > 1)
    {
        base = base / 2;
        updateTone();
    }
}

void strum(Strum strum, State state, State last) 
{
    Serial.println("Strum ");
    Serial.println(strum == Strum::NONE ? "none" : "up/down");
    if(strum == Strum::DOWN) {
        transpose = 4;
    }
    else if(strum == Strum::UP) {
        transpose = 7;
    }
    else if(strum == Strum::NONE) {
        transpose = 0;
    }
    updateTone();
}

void whammy(int value, State state, State last) 
{
    Serial.print("Whammy ");
    Serial.println(value);
    bend = state.whammy(); 
    updateTone();
}

void joystick(int x, int y, State state, State last) 
{
    Serial.print("Joystick x=");
    Serial.print(x);
    Serial.print("y= ");
    Serial.println(y);
    chorusSpeed = min(abs(x),30);
    chorusDepth = min(abs(y),30);
    updateTone();
}