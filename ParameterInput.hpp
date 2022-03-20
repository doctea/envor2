class ParameterInput {
  public:
    int inputPin;
    Envelope *target = NULL ;


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
    DigitalParameterInput(int in_inputPin, Callback in_callback) {
      inputPin = in_inputPin;
      callback = in_callback;
      pinMode(inputPin, INPUT);
    }
    DigitalParameterInput(int in_inputPin, Envelope *in_target) {
      inputPin = in_inputPin;
      target = in_target;
    }
  
    void read() {
      // todo: debouncing
      bool currentValue = digitalRead(inputPin);
      if (currentValue != lastValue) {
        if (callback != NULL)
          callback(currentValue);
        if (target != NULL)
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
  
  public:
    using Callback = void (*)(float);
    Callback callback;
    AnalogParameterInput(int in_inputPin, Callback in_callback, int sensitivity = 2) {
      inputPin = in_inputPin;
      callback = in_callback;
      pinMode(inputPin, INPUT);
    }
    AnalogParameterInput(int in_inputPin, Envelope *in_target) {
      inputPin = in_inputPin;
      target = in_target;
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
        if (callback != NULL)
          callback(get_normal_value(currentValue));
        if (target != NULL)
          target->setParamValueA(get_normal_value(currentValue));
        //return currentValue;
      }
    }
};
