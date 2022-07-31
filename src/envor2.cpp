#include <Arduino.h>

#define IN_GATE_A  A0
//#define IN_GATE_A  A0
#define IN_CV_A    A7
#define IN_KNOB_A  A1
#define IN_KNOB_B  A2
#define IN_KNOB_C  A4
#define IN_KNOB_D  A3
#define IN_KNOB_E  A5

unsigned long last_ticked = 0;

const float global_inversion = 1.0f;
const float global_offset = 0.0f;

#define TIME_MULT 50  // larger = faster envelopes
#define TIME_BETWEEN_UPDATES  5  // ms to wait between updating envelopes

unsigned long effective_TIME_MULT = TIME_MULT;
unsigned long effective_TIME_BETWEEN_UPDATES = TIME_BETWEEN_UPDATES;

unsigned long long bpm_clock () {
  return millis() * effective_TIME_MULT; //millis();
}

#include "MCP_DAC.h"
#include "weirdolope_oo_class.hpp"
#include "parameter_inputs/DigitalParameterInput.h"
#include "voltage_sources/ArduinoPinVoltageSource.h"
#include "MCP.h"

#include "Config.hpp"

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println(__FILE__);

  // start the MCP hardware output -- need to do this before envelopes are started!
  setup_mcp();

  setup_envelopes();
  setup_gates();
  setup_parameters();
  
  //////// start the envelopes (ie send initial value)
  /*for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
    envelopes[i]->begin();
  }*/

  Serial.println(F("\nDone..."));
}

void loop() {
  // put your main code here, to run repeatedly:

  //Serial.println(F("loop()===>"));

  if (Serial.available() > 0) {
    // read the incoming byte:
    int incomingByte = Serial.read();
    if (incomingByte=='R') {
      Serial.println(F("Resetting!"));
      MCP.shutDown();
      MCP.reset();
      setup_mcp();

      for (unsigned int a = 0 ; a < MCP.maxValue() ; a++) {
        callback_a(random(0,MCP.maxValue()-a), true);
        callback_b(random(0,a), true);
      }
      
      envelopes[0]->begin();
      envelopes[1]->begin();
    } else if (incomingByte=='A' || incomingByte=='B' || incomingByte=='C' || incomingByte=='D') {
      bool found = false;
      for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
        if (envelopes[i]->matches_label(incomingByte)) {
          Serial.print(F("toggling debug on "));
          Serial.println((char)incomingByte);
          envelopes[i]->setDebug();
          found = true;
        }
      }
      if (!found) {
        Serial.print(F("Didn't find an enveloped named "));
        Serial.println((char)incomingByte);
      }
    /*} else if (incomingByte=='M' || incomingByte=='N' || incomingByte=='O' || incomingByte=='P') {
      bool found = false;
      for (int i = 0 ; i < NUM_GATE_INPUTS ; i++) {
        if (gate_inputs[i].matches_label(incomingByte)) {
          Serial.print(F("toggling debug on "));
          Serial.println((char)incomingByte);
          gate_inputs[i].setDebug();
          found = true;
        }
      }
      if (!found) {
        Serial.print(F("Didn't find a gate named "));
        Serial.println((char)incomingByte);
      }*/
    } else if (incomingByte=='W' || incomingByte=='X' || incomingByte=='Y' || incomingByte=='Z' || incomingByte=='[' || incomingByte=='\\') {
      bool found = false;
      for (int i = 0 ; i < NUM_PARAM_INPUTS ; i++) {
        if (param_inputs[i]->matches_label(incomingByte)) {
          Serial.print(F("toggling debug on "));
          Serial.println((char)incomingByte);
          param_inputs[i]->toggleDebug();
          found = true;
        }
      }
      if (!found) {
        Serial.print(F("Didn't find a paramter named "));
        Serial.println((char)incomingByte);
      }
    } 
  }

  for (int i = 0 ; i < NUM_PARAM_INPUTS ; i++) {
     //if (param_inputs[i]) 
       param_inputs[i]->loop();
  }

  for (int i = 0 ; i < NUM_GATE_INPUTS ; i++) {
    //if (gate_inputs[i]!=NULL)
      gate_inputs[i]->loop();
  }

  for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
    //if (envelopes[i]!=NULL) {
      //Serial.print("loop calling updateEnvelope() against ");
      //Serial.println((uint32_t) &envelopes[i]);
      envelopes[i]->updateEnvelope();
    //}
  }

  //Serial.println(F("<===loop()"));
  
}
