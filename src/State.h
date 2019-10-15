#pragma once

class State
{
  public:

    virtual void onEnter() { /* no operation in base class */ };
    virtual int  onRun()   { /* no operation in base class */ };
    virtual void onExit()  { /* no operation in base class */ };

};
