#include "ghero.h"

void GuitarNotifier::onPressed(void (*handler)(int))
{
    pressedHandler = handler;
}

void GuitarNotifier::onReleased(void (*handler)(int))
{
    releasedHandler = handler;
}

void GuitarNotifier::notifyHandlers(const GuitarC &guitar)
{
    delete lastState;
    lastState = state;
    state = new GuitarState(guitar);

    // Detect pressed and release buttons and notify handlers
    for (int i = 0; i < 10; i++)
    {
        if (!lastState->isPressed(i) && state->isPressed(i))
        {
            if (pressedHandler != NULL) {
                pressedHandler(i);
            }
        }
        if (lastState->isPressed(i) && !state->isPressed(i))
        {
            if (releasedHandler != NULL) {
                releasedHandler(i);
            }
        }
    }    
}
