#include <Arduino.h>

int ir_sensor1 = 2;
int ir_sensor2 = 4;
int ir_sensor3 = 7;

int led1_r = 5;
int led1_g = 3;

int led2_r = 9;
int led2_g = 6;

int led3_r = 11;
int led3_g = 10;

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
}

// Hàm để điều khiển đèn LED theo giá trị cảm biến
void controlLED(int sensorValue, int led_red, int led_green) {
  if (sensorValue == HIGH) {
    analogWrite(led_red, 0);  // Bật đèn đỏ
    analogWrite(led_green, 255); // Tắt đèn xanh
  } else {
    analogWrite(led_red, 255);   // Tắt đèn đỏ
    analogWrite(led_green, 0);// Bật đèn xanh
  }
}

void loop() {
  // Đọc giá trị từ từng cảm biến IR
  int value1 = digitalRead(ir_sensor1);
  int value2 = digitalRead(ir_sensor2);
  int value3 = digitalRead(ir_sensor3);

  // Send the sensor values via Serial
  Serial.print("Sending values: ");
  Serial.print(value1);  // Send value for sensor 1
  Serial.print(",");     // Add a separator for clarity
  Serial.print(value2);  // Send value for sensor 2
  Serial.print(",");     // Add a separator
  Serial.println(value3); // Send value for sensor 3, followed by a newline
  
  // Kiểm tra và điều khiển đèn LED tương ứng với từng cảm biến
  controlLED(value1, led1_r, led1_g);
  controlLED(value2, led2_r, led2_g);
  controlLED(value3, led3_r, led3_g);

  delay(100);
}

