#include "relay.h"
/*
This script is intended for a output HIGH for OFF and LOW for ON*/
namespace piscina {
  Relay::Relay(const int pin) : pin_(pin), state_(1)
  {
    pinMode(pin_,OUTPUT);
    pin_off();
  }

  Relay::Relay(const int pin, const String description) : pin_(pin), state_(1), description_(description)
  {
    pinMode(pin_,OUTPUT);
    pin_off();
  }

  Relay::~Relay()
  {
      // intentionally empty
  }

  const int Relay::get_pin()
  {
      return pin_;
  }

  const int Relay::get_state()
  {
      return state_;
  }

  const String Relay::get_desciption()
  {
    return description_;
  }

  void Relay::check_state()
  {
    state_ = digitalRead(pin_);
  }

  void Relay::pin_on()
  {
    digitalWrite(pin_,LOW);
  }

  void Relay::pin_off()
  {
    digitalWrite(pin_,HIGH);
  }


  void Relay::set_state()
  {
      if(state_)
      {
        state_ = 0;
        pin_on();
      }
      else
      {
        state_ = 1;
        pin_off();
      }
  }


  void Relay::set_state(const int new_state)
  {
      state_ = new_state;
      if(state_)
      {
        pin_off();
      }
      else
      {
        pin_on();
      }
  }

  void Relay::set_description(const String description)
  {
    description_ = description;
  }
}