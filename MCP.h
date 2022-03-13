MCP4922 MCP;  // HW SPI

void setup_mcp() {
  MCP.begin(10);
}

void callback_a (float value, bool force = false) {
  //Serial.print("callback_a ");
  //Serial.println(value);
  static float last_value;
  if (value==0.0 || value!=last_value)
    MCP.fastWriteA(value * MCP.maxValue());
}

void callback_b (float value, bool force = false) {
  //Serial.print("callback_b ");
  //Serial.println(value);
  static float last_value;
  if (force || value==0.0 || value!=last_value)
    MCP.fastWriteB(value * MCP.maxValue());
}
