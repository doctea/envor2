MCP4922 MCP;  // HW SPI
MCP4922 MCP2;

void setup_mcp() {
  //MCP.begin(10);
  MCP.begin(10);
  MCP.setBufferedMode(true);

  MCP2.begin(9);
  MCP2.setBufferedMode(true);
}

void callback_a (float value, bool force = false) {
  static float last_value;
  if (force || value==0.0 || value!=MCP.lastValue()) {
    //Serial.print("callback_a passed value ");
    //Serial.println(value);
    MCP.fastWriteA(/*global_offset +*/ (global_inversion * value * MCP.maxValue() * 0.9f));
  }
  last_value = value;
}

void callback_b (float value, bool force = false) {
  static float last_value;
  if (force || value==0.0 || value!=MCP.lastValue()) {
    //Serial.print("callback_b passed value ");
    //Serial.println(value);
    MCP.fastWriteB(/*global_offset +*/ (global_inversion * value * MCP.maxValue() * 0.9f));
  } 
  last_value = value;
}

void callback_c (float value, bool force = false) {
  static float last_value;
  if (force || value==0.0 || value!=MCP2.lastValue()) {
    MCP2.fastWriteA(/*global_offset +*/ (global_inversion * value * MCP2.maxValue() * 0.9f));
  } 
  last_value = value;
}

void callback_d (float value, bool force = false) {
  static float last_value;
  if (force || value==0.0 || value!=MCP2.lastValue()) {
    MCP2.fastWriteB(/*global_offset +*/ (global_inversion * value * MCP2.maxValue() * 0.9f));
  } 
  last_value = value;
}



/*
void callback_a (float value, bool force = false) {
  //Serial.print("callback_a ");
  //Serial.println(value);
  static float last_value;
  if (force || value==0.0 || value!=MCP.lastValue()) {
    MCP.fastWriteA(value * MCP.maxValue() * 0.9f);
  } //else {
    //Serial.println("Not sending to callback_a as value is same!");
  }
  last_value = value;
}*/
