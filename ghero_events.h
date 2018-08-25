#ifndef ghero_guitar_events_h
#define ghero_guitar_events_h

/**
 * This class allows to register handlers which are notified when a button on the guitar is pressed
 * or some other state changes.
 */
class GuitarEvents
{
  public:

    /**
    * Registers a handler function which is notified when a fret or touch button 
    * changes state from released to pressed.
    * 
    * The argument of the handler function is a number from 0 (green fret button) up to 
    * 9 (orange touch button).
    */
    void onPressed(void (*handler)(int, State));

    /**
    * Registers a handler function which is notified when a fret or touch button 
    * changes state from pressed to released.
    * 
    * The argument of the handler function is a number from 0 (green fret button) up to 
    * 9 (orange touch button).
    */
    void onReleased(void (*handler)(int, State));

    // Notify this class about a new state. 
    // This method will check for changes and dispatch them tp registered handlers.
    void newState(State state);

  private:
    State state_;
    State lastState_;

    void (*pressedHandler)(int, State) = NULL;
    void (*releasedHandler)(int, State) = NULL;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_GUITAR_EVENTS)
extern GuitarEvents Events;
#endif

#endif // ghero_guitar_events_h