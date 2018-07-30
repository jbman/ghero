#include "ghero.h"

GuitarState::GuitarState(const GuitarC& guitar) 
{
    buttonBitset = guitar.buttonBitset();
}

bool GuitarState::isPressed(unsigned int position) 
{
    return ((buttonBitset >> position) & 1) == 1;
}
