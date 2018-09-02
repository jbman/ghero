#include "ghero.h"
#include "ghero_events.h"

void GuitarEventsC::onPressed(void (*handler)(int, State, State))
{
    pressedHandler = handler;
}

void GuitarEventsC::onReleased(void (*handler)(int, State, State))
{
    releasedHandler = handler;
}

void GuitarEventsC::onPlusMinus(void (*handler)(PlusMinusAction, State, State))
{
    plusMinusHandler = handler;
}

void GuitarEventsC::onStrum(void (*handler)(Strum, State, State))
{
    strumHandler = handler;
}

void GuitarEventsC::onWhammy(void (*handler)(int, State, State))
{
    whammyHandler = handler;
}

void GuitarEventsC::onJoystick(void (*handler)(int, int, State, State))
{
    joystickHandler = handler;
}

void GuitarEventsC::newState(State state)
{
    lastState_ = state_;
    state_ = state;

    // Detect pressed and release buttons
    for (int i = 0; i < 10; i++) {
        if (!lastState_.pressed(i) && state_.pressed(i)) {
            if (pressedHandler != NULL) {
                pressedHandler(i, state_, lastState_);
            }
        }
        if (lastState_.pressed(i) && !state_.pressed(i)) {
            if (releasedHandler != NULL) {
                releasedHandler(i, state_, lastState_);
            }
        }
    }

    // Strum
    if (!lastState_.strumDown() && state_.strumDown()) {
        strumHandler(Strum::DOWN, state_, lastState_);
    }
    if (!lastState_.strumUp() && state_.strumUp()) {
        strumHandler(Strum::UP, state_, lastState_);
    }
    if ((lastState_.strumUp() && !state_.strumUp()) ||
        (lastState_.strumDown() && !state_.strumDown())) {
        strumHandler(Strum::NONE, state_, lastState_);
    }

    // Plus/Minus buttons
    if (!lastState_.plus() && state_.plus()) {
        plusMinusHandler(PlusMinusAction::PLUS_PRESSED, state_, lastState_);
    }
    if (lastState_.plus() && !state_.plus()) {
        plusMinusHandler(PlusMinusAction::PLUS_RELEASED, state_, lastState_);
    }
    if (!lastState_.minus() && state_.minus()) {
        plusMinusHandler(PlusMinusAction::MINUS_PRESSED, state_, lastState_);
    }
    if (lastState_.minus() && !state_.minus()) {
        plusMinusHandler(PlusMinusAction::MINUS_RELEASED, state_, lastState_);
    }

    // Joystick
    if (lastState_.joystick_x() != state_.joystick_x()  || 
        lastState_.joystick_y() != state_.joystick_y()) {
        joystickHandler(state.joystick_x(), state.joystick_y(), state_, lastState_); 
    }

    // Whammy
    int whammy = state_.whammy();
    if (lastState_.whammy() != whammy) {
        whammyHandler(whammy, state_, lastState_);
    }
}

// Define global object
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_GUITAR_EVENTS)
GuitarEventsC GuitarEvents;
#endif
