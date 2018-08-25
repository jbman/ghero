#include "ghero.h"
#include "ghero_events.h"

void GuitarEvents::onPressed(void (*handler)(int, State))
{
    pressedHandler = handler;
}

void GuitarEvents::onReleased(void (*handler)(int, State))
{
    releasedHandler = handler;
}

void GuitarEvents::newState(State state)
{
    lastState_ = state_;
    state_ = state;

    // Detect pressed and release buttons
    for (int i = 0; i < 10; i++)
    {
        if (!lastState_.pressed(i) && state_.pressed(i))
        {
            if (pressedHandler != NULL) {
                pressedHandler(i, state_);
            }
        }
        if (lastState_.pressed(i) && !state_.pressed(i))
        {
            if (releasedHandler != NULL) {
                releasedHandler(i, state_);
            }
        }
    }
}

// Define global object
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_GUITAR_EVENTS)
GuitarEvents Events;
#endif
