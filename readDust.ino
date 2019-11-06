#include <ArduinoJson.h>

int dust_sensor = A0;   // 미세먼지 핀 번호
int rgb_red = 3;    // rgb 핀 빨간색 핀
int rgb_green = 4;  // rgb핀 녹색 핀
int rgb_blue = 5;  // rgb핀 파란색 핀

float dust_value = 0;  // 센서에서 입력 받은 미세먼지 값
float dustDensityug = 0; // ug/m^3 값을 계산

int sensor_led = 12;      // 미세먼지 센서 안에 있는 적외선 led 핀 번호
int sampling = 280;    // 적외선 led를 키고, 센서 값을 읽어 들여 미세먼지를 측정하는 샘플링 시간
int waiting = 40;
float stop_time = 9680;   // 센서를 구동하지 않는 시간

void DustSetup() {
  Serial.begin(9600);   // 시리얼 모니터 시작, 속도는 9600
  pinMode(sensor_led, OUTPUT); // 미세먼지 적외선 led를 출력으로 설정
  pinMode(rgb_red, OUTPUT);     // 3색 LED 모듈 출력으로 설정, 붉은색
  pinMode(rgb_green, OUTPUT);   // 녹색
  pinMode(rgb_blue, OUTPUT);    // 파란색
}

double ReadDust() {
  digitalWrite(sensor_led, LOW);    // LED 켜기
  delayMicroseconds(sampling);   // 샘플링해주는 시간.

  dust_value = analogRead(dust_sensor); // 센서 값 읽어오기

  delayMicroseconds(waiting);  // 너무 많은 데이터 입력을 피해주기 위해 잠시 멈춰주는 시간.

  digitalWrite(sensor_led, HIGH); // LED 끄기
  delayMicroseconds(stop_time);   // LED 끄고 대기

  dustDensityug = (0.17 * (dust_value * (5.0 / 1024)) - 0.1) * 1000;    // 미세먼지 값 계산
  if (dustDensityug < 0) {
    Serial.println("dust 측정이 안됐습니다.");
    return -1;
  }
  else {
    Serial.print("Dust Density [ug/m3]: ");            // 시리얼 모니터에 미세먼지 값 출력
    Serial.println(dustDensityug);

//    if (dustDensityug <= 30.0) {     // 대기 중 미세먼지가 좋음 일 때 파란색 출력
//      Serial.print("   ");
//      Serial.println("blue");
//    } else if (30.0 < dustDensityug && dustDensityug <= 80.0) {    // 대기 중 미세먼지가 보통 일 때 녹색 출력
//      Serial.print("   ");
//      Serial.println("green");
//    } else if (80.0 < dustDensityug && dustDensityug <= 150.0) {   // 대기 중 미세먼지가 나쁨 일 때 노란색 출력
//      Serial.print("   ");
//      Serial.println("yellow");
//    } else {                                                    // 대기 중 미세먼지가 매우 나쁨 일 때 빨간색 출력
//      Serial.print("   ");
//      Serial.println("red");
//    }
    /* POST DATA set */
    return dustDensityug;
  }

}
