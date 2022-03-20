#define NUM_ENVELOPES      4
#define NUM_GATE_INPUTS    1
#define NUM_PARAM_INPUTS   4

void multi_trigger_on();
void multi_trigger_off();

Envelope        envelopes[NUM_ENVELOPES] = {
  Envelope(), 
  Envelope(),
  Envelope(),
  Envelope(),
};
AnalogGateInput gate_inputs[NUM_GATE_INPUTS] = {
  /*AnalogGateInput(IN_GATE_A, envelopes[0]),       // this works but uses up a lot of -- almost too much -- memory
  AnalogGateInput(IN_GATE_A, envelopes[2]),
  AnalogGateInput(IN_GATE_A, envelopes[3]),*/
  AnalogGateInput(IN_GATE_A, &multi_trigger_on, &multi_trigger_off),
};
AnalogParameterInput  param_inputs[NUM_PARAM_INPUTS] = {
  //NULL, NULL
  AnalogParameterInput(IN_KNOB_B, envelopes[0]),
  AnalogParameterInput(IN_KNOB_A, envelopes[1]),
  AnalogParameterInput(IN_KNOB_C, envelopes[2]),
  AnalogParameterInput(IN_KNOB_D, envelopes[3]),
};


// use these function insteads of adding multple GateInput objects to reduce memory usage
void multi_trigger_on() {
  envelopes[0].gate_on();
  envelopes[2].gate_on();
  envelopes[3].gate_on();
}
void multi_trigger_off() {
  envelopes[0].gate_off();
  envelopes[2].gate_off();
  envelopes[3].gate_off();
}


void setup_envelopes() {
  
  // set up envelopes
  for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
    //envelopes[i] = new Envelope();

      Serial.print(F("in setup_envelopes, got envelope "));
      Serial.print(i);
      Serial.print(F(" named "));
      Serial.print(envelopes[i].name);
      Serial.print(F(" address "));
      Serial.println((uint32_t) &envelopes[i]);
  }


  // for envelope 0, use mcp callback_a, trigger envelope 1 when reaching RELEASE stage, turn off envelope 1 when reach SUSTAIN mode
  envelopes[0].registerSetterCallback(&callback_a);
  envelopes[0].registerStateChangeCallback([](int old_stage, int new_stage) {
    //Serial.print("State changed from ");
    //Serial.print(old_stage);
    //Serial.print(" to ");
    //Serial.println(new_stage);
    
    if (new_stage==Envelope::ENVELOPE_STATE_RELEASE) {        // if envelope 0 changes to RELEASE stage, trigger envelope 1
      //Serial.println("trigger envelope 1");
      envelopes[1].gate_on();
    } else if (new_stage==Envelope::ENVELOPE_STATE_SUSTAIN) { // if envelope 0 changes to SUSTAIN stage, stop envelope 1 (go straight to IDLE with slew if enabled)
      //Serial.println("trigger envelope 1 OFF");
      //envelopes[1]->gate_off();
      envelopes[1].stop();
    } else if (new_stage==Envelope::ENVELOPE_STATE_IDLE) {    // if envelope 0 changes to IDLE stage, send envelope 1 to the invert_release stage
      envelopes[1].invert_release(); //|| new_stage==Envelope::ENVELOPE_STATE_IDLE
    }
  });

  // for envelope 1, use mcp callback_b, and invert output
  envelopes[1].registerSetterCallback(&callback_b);
  envelopes[1].setInverted(true);
  //envelopes[1]->setIdleSlew(1000.0*1000.0);
  envelopes[1].setSlewRate(1000000.0005f); //setIdleSlew(1000.0*1000.0);

  envelopes[2].registerSetterCallback(&callback_c);
  envelopes[3].registerSetterCallback(&callback_d);
  envelopes[3].setInverted();
  envelopes[3].setSlewRate(10000.0005f);

}

void setup_gates() {
  /////// set up gate inputs
  //gate_inputs[0] = new AnalogGateInput(IN_GATE_A, &envelopes[0]->gate_on, &envelopes[0]->gate_off); // alternate callback method using functions
  // analog input on pin IN_GATE_A sends gate_on / gate_off to envelope 0
  //gate_inputs[0] = new AnalogGateInput(IN_GATE_A, envelopes[0]);

  for (int i = 0 ; i < NUM_GATE_INPUTS ; i++) {
    Serial.print(F("in setup_gates, got GateInput "));
    Serial.print(i);
    Serial.print(F(" named "));
    Serial.print(gate_inputs[i].name);
    Serial.print(F(" at address "));
    Serial.println((uint32_t) &gate_inputs[i]);
  }
}

void setup_parameters() {

  param_inputs[3].setInverted(true);

  for (int i = 0 ; i < NUM_PARAM_INPUTS ; i++) {
    Serial.print(F("in setup_parameters, got ParameterInput "));
    Serial.print(i);
    Serial.print(F(" named "));
    Serial.print(param_inputs[i].name);
    Serial.print(F(" at address "));
    Serial.print((uint32_t) &param_inputs[i]);
    if (param_inputs[i].inverted) Serial.print(F(" (is inverted)"));
    Serial.println();
  }
  //////// set up parameter controls
  // for envelope 0, set ParamValueA when analogue input on pin IN_KNOB_B changes
  /*param_inputs[0] = new AnalogParameterInput(IN_KNOB_B, [](float normal) {
    envelopes[0]->setParamValueA(normal);
  });
  
  // for envelope 1, set ParamValueA when analogue input on pin IN_KNOB_A changes
  param_inputs[1] = new AnalogParameterInput(IN_KNOB_A, [](float normal) {
    envelopes[1]->setParamValueA(normal);
  });*/
  //param_inputs[0] = new AnalogParameterInput(IN_KNOB_B, envelopes[0]);
  //param_inputs[1] = new AnalogParameterInput(IN_KNOB_A, envelopes[1]);
}
