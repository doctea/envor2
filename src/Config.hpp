#define NUM_ENVELOPES      4
#define NUM_GATE_INPUTS    1
#define NUM_PARAM_INPUTS   6

#include "parameter_inputs/DigitalParameterInput.h"

#define PIN_SDA SDA
#define PIN_SCL SCL

void multi_trigger(bool status);

void multi_trigger_on();
void multi_trigger_off();
void setFifthParameterValue(double normal);
void setCVParameter(double normal);

Envelope *envelopes[] = {
  new Envelope("A"), 
  new Envelope("B"),
  new Envelope("C"),
  new Envelope("D"),
};

// if using an analog pin as the gate, use this
DigitalParameterInput *gate_inputs[] = { //NUM_GATE_INPUTS] = {
  //AnalogGateInput(IN_GATE_A, envelopes[0]),       // this works but uses up a lot of -- almost too much -- memory
  //AnalogGateInput(IN_GATE_A, envelopes[2]),
  //AnalogGateInput(IN_GATE_A, envelopes[3]),
  new DigitalParameterInput(IN_GATE_A, &multi_trigger_on, &multi_trigger_off),
};

// if using a digital pin as the gate, use this
/*DigitalGateInput gate_inputs[NUM_GATE_INPUTS] = {
  DigitalGateInput(IN_GATE_A, &multi_trigger_on, &multi_trigger_off),
};*/

#include "voltage_sources/ArduinoPinVoltageSource.h"

VoltageSourceBase *voltage_sources[] = {
  new ArduinoPinVoltageSource(IN_KNOB_A),
  new ArduinoPinVoltageSource(IN_KNOB_B),
  new ArduinoPinVoltageSource(IN_KNOB_C),
  new ArduinoPinVoltageSource(IN_KNOB_D),
  new ArduinoPinVoltageSource(IN_KNOB_E),
  new ArduinoPinVoltageSource(IN_CV_A),
};

/*AnalogParameterInput param_inputs[NUM_PARAM_INPUTS] = {
  //NULL, NULL
  AnalogParameterInput(IN_KNOB_B, envelopes[0]),
  AnalogParameterInput(IN_KNOB_A, envelopes[1]),
  AnalogParameterInput(IN_KNOB_C, envelopes[2]),
  AnalogParameterInput(IN_KNOB_D, envelopes[3]),
  AnalogParameterInput(IN_KNOB_E, &setFifthParameterValue),
  AnalogParameterInput(IN_CV_A,   &setCVParameter),
};*/
#include "parameter_inputs/AnalogParameterInputBase.h"
#include "parameter_inputs/VoltageParameterInput.h"
#include "parameters/Parameter.h"
#include "parameters/FunctionParameter.h"

FunctionParameter set_fifth_parameter = FunctionParameter("Fifth parameter",  &setFifthParameterValue);
FunctionParameter set_cv_parameter =    FunctionParameter("CV parameter",     &setCVParameter);

BaseParameterInput *param_inputs[] = {
  new VoltageParameterInput<BaseParameter>('A', voltage_sources[0], 
    new Parameter<Envelope> ("Env A type", envelopes[1], &Envelope::setParamValue)
  ),
  new VoltageParameterInput<BaseParameter>('B', voltage_sources[1], 
    new Parameter<Envelope> ("Env B type", envelopes[0], &Envelope::setParamValue)
  ),
  new VoltageParameterInput<BaseParameter>('C', voltage_sources[2], 
    new Parameter<Envelope> ("Env C type", envelopes[2], &Envelope::setParamValue)
  ),
  new VoltageParameterInput<BaseParameter>('D', voltage_sources[3], 
    new Parameter<Envelope> ("End D type", envelopes[3], &Envelope::setParamValue)
  ),
  new VoltageParameterInput<BaseParameter>  ('E', voltage_sources[4], &set_fifth_parameter),
  new VoltageParameterInput<BaseParameter>  ('F', voltage_sources[5], &set_cv_parameter)
};

// use these function insteads of adding multple GateInput objects to reduce memory usage
void multi_trigger_on() {
  envelopes[0]->gate_on();
  envelopes[2]->gate_on();
  envelopes[3]->gate_on();
}
void multi_trigger_off() {
  envelopes[0]->gate_off();
  envelopes[2]->gate_off();
  envelopes[3]->gate_off();
}

float cvParameterValue = 0.0f;
float knobParameterValue = 0.0f;

void updateEffectiveTimeBetweenUpdates (float normal) {
  //float val = constrain(normal + cvParameterValue), 0.0
  if (normal>1.0f) normal = 1.0f - (normal-1.0f); //wrapround sorta thing
  effective_TIME_BETWEEN_UPDATES = map(normal*1000.0, 0, 1000, 0, 500);
}

void setSecondaryBaseLevel(float norm_value) {
  envelopes[1]->setBaseLevel(norm_value);
  envelopes[3]->setBaseLevel(norm_value);
}

