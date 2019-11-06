#include <SoftwareSerial.h>
#include <ArduinoJson.h>

SoftwareSerial gps(4, 5);   //tx, rx를 각각의 핀에 연결
String str = "";
String targetStr = "GPGGA";
char c = ' ';
double gpsValue[2];
bool checkGps = false;
void GpsSetup() {
  Serial.println("Gps Starting....");
  gps.begin(9600);
}

int ReadGpsValue() {
  if (gps.available()) {
    c = gps.read();
    if (c == '\n') {
      if (targetStr.equals(str.substring(1, 6))) {
        Serial.println(str);

        // , 를 토큰으로서 파싱.
        int first = str.indexOf(",");
        int two = str.indexOf(",", first + 1);
        int three = str.indexOf(",", two + 1);
        int four = str.indexOf(",", three + 1);
        int five = str.indexOf(",", four + 1);

        // Lat과 Long 위치에 있는 값들을 index로 추출
        String Lat = str.substring(two + 1, three);
        String Long = str.substring(four + 1, five);

        // Lat의 앞값과 뒷값을 구분
        String Lat1 = Lat.substring(0, 2);
        String Lat2 = Lat.substring(2);

        // Long의 앞값과 뒷값을 구분
        String Long1 = Long.substring(0, 3);
        String Long2 = Long.substring(3);

        // 좌표 계산.
        double LatF = Lat1.toDouble() + Lat2.toDouble() / 60;
        float LongF = Long1.toFloat() + Long2.toFloat() / 60;

        // 좌표 출력.
        Serial.print("Lat : ");
        Serial.println(LatF, 15);
        Serial.print("Long : ");
        Serial.println(LongF, 15);
        if (LatF == 0 || LongF == 0) {
          str = "";
          checkGps = false;
          return checkGps;
        }

        gpsValue[0] = LatF;
        gpsValue[1] = LongF;
        SendDataToServer(gpsValue);
        checkGps = true;
      }
      str = "";

    } else {
      str += c;
      checkGps = false;
    }
  }
  return checkGps;
}
