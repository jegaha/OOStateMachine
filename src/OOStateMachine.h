#pragma once

#include <Arduino.h>
#include "State.h"
#include "Transition.h"

class OOStateMachine
{
  public:

    OOStateMachine(State *initialState, Transition *transitions[], int numOfTransitions)
    : initialized(false)
    , currentState(initialState)
    , transitions(transitions)
    , numOfTransitions(numOfTransitions)
    {
    }

    void run()
    {
      int triggerThrownByOnRun = 0;
      bool triggerHasAccomplishedTransition = false;

      if (!initialized)
      {
        initialized = true;
        enterState(currentState);
      }

      triggerThrownByOnRun = currentState->onRun();

      if (triggerThrownByOnRun)
      {
        triggerHasAccomplishedTransition = this->trigger(triggerThrownByOnRun);
      }

      if (!triggerHasAccomplishedTransition)
      {
        this->handleLambdas();
      }
    }

    bool trigger(int trigger)
    {
      for(int i=0; i<numOfTransitions; i++)
      {
        if (this->isMatchingTransition(transitions[i], trigger))
        {
          this->transit(transitions[i]);
          return true;
        }
      }
      return false;
    }

    State* getCurrentState()
    {
        return this->currentState;
    }

  protected:

    void enterState(State *state)
    {
        currentState = state;
        currentStateEnteredAt = millis();
        currentState->onEnter();
    }

    void exitState()
    {
      currentState->onExit();
    }

    void transit(Transition *transition)
    {
          this->exitState();
          transition->onTransition();
          this->enterState(transition->toState);
    }

    void handleLambdas()
    {
      for(int i=0; i<numOfTransitions; i++)
      {
        if (this->isTimedOutTransition(transitions[i]))
        {
          this->transit(transitions[i]);
          break;
        }
      }
    }

    bool isMatchingTransition(Transition *transition, int trigger)
    {
      return transition->trigger == trigger
        && transition->fromState == currentState;
    }

    bool isTimedOutTransition(Transition *transition)
    {
      return transition->trigger == Transition::TRIGGER_LAMBDA
        && transition->fromState == currentState
        && millis() - currentStateEnteredAt >= transition->lambdaTimeout;
    }

    bool initialized;
    State *currentState;
    unsigned long currentStateEnteredAt;
    Transition **transitions;
    int numOfTransitions;

};