void setFifthParameterValue(double normal) {
  //Serial.print("The F#fth Meat: ");
  //Serial.println(normal);
  //global_inversion = map(normal, 0.0, 1.0, -1.0f, 1.0f);
  /*global_inversion = ((float)map(normal*1000, 0, 1000, -1000, 1000))/1000.0f;
  if (global_inversion<0.0f)
    global_offset = 1.0f;
  else 
    global_offset = 0.0f;
  Serial.print("global_inversion: ");
  Serial.print(global_inversion);
  Serial.print(", global_offset: ");
  Serial.print(global_offset);*/
  //effective_TIME_MULT = map(normal*1000.0, 0, 1000, 1, 500);
  //effective_TIME_BETWEEN_UPDATES = map(normal*1000.0, 0, 1000, 0, 500);
  //updateEffectiveTimeBetweenUpdates(normal, 
  knobParameterValue = normal;
  //updateEffectiveTimeBetweenUpdates(knobParameterValue + cvParameterValue);
  if (knobParameterValue<0.5) {
    setSecondaryBaseLevel((knobParameterValue*2.0) + cvParameterValue);
  } else {
    updateEffectiveTimeBetweenUpdates(((knobParameterValue-0.5)*2.0) + cvParameterValue);
  }
}

void setCVParameter(double normal) {
  //Serial.print("setCVParameter: ");
  //Serial.println(normal);
  cvParameterValue = normal;
  //updateEffectiveTimeBetweenUpdates(knobParameterValue + cvParameterValue);
  if (knobParameterValue<0.5) {
    setSecondaryBaseLevel((knobParameterValue*2.0) + cvParameterValue);
  } else {
    updateEffectiveTimeBetweenUpdates(((knobParameterValue-0.5)*2.0) + cvParameterValue);
  }
}

void setup_envelopes() {
  
  // set up envelopes
  for (int i = 0 ; i < NUM_ENVELOPES ; i++) {
    //envelopes[i] = new Envelope();

      Serial.print(F("in setup_envelopes, got envelope "));
      Serial.print(i);
      Serial.print(F(" named "));
      Serial.print(envelopes[i]->label);
      Serial.print(F(" address "));
      Serial.println((uint32_t) envelopes[i]);
  }


  // for envelope 0, use mcp callback_a, trigger envelope 1 when reaching RELEASE stage, turn off envelope 1 when reach SUSTAIN mode
  envelopes[0]->registerSetterCallback(&callback_a);
  envelopes[0]->registerStateChangeCallback([](int old_stage, int new_stage) {
    //Serial.print("State changed from ");
    //Serial.print(old_stage);
    //Serial.print(" to ");
    //Serial.println(new_stage);
    
    if (new_stage==Envelope::ENVELOPE_STATE_RELEASE) {        // if envelope 0 changes to RELEASE stage, trigger envelope 1
      //Serial.println("trigger envelope 1");
      envelopes[1]->gate_on();
    } else if (new_stage==Envelope::ENVELOPE_STATE_SUSTAIN) { // if envelope 0 changes to SUSTAIN stage, stop envelope 1 (go straight to IDLE with slew if enabled)
      //Serial.println("trigger envelope 1 OFF");
      //envelopes[1]->gate_off();
      envelopes[1]->stop();
      envelopes[3]->invert_release();
    } else if (new_stage==Envelope::ENVELOPE_STATE_IDLE) {    // if envelope 0 changes to IDLE stage, send envelope 1 to the invert_release stage
      envelopes[1]->invert_release(); //|| new_stage==Envelope::ENVELOPE_STATE_IDLE
      envelopes[3]->invert_release();
    }
  });

  // for envelope 1, use mcp callback_b, and invert output
  envelopes[1]->registerSetterCallback(&callback_b);
  envelopes[1]->setInverted(true);
  //envelopes[1]->setIdleSlew(1000.0*1000.0);
  envelopes[1]->setSlewRate(1000000.0005f); //setIdleSlew(1000.0*1000.0);

  envelopes[2]->registerSetterCallback(&callback_c);
  envelopes[3]->registerSetterCallback(&callback_d);
  envelopes[3]->setInverted();
  envelopes[3]->setSlewRate(10000.0005f);

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
    Serial.print(gate_inputs[i]->name);
    //Serial.print(F(" with pin "));
    //Serial.print(gate_inputs[i]->inputPin);
    Serial.print(F(" at address "));
    Serial.println((uint32_t) gate_inputs[i]);
  }
}

void setup_parameters() {

  param_inputs[3]->setInverted(true);    // these are wired up back-to-front in my build, so invert them
  param_inputs[4]->setInverted(true);    // these are wired up back-to-front in my build, so invert them

  for (int i = 0 ; i < NUM_PARAM_INPUTS ; i++) {
    Serial.print(F("in setup_parameters, got ParameterInput "));
    Serial.print(i);
    Serial.print(F(" named "));
    Serial.print(param_inputs[i]->name);
    Serial.print(F(" at address "));
    Serial.print((uint32_t) &param_inputs[i]);
    if (param_inputs[i]->inverted) Serial.print(F(" (is inverted)"));
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
