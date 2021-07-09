void setup_wifi(void);

 // Outputs to serial on connect. call in loop()
 // pass it the current millis as of that loop
 // It's not necessary -- it doesn't actually do anything
 // except let you see the SSID and IP upon connect
void loop_check_wifi(int loop_millis);

bool setup_wait_wifi(int timeout_s=10);
