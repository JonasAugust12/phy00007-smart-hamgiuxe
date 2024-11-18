#include <Arduino.h>
#include <RTClib.h>
#include <SPI.h>

int ir_sensor1 = 2;
int ir_sensor2 = 4;
int ir_sensor3 = 7;

int led1_r = 5;
int led1_g = 3;
bool occupy_1 = false;

int led2_r = 9;
int led2_g = 6;
bool occupy_2 = false;

int led3_r = 11;
int led3_g = 10;
bool occupy_3 = false;

char daysOfTheWeek[7][12] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
char monthsOfYear[12][12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

RTC_DS1307 rtc;

void initRTC(){
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2024, 11, 19, 4, 10, 0));
  }
}

void setup() {
  // Khởi tạo cổng cho các cảm biến và đèn LED
  Serial.begin(9600);
  
  pinMode(ir_sensor1, INPUT);
  pinMode(ir_sensor2, INPUT);
  pinMode(ir_sensor3, INPUT);
  pinMode(led1_r, OUTPUT);
  pinMode(led1_g, OUTPUT);

  pinMode(led2_r, OUTPUT);
  pinMode(led2_g, OUTPUT);

  pinMode(led3_r, OUTPUT);
  pinMode(led3_g, OUTPUT);

  initRTC();
}

// Hàm in thời gian hiện tại
void printCurrentTime(int slot){
  DateTime now = rtc.now();
  Serial.print(slot);
  Serial.print(" - ");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(", ");
  Serial.print(now.day(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.year(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC); 
}


// Hàm để điều khiển đèn LED theo giá trị cảm biến
void controlLED(int slot, bool &occupy, int sensorValue, int led_red, int led_green) {
  if (sensorValue == HIGH) {
    analogWrite(led_red, 0);  // Bật đèn đỏ
    analogWrite(led_green, 255); // Tắt đèn xanh
    if (occupy == false){
      printCurrentTime(slot);
      Serial.println(" IN ");
      occupy = true;
    }
  } else {
    analogWrite(led_red, 255);   // Tắt đèn đỏ
    analogWrite(led_green, 0);// Bật đèn xanh
    if (occupy == true) {
      printCurrentTime(slot);
      Serial.println(" OUT ");
      occupy = false;
    }
  }
}

void loop() {
  // Đọc giá trị từ từng cảm biến IR
  int value1 = digitalRead(ir_sensor1);
  int value2 = digitalRead(ir_sensor2);
  int value3 = digitalRead(ir_sensor3);
  
  // Kiểm tra và điều khiển đèn LED tương ứng với từng cảm biến
  controlLED(1, occupy_1, value1, led1_r, led1_g);
  controlLED(2, occupy_2, value2, led2_r, led2_g);
  controlLED(3, occupy_3, value3, led3_r, led3_g);

  delay(100);
}

