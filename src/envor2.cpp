#include <Arduino.h>

#include "Config.hpp"

#include "Clock.h"
#include "weirdolope_oo_class.hpp"
#include "parameter_inputs/DigitalParameterInput.h"
#include "voltage_sources/ArduinoPinVoltageSource.h"
#include "MCP.h"

#define WAIT_FOR_SERIAL

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  #ifdef WAIT_FOR_SERIAL
    while (!Serial) {
      delay(1);
    }
    /*while(1) {
      Serial.println("STARTING!");
    }*/
  #endif
  Serial.println(__FILE__);

  // start the MCP hardware output -- need to do this before envelopes are started!
  setup_mcp();

  setup_envelopes();
  Serial.println("finished setup_envelopes"); Serial.flush();
  setup_gates();
  Serial.println("finished setup_gates"); Serial.flush();
  setup_parameters();
  Serial.println("finished setup_parameters"); Serial.flush();
  
  //////// start the envelopes (ie send initial value)
  /*for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
    envelopes[i]->begin();
  }*/

  Serial.println(F("\nDone..."));
}

void loop() {
  // put your main code here, to run repeatedly:

  Serial.println(F("loop()===>"));

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
      
      envelopes[0].begin();
      envelopes[1].begin();
    } else if (incomingByte=='A' || incomingByte=='B' || incomingByte=='C' || incomingByte=='D') {
      bool found = false;
      for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
        if (envelopes[i].matches_label(incomingByte)) {
          Serial.print(F("toggling debug on "));
          Serial.println((char)incomingByte);
          envelopes[i].setDebug();
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
      envelopes[i].updateEnvelope();
    //}
  }

  //Serial.println(F("<===loop()"));
  
}
