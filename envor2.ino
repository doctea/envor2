#include "MCP_DAC.h"

#define IN_GATE_A   A0
//#define IN_ATTACK   A3
//#define IN_DECAY    A4
#define IN_KNOB_A  A1
#define IN_KNOB_B  A2

unsigned long last_ticked = 0;

#define TIME_MULT 5  // larger = faster envelopes

long bpm_clock () {
  return micros() * TIME_MULT; //millis();
}

#include "weirdolope_oo_class.hpp"
#include "GateInput.hpp"
#include "ParameterInput.hpp"
#include "MCP.h"

#define NUM_ENVELOPES      2
#define NUM_GATE_INPUTS    1
#define NUM_PARAM_INPUTS   2

Envelope        *envelopes[NUM_ENVELOPES];
GateInput       *gate_inputs[NUM_GATE_INPUTS];
ParameterInput  *param_inputs[NUM_PARAM_INPUTS];

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  Serial.println(__FILE__);

  // start the MCP hardware output -- need to do this before envelopes are started!
  setup_mcp();

  // set up envelopes
  for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
    envelopes[i] = new Envelope();
  }

  // for envelope 0, use mcp callback_a, trigger envelope 1 when reaching RELEASE stage, turn off envelope 1 when reach SUSTAIN mode
  envelopes[0]->registerSetterCallback(&callback_a);
  envelopes[0]->registerStateChangeCallback([](int old_stage, int new_stage) {
    //Serial.print("State changed from ");
    //Serial.print(old_stage);
    //Serial.print(" to ");
    //Serial.println(new_stage);
    
    if (new_stage==Envelope::ENVELOPE_STATE_RELEASE) {
      //Serial.println("trigger envelope 1");
      envelopes[1]->gate_on();
    } else if (new_stage==Envelope::ENVELOPE_STATE_SUSTAIN) {
      //Serial.println("trigger envelope 1 OFF");
      //envelopes[1]->gate_off();
      envelopes[1]->stop();
    } else if (new_stage==Envelope::ENVELOPE_STATE_IDLE) {
      envelopes[1]->invert_release(); //|| new_stage==Envelope::ENVELOPE_STATE_IDLE
    }
  });

  // for envelope 1, use mcp callback_b, and invert output
  envelopes[1]->registerSetterCallback(&callback_b);
  envelopes[1]->setInverted(true);
  //envelopes[1]->setIdleSlew(1000.0*1000.0);
  envelopes[1]->setSlewRate(1.0005f); //setIdleSlew(1000.0*1000.0);

  //////// start the envelopes (ie send initial value)
  for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
    envelopes[i]->begin();
  }

  /////// set up gate inputs
  //gate_inputs[0] = new AnalogGateInput(IN_GATE_A, &envelopes[0]->gate_on, &envelopes[0]->gate_off); // alternate callback method using functions
  // analog input on pin IN_GATE_A sends gate_on / gate_off to envelope 0
  gate_inputs[0] = new AnalogGateInput(IN_GATE_A, envelopes[0]);

  //////// set up parameter controls
  // for envelope 0, set ParamValueA when analogue input on pin IN_KNOB_B changes
  param_inputs[0] = new AnalogParameterInput(IN_KNOB_B, [](float normal) {
    envelopes[0]->setParamValueA(normal);
  });
  
  // for envelope 1, set ParamValueA when analogue input on pin IN_KNOB_A changes
  param_inputs[1] = new AnalogParameterInput(IN_KNOB_A, [](float normal) {
    envelopes[1]->setParamValueA(normal);
  });


  Serial.println("\nDone...");

}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available() > 0) {
    // read the incoming byte:
    int incomingByte = Serial.read();
    if (incomingByte=='R') {
      Serial.println("Resetting!");
      MCP.shutDown();
      MCP.reset();
      setup_mcp();
      envelopes[0]->begin();
      envelopes[1]->begin();
    }
  }

  for (int i = 0 ; i < NUM_PARAM_INPUTS ; i++) {
    param_inputs[i]->loop();
  }

  for (int i = 0 ; i < NUM_GATE_INPUTS ; i++) {
    gate_inputs[i]->loop();
  }

  /*static int knob_a;
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
  }*/

  for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
    envelopes[i]->updateEnvelope(); //knob_a);
    envelopes[1]->updateEnvelope(); //knob_b);
  }
  
}
