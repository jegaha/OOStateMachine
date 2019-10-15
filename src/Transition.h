#pragma once

#include "State.h"

class Transition
{
  public:

   static const int TRIGGER_LAMBDA = 0;
   static const int TRIGGER_NO_TRIGGER = 0;

    Transition(State *fromState, State *toState, int trigger, long lambdaTimeout = 0)
    : fromState(fromState)
    , toState(toState)
    , trigger(trigger)
    , lambdaTimeout(lambdaTimeout)
    {};

    virtual void onTransition() { /* no operation in base class */ };

    State *fromState;
    State *toState;
    int trigger;
    long lambdaTimeout;

};
