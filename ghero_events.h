/*
    ghero_events.h - Add-on library for ghero.h

    This library allows to register handler functions which get called if the state of the Wii Guitar 
    Hero controller changes.
    
    Created by Johannes Bergmann,
    Copyright 2018 License: GNU GPL v3 http://www.gnu.org/licenses/gpl.html
*/
#ifndef ghero_guitar_events_h
#define ghero_guitar_events_h

// An instance of this class is exported as "GuitarEvents" by this header file.
// Use GuitarEvents.on...(...) methods to register handler functions. 
// In your loop functio read the Guitar state and call GuitarEvents.newState(state).
class GuitarEventsC
{
  public:

    // Registers a handler function which is notified when a fret or touch button 
    // changes state from released to pressed.
    // First arg is the button number: 0 (green fret button) .. 9 (orange touch button).
    void onPressed(void (*handler)(int, State));

    // Registers a handler function which is notified when a fret or touch button 
    // changes state from pressed to released.
    //  First arg is the button number: 0 (green fret button) .. 9 (orange touch button).
    void onReleased(void (*handler)(int, State));

    // Registers a handler function which is notified when the whammy bar position chnaged.
    // First arg is the position of the whammy bar: 0 (neutral) .. 15 (pushed fully down)
    void onWhammy(void (*handler)(int, State /* current state */, State /* last state */));

    // Notify this class about a new state. 
    // This method will check for changes and dispatch them to registered handlers.
    void newState(State state);

  private:
    State state_;
    State lastState_;

    void (*pressedHandler)(int, State) = NULL;
    void (*releasedHandler)(int, State) = NULL;
    void (*whammyHandler)(int, State, State) = NULL;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_GUITAR_EVENTS)
extern GuitarEventsC GuitarEvents;
#endif

#endif // ghero_guitar_events_h