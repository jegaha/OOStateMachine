/*

        +-------------------------+                      +--------------------------+
        |                         |    trigger: on       |                          |
        | off                     +--------------------->+ on                       |
        |                         |                      |                          |
        +-------------------------+    trigger: toggle   +--------------------------+
        |                         +--------------------->+                          |
        |                         |                      | onEnter() { ledOn(); }   |
        |                         |    trigger: toggle   |                          |
        |                         +<---------------------+ onExit() { ledoOff(); }  |
        |                         |                      |                          |
        +---+---------------------+                      +----------------------+---+
            ^                                                                   |
            |                          trigger: lambda 5000ms                   |
            +------------+------------------------------------------+-----------+
                         |                                          |
                         | onTransition() { blink led 8 times(); }  |
                         |                                          |
                         +------------------------------------------+

In this simple example a state machine with two states is defined.
Hits on the trigger button toggle the state from off to on and back.
Different to that hits on the on button only trigger a transition from off to on.
If the on state is active for longer than 5000ms a lambda transition will occur and blink the led during th transition.


*/



#include <OOStateMachine.h>

#define BTN_ON_PIN 2
#define BTN_TOGGLE_PIN 3
#define LED_PIN 13

// declare an individual State which tells a about its phases to the console
class MyLoggingState : public State
{
  public:

    MyLoggingState(String name) : name(name) {}
    void onEnter() { Serial.println("-------------------------------"); Serial.println(name + "->onEnter"); }
    void onRun() { Serial.println(name + "->onRun"); }
    void onExit() { Serial.println(name + "->onExit"); }

    String name;

};


// declare an individual state
// In real world applications it would directly inherit from State
class MyOnState : public MyLoggingState
{
  public:

    MyOnState(String name) : MyLoggingState(name) {}
    
    void onEnter()
    { 
      MyLoggingState::onEnter();
      digitalWrite(LED_PIN, HIGH);
    }
    void onExit()
    {
      MyLoggingState::onExit();
      digitalWrite(LED_PIN, LOW);
    }

};

// define some states
MyLoggingState off(String("off"));
MyOnState on(String("on"));

// decrlare some triggers
// keep in mind the the id 0 is not allowed!

#define TRIGGER_ON 1
#define TRIGGER_TOGGLE 2

// declare a blink out transition
class BlinkOutTransition : public Transition
{
  public:

    BlinkOutTransition(State *fromState, State *toState, int trigger, long timeout)
    : Transition(fromState, toState, trigger, timeout)
    {}
    
    void onTransition() 
    {
      Serial.print("BlinkOutTransition ");
      
      for (int i=0; i<8; i++)
      {
         Serial.print(".");
         digitalWrite(LED_PIN, HIGH);
         delay(50);
         digitalWrite(LED_PIN, LOW);
         delay(50);
      }

      Serial.println();
    }

};

Transition *transitions[] = {
  new Transition(&off, &on, TRIGGER_ON),
  new Transition(&off, &on, TRIGGER_TOGGLE),
  new Transition(&on, &off, TRIGGER_TOGGLE),
  new BlinkOutTransition(&on, &off, Transition::TRIGGER_LAMBDA, 5000)
};

// put it all together
//OOStateMachine oosm(&off, transitions, 4);
OOStateMachine oosm(&off, transitions, sizeof(transitions)/sizeof(*transitions));

// flags to store detected interrupts
bool onButtonPressed, toggleButtonPressed;

// interrupt handlers 
void handleOnPinInterrupt() { onButtonPressed = true; }
void handleTogglePinInterrupt() { toggleButtonPressed = true; }

void setup() {

  // start serial
  Serial.begin(115200);
  Serial.println("setup");

  // setup io hardware
  pinMode(BTN_ON_PIN, INPUT_PULLUP); 
  pinMode(BTN_TOGGLE_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  // setup interrups and flags
  onButtonPressed = toggleButtonPressed = false;
  attachInterrupt(digitalPinToInterrupt(BTN_ON_PIN), handleOnPinInterrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_TOGGLE_PIN), handleTogglePinInterrupt, RISING);
   
}

void loop() {

  // check the button flags and generate the trigger events on the oosm
  if (onButtonPressed)
  {
    onButtonPressed = false;
    oosm.trigger(TRIGGER_ON);
  }

  if (toggleButtonPressed)
  {
    toggleButtonPressed = false;
    oosm.trigger(TRIGGER_TOGGLE);
  }

  // run the oosm to let the states do their work
  oosm.run();
  
  delay(300);

}
