#include "MCP_DAC.h"

#define IN_GATE_A   A0
//#define IN_ATTACK   A3
//#define IN_DECAY    A4
#define IN_KNOB_A  A1
#define IN_KNOB_B  A2

unsigned long last_ticked = 0;

#define TIME_MULT 5  // larger = faster envelopes
#define TIME_BETWEEN_UPDATES  500

unsigned long bpm_clock () {
  return millis() * TIME_MULT; //millis();
}

#include "weirdolope_oo_class.hpp"
#include "GateInput.hpp"
#include "ParameterInput.hpp"
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
  for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
    envelopes[i]->begin();
  }

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

      for (int a = 0 ; a < MCP.maxValue() ; a++) {
        callback_a(random(0,MCP.maxValue()-a), true);
        callback_b(random(0,a), true);
      }
      
      envelopes[0]->begin();
      envelopes[1]->begin();
    } else if (incomingByte=='A' || incomingByte=='B' || incomingByte=='C' || incomingByte=='D') {
      bool found_envelope = false;
      for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
        if (envelopes[i]->name==incomingByte) {
          Serial.println(F("toggling debug on "));
          Serial.println(incomingByte);
          envelopes[i]->setDebug();
          found_envelope = true;
        }
      }
      if (!found_envelope) {
        Serial.print(F("Didn't find an enveloped named "));
        Serial.println((char)incomingByte);
      }
    }
  }

  //for (int i = 0 ; i < NUM_PARAM_INPUTS ; i++) {
  //   param_inputs[i]->loop();
  //}
  //param_inputs[0]->loop();

  for (int i = 0 ; i < NUM_GATE_INPUTS ; i++) {
    gate_inputs[i]->loop();
  }

  for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
    envelopes[i]->updateEnvelope();
  }

  //Serial.println(F("<===loop()"));
  
}
