bool chk;
void setup() {
  DustSetup();
  ConnectSetup();
  GpsSetup();
}

void loop() {
  if(chk = CheckTime()){
    while(!ReadGpsValue());
  }
}
