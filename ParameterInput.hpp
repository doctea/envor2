class ParameterInput {
  public:
    virtual void read() {};
    void loop() {
      read();
    }
};

class DigitalParameterInput : public ParameterInput  {
  int inputPin;
  bool lastValue = false;

  public:
    using Callback = void (*)(bool);
    Callback callback;
    DigitalParameterInput(int in_inputPin, Callback in_callback) {
      inputPin = in_inputPin;
      callback = in_callback;
      pinMode(inputPin, INPUT);
    }
  
    void read() {
      // todo: debouncing
      bool currentValue = digitalRead(inputPin);
      if (currentValue != lastValue) {
        callback(currentValue);
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
        callback(get_normal_value(currentValue));
        //return currentValue;
      }
    }
};
