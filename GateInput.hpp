class GateInput {
  protected:
    int inputPin;
    using Callback = void (*)();

    Callback gate_on_callback = NULL ;
    Callback gate_off_callback = NULL ;
    Envelope *target = NULL ;
    
    bool last_read_state = false;
    bool triggerState = false;      // the current state of the output pin
    bool buttonState = false;             // the current reading from the input pin
    bool lastButtonState = LOW;
    
    bool triggered = false;
    
    unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
    unsigned long debounceDelay = 5;    // the debounce time; increase if the output flickers   

  public:
    GateInput(int in_inputPin, Envelope *in_target) {
      inputPin = in_inputPin;
      pinMode(inputPin, INPUT);

      target = in_target;
      //gate_on_callback = in_gate_on_callback;
      //gate_off_callback = in_gate_off_callback;
    }
    GateInput(int in_inputPin, Callback in_gate_on_callback, Callback in_gate_off_callback) {
      inputPin = in_inputPin;
      pinMode(inputPin, INPUT);
      gate_on_callback = in_gate_on_callback;
      gate_off_callback = in_gate_off_callback;
    }

    // read current value directly
    virtual bool read_value() {};

    void loop() {
      /*if (reading) {
        Serial.println("high?");
      }*/
      bool reading = read_value();
    
      bool changed = false;
    
      // If the switch changed, due to noise or pressing:
      if (reading != last_read_state) {
        // reset the debouncing timer
        lastDebounceTime = millis();
      }
    
      last_read_state = reading;
    
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
        //Serial.println("GateInput changed!");
        if (triggered) {
          //Serial.println("==== Gate start");
          if (gate_on_callback != NULL) {
            gate_on_callback();
          }
          if (target != NULL) {
            //Serial.print("calling gate_on against ");
            //Serial.println((uint32_t) &target);
            target->gate_on();
          }
          //envelopes[1]->gate_on();
        } else {
          //Serial.println("==== Gate stop");
          if (gate_off_callback != NULL) {
            gate_off_callback();
          }
          if (target != NULL) {
            target->gate_off();
          }
        }
      }
    }
};

class AnalogGateInput : public GateInput {

  public:
    int trigger_level = 768;

    AnalogGateInput(int in_inputPin, Envelope *target)
      : GateInput{in_inputPin, target} {}
    
    bool read_value() {
      int areading = analogRead(inputPin); //>=768;
      //Serial.print("areading is ");
      //Serial.println(areading);
      
      int reading = areading>=trigger_level;
      return reading;    
  }

};

class DigitalGateInput : public GateInput {

  public:
    DigitalGateInput(int in_inputPin, Envelope *target)
      : GateInput{in_inputPin, target } {}
    
    bool read_value() {
      return digitalRead(inputPin);
    }
};
