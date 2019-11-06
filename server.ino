#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <time.h>
#include <ArduinoJson.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

//const char* ssid = "U+Net4B3C";
//const char* password = "DD61044260";

const char* ssid = "bomin의 iPhone";
const char* password = "bomin0512";

//const char* ssid = "TOMNTOMS SM";
//const char* password = "sm9958828";

//ESP8266WebServer server(8080);
IPAddress server{52, 78, 37, 78};
const int led = 13;
const char* host = "http://52.78.37.78";
WiFiClient client;
int httpPort = 80;

/* time setting */
int timezone = 3;
int dst = 0;
unsigned long previousMillis = 0;
long interval = 20000;

bool ConnectSetup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  client.connect(server, httpPort);
  //Serial.println(client.connected());

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for time");
  while (!time(nullptr)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("");
}

void SendDataToServer(double gpsValue[]) {
  /* POST DATA set */
  String jsonData = " ";
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["xLocationInfo"]  = gpsValue[0];
  root["yLocationInfo"] = gpsValue[1];

  /* 미세먼지 수치 제대로 안들어오면 5번까지 체크 */
  int dustCnt = 0;
  while (dustCnt++ < 5) {
    root["dust"] = ReadDust();
    if (root["dust"] >= 0) {
      break;
    }
  }
  /* 미세먼지 수치 제대로 측정 안되면 interval 10초로 줘서 다시 측정할 수 있게 함. 성공하면 1분으로 설정해서 분 당 한번만 전송하게 함 */
  if (root["dust"] == -1){
    interval = 10000;
    return;
  }
  else{
    interval = 60000;
  }
  root.printTo(jsonData);
  //Serial.println(jsonData);
  SendHttp(jsonData);
}

void SendHttp(String jsonData) {
  client.connect(server, httpPort);
  //Serial.println(client.connected());
  if (client.connected()) {
    client.println("POST /wannaGoOut/api/dust/add HTTP/1.1");
    client.println("Host: 52.78.37.78");
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(jsonData.length());
    client.println();
    client.println(jsonData);
    Serial.println(jsonData);
  } else {
    Serial.println("connection failed");
    client.connect(server, httpPort);
  }
  delay(1000);
  String rcv = " ";
  while (client.connected() && client.available()) {
    rcv = client.readStringUntil('\r');
    Serial.println(rcv);
  }
  // rcv는 왜 success와 같지 않은가,,, 
}

bool CheckTime() {
  unsigned long currentMillis = millis();
  bool timeCheck = false;
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    time_t now = time(nullptr);
    String nowString = ctime(&now);
    String nowMinute = nowString.substring(14, 16);
    String nowYear = nowString.substring(20, 24);
    if (nowYear.toInt() <= 1970) {
      timeCheck = false;
      return timeCheck;
    }
    Serial.print("currentMin : ");
    Serial.println(nowMinute);
    Serial.print("interval : ");
    Serial.println(interval);
    /* 10분 마다 한번 씩 측정하게 함 */
    if (nowMinute.toInt() % 10 == 0) {
      timeCheck = true;
    }
    else{
      interval = 20000;
    }
  }
  return timeCheck;
}
