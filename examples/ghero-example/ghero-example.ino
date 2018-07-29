#include <ghero.h>

void setup()
{
    Serial.begin(9600);

    // Initialize guitar controller communication
    Guitar.begin();
    delay(100);

    Guitar.read();
    // Prints "LogitechGuitarHero" if the Logitech guitar is connected
    Serial.println("Identified Controller: " + Guitar.identifyController());
}

void loop()
{
    Guitar.read();

    // Detect if strum bar is pushed or pulled:
    if (Guitar.strumDown())
    {
        Serial.println("Strum down");
    }
    if (Guitar.strumUp())
    {
        Serial.println("Strum up");
    }

    // Get the highest fret, in which a button is pressed
    int fret = Guitar.fret();
    if (fret > 0) 
    {
        Serial.print("Fret ");
        Serial.println(fret);
    }

    // Detect button presses
    if (Guitar.fretGreen())
    {
        Serial.println("Green button pressed");
    }
    if (Guitar.fret(Guitar.FRET_RED | Guitar.FRET_YELLOW))
    {
        Serial.println("Red or yellow pressed");
    }
    if (Guitar.fretAll(Guitar.FRET_BLUE | Guitar.FRET_ORANGE))
    {
        Serial.println("Blue and orange pressed together");
    }
    
    // Detect touch presses
    if (Guitar.touchGreen())
    {
        Serial.println("Green touch field pressed");
    }
    if (Guitar.touch(Guitar.TOUCH_RED))
    {
        Serial.println("Red touch field pressed");
    }

    // Read whammy bar value
    int whammy = Guitar.whammyValue();
    if (whammy > 0) 
    {
        Serial.print("Whammy bar: ");
        Serial.println(whammy);
    }
}
