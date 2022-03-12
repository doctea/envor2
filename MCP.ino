void setup_mcp() {
  MCP.begin(10);
}


void callback_a (float value) {
  //Serial.print("callback_a ");
  //Serial.println(value);
  static float last_value;
  if (value!=last_value)
    MCP.fastWriteA(value * MCP.maxValue());
}

void callback_b (float value) {
  //Serial.print("callback_b ");
  //Serial.println(value);
  static float last_value;
  if (value!=last_value)
    MCP.fastWriteB(value * MCP.maxValue());
}
