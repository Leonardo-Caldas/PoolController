#ifndef Relay_H
#define Relay_H

#include <Arduino.h>
namespace piscina {
  class Relay
  {
  public:
    Relay();
    Relay(const int pin);
    Relay(const int pin, const String description);
    ~Relay();

    const int get_pin();
    const int get_state();
    const String get_desciption();
    void set_state();
    void set_state(const int new_state);
    void set_description(const String description);
    void check_state();
  private:
    int pin_,state_;
    String description_;
    void pin_on();
    void pin_off();
  };

  #endif //Relay_H
}

