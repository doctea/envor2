#include "MCP_DAC.h"


#define IN_GATE     A0
#define IN_ATTACK   A3
#define IN_DECAY    A4
#define IN_SUSTAIN  A2
#define IN_RELEASE  A1


bool last_a0_state = false;
bool triggerState = false;      // the current state of the output pin
bool buttonState = false;             // the current reading from the input pin
bool lastButtonState = LOW;

bool triggered = false;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 5;    // the debounce time; increase if the output flickers

unsigned long last_ticked = 0;


long bpm_clock () {
  return micros() / 4; //millis();
}

#include "weirdolope_oo_class.hpp"


MCP4922 MCP;  // HW SPI

Envelope *envelopes[2];

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println(__FILE__);

  envelopes[0] = new Envelope();
  envelopes[1] = new Envelope();

  envelopes[0]->setCallback(&callback_a);
  envelopes[1]->setCallback(&callback_b);
  envelopes[1]->setInverted(true);

  setup_mcp();

  pinMode(IN_GATE,    INPUT);
  pinMode(IN_ATTACK,  INPUT);
  pinMode(IN_RELEASE, INPUT);

  Serial.println("\nDone...");

}

void loop() {
  // put your main code here, to run repeatedly:

  int areading = analogRead(IN_GATE); //>=768;
  //Serial.print("areading is ");
  //Serial.println(areading);
  
  bool reading = areading>=768;
  /*if (reading) {
    Serial.println("high?");
  }*/

  bool changed = false;

  //return;
  
  // If the switch changed, due to noise or pressing:
  if (reading != last_a0_state) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  last_a0_state = reading;

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      //Serial.print("changed to ");
      //Serial.println(reading);
      buttonState = reading;
      changed = true;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == true) {
        triggered = true;
      } else {
        triggered = false;
      }   
    }
  }

  if (changed) {
    if (triggered) {
      Serial.println("==== Gate start");
      //update_envelope(0, 127, true);
      //update_envelope(1, 127, false);
      envelopes[0]->gate_on();
      envelopes[1]->gate_on();
    } else {
      Serial.println("==== Gate stop");
      //update_envelope(0, 0, false);
      envelopes[0]->gate_off();
      envelopes[1]->gate_off();
      //update_envelope(1, 0, false);
    }
  }

  static int knob_a;
  static int knob_b;
  int new_knob_a = analogRead(IN_RELEASE);
  int new_knob_b = analogRead(IN_SUSTAIN);
  if (abs(new_knob_a - knob_a)>2) {
    Serial.print("Changed knob_a from ");
    Serial.print(knob_a);
    Serial.print("\tto ");
    Serial.print(new_knob_a);
    Serial.println();
    knob_a = new_knob_a;
  }
  if (abs(new_knob_b - knob_b)>2) {
    Serial.print("Changed knob_b from ");
    Serial.print(knob_b);
    Serial.print("\tto ");
    Serial.print(new_knob_b);
    Serial.println();
    knob_b = new_knob_b;
  }

  envelopes[0]->updateEnvelope(knob_a);
  envelopes[1]->updateEnvelope(knob_b);
  
}
