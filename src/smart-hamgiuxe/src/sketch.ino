#include <Arduino.h>

// GATE
int in_trig = 23;
int in_echo = 19;
int servo = 1;
int out_trig = 3;
int out_echo = 18;

// FIRE 
int temp_sensor = 35;
int button = 34;
int led = 39;
int buzzer = 36;
int fire_relay = 32;
int smoke_sensor = 33;

String receivedMessage = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // Serial.begin(9600);
  Serial.println("Hello, ESP32!");
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100); // this speeds up the simulation
  while (Serial.available() > 0) {
    char incomingChar = Serial.read();  // Read each character from the buffer
    
    if (incomingChar == '\n') {  // Check if the user pressed Enter (new line character)
      // Print the message
      Serial.print("You sent: ");
      Serial.println(receivedMessage);
      
      // Clear the message buffer for the next input
      receivedMessage = "";
    } else {
      // Append the character to the message string
      receivedMessage += incomingChar;
    }
  }
}
