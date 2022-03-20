char NEXT_PARAMETER_NAME = 'W';

class ParameterInput {
  public:
    int inputPin;
    Envelope *target;
    char name;
    bool debug = false;

    ParameterInput() {
      name = NEXT_PARAMETER_NAME++;
    }

    void setDebug() {
      debug = !debug;
    }

    virtual void read() {};
    void loop() {
      read();
    }
};

class DigitalParameterInput : public ParameterInput  {
  
  bool lastValue = false;

  public:
    using Callback = void (*)(bool);
    Callback callback;
    DigitalParameterInput(int in_inputPin, Callback in_callback) : ParameterInput() {
      inputPin = in_inputPin;
      callback = in_callback;
      pinMode(inputPin, INPUT);
    }
    DigitalParameterInput(int in_inputPin, Envelope &in_target) : ParameterInput() {
      inputPin = in_inputPin;
      target = &in_target;
    }
  
    void read() {
      // todo: debouncing
      bool currentValue = digitalRead(inputPin);
      if (currentValue != lastValue) {
        if (callback != NULL)
          callback(currentValue);
        if (target)
          target->setParamValueA(currentValue);
        lastValue = currentValue;
        //return currentValue;
      }
    }
};

class AnalogParameterInput : public ParameterInput {
  int inputPin;

  int sensitivity = 2;
  int lastValue = 0;
  byte envelope_number = 0xff;
  
  public:
    using Callback = void (*)(float);
    Callback callback;
    AnalogParameterInput(int in_inputPin, Callback in_callback, int sensitivity = 2) {
      inputPin = in_inputPin;
      callback = in_callback;
      pinMode(inputPin, INPUT);
    }
    AnalogParameterInput(int in_inputPin, Envelope &in_target) : ParameterInput() {
      inputPin = in_inputPin;
      target = &in_target;
    }
    AnalogParameterInput(int in_inputPin, byte in_envelope_number) : ParameterInput() {
      inputPin = in_inputPin;
      envelope_number = in_envelope_number;
    }
    

    void loop () {
      read();
    }

    float get_normal_value(int value) {
      return (float)value / 1023.0f;
    }

    void read() {
      int currentValue = analogRead(inputPin);
      if (abs(currentValue - lastValue) > sensitivity) {
        lastValue = currentValue;
        float normal = get_normal_value(currentValue);
        if (callback != NULL) {
          if (debug) {
            Serial.print(name);
            Serial.print(": calling callback(");
            Serial.print(normal);
            Serial.println(")");
          }      
          callback(normal);
        }
        if (target) {
          if (debug) {
            Serial.print(name);
            Serial.print(": calling target setParamValueA(");
            Serial.print(normal);
            Serial.println(")");
          }
          target->setParamValueA(normal);
        }
        //if (envelope_number!=0xff)
          //envelopes[envelope_number]->setParamValueA(get_normal_value(currentValue));
        //return currentValue;
      }
    }
};
