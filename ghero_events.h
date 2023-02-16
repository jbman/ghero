/*
    ghero_events.h - Add-on library for ghero.h

    This library allows to register handler functions which get called if the state of the Wii Guitar 
    Hero controller changes.
    
    Created by Johannes Bergmann,
    MIT license, all text above must be included in any redistribution
*/
#ifndef ghero_guitar_events_h
#define ghero_guitar_events_h

enum class Strum {
  NONE,
  DOWN,
  UP
};

enum class PlusMinusAction {
  PLUS_PRESSED,
  PLUS_RELEASED,
  MINUS_PRESSED,
  MINUS_RELEASED
};

// An instance of this class is exported as "GuitarEvents" by this header file.
// Use GuitarEvents.on...(...) methods to register handler functions. 
// In your loop functio read the Guitar state and call GuitarEvents.newState(state).
class GuitarEventsC
{
  public:

    // Registers a handler function which is notified when a fret or touch button 
    // changes state from released to pressed.
    // First arg is the button number: 0 (green fret button) .. 9 (orange touch button).
    void onPressed(void (*handler)(int, State /* current state */, State /* last state */));

    // Registers a handler function which is notified when a fret or touch button 
    // changes state from pressed to released.
    // First arg is the button number: 0 (green fret button) .. 9 (orange touch button).
    void onReleased(void (*handler)(int, State /* current state */, State /* last state */));

    // Registers a handler function which is notified when the strum bar is used for up or down strum.
    // First arg is the strum state.
    void onStrum(void (*handler)(Strum, State /* current state */, State /* last state */));

    // Registers a handler function which is notified when the whammy bar position changed.
    // First arg is the position of the whammy bar: 0 (neutral) .. 15 (pushed fully down)
    void onWhammy(void (*handler)(int, State /* current state */, State /* last state */));

    // Registers a handler function which is notified when the plus or minus button was pressed or released.
    // First arg provides the action on the buttons
    void onPlusMinus(void (*handler)(PlusMinusAction, State /* current state */, State /* last state */));

    // Registers a handler function which is notified when the joystick position changed.
    // First arg is the position of the whammy bar: 0 (neutral) .. 15 (pushed fully down)
    void onJoystick(void (*handler)(int, int, State /* current state */, State /* last state */));

    // Notify this class about a new state. 
    // This method will check for changes and dispatch them to registered handlers.
    void newState(State state);

  private:
    State state_;
    State lastState_;

    // Pointers to handler funtions. Initialized with a "do nothing" function.
    void (*pressedHandler)(int, State, State) = [](int button, State s1, State s2) -> void {};
    void (*releasedHandler)(int, State, State) = [](int button, State s1, State s2) -> void {};
    void (*plusMinusHandler)(PlusMinusAction, State, State) = [](PlusMinusAction a, State s1, State s2) -> void {};
    void (*strumHandler)(Strum, State, State) = [](Strum strum, State s1, State s2) -> void {};
    void (*whammyHandler)(int, State, State) = [](int value, State s1, State s2) -> void {};
    void (*joystickHandler)(int, int, State, State) = [](int x, int y, State s1, State s2) -> void {};
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_GUITAR_EVENTS)
extern GuitarEventsC GuitarEvents;
#endif

#endif // ghero_guitar_events_h