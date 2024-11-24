#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <string>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// WIFI
const char *ssid = "Wokwi-GUEST";
const char *password = "";

// FIREBASE
#define FIREBASE_URL "https://phy00007-smart-hamgiuxe-22326-default-rtdb.firebaseio.com/"
#define FIREBASE_API_KEY "AIzaSyDT9N4qIWK179RqKhlGmR5TqUzJfP4hMvY"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPreviousMillis = 0;
bool signupOk = false;
bool ledState = false;

// MQTT BROKER
const char *mqtt_broker = "broker.emqx.io";
const char *mqtt_topic = "smart-parking-system";
const char *mqtt_username = "sps";
const char *mqtt_password = "spspwd";
const int mqtt_port = 1883;

// GATE
int in_trig = 33;
int in_echo = 34;
int servo_pin = 16;
int out_trig = 23;
int out_echo = 39;

// FIRE
int temp_sensor = 32;
int button = 35;
int led = 18;
int buzzer = 5;
int fire_relay = 17;
int smoke_sensor = 36;

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 16, 2);

Servo servo;
#define SERVO_TIMER_GROUP 1 // Sử dụng timer group 1

WiFiClient espClient;
PubSubClient client(espClient);

void setupWifi()
{
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
  }
  Serial.println("Connected to WiFi");
}

void connectToMQTT()
{
  while (!client.connected())
  {
    String client_id = "esp32-client-" + String(WiFi.macAddress());
    Serial.printf("Connecting to MQTT Broker as %s.....\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Connected to MQTT broker");
      client.subscribe(mqtt_topic);
      client.publish(mqtt_topic, "Hello from ESP32");
    }
    else
    {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 2 seconds");
      delay(2000);
    }
  }
}

void mqttCallback(char *mqtt_topic, byte *payload, unsigned int length)
{
  Serial.print("Message received on mqtt_topic: ");
  Serial.println(mqtt_topic);
  Serial.print("Message: ");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println("\n-----------------------");
}

void setupServoTimer()
{
  // Cấu hình ESP32Servo để sử dụng timer cụ thể
  ESP32PWM::allocateTimer(SERVO_TIMER_GROUP);

  // Thiết lập thông số cho servo
  servo.setPeriodHertz(50);           // Tần số tiêu chuẩn cho servo là 50Hz
  servo.attach(servo_pin, 500, 2400); // Thiết lập min/max pulse width
}

void setupFirebase()
{
  config.host = FIREBASE_URL;
  config.api_key = FIREBASE_API_KEY;

  // Sign up to Firebase
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Connected to Firebase");
    signupOk = true;
  }
  else
  {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  LCD.init();
  LCD.backlight();

  // WIFI & MQTT
  setupWifi();
  client.setServer(mqtt_broker, mqtt_port);
  client.setKeepAlive(60);
  client.setCallback(mqttCallback);
  while (!client.connected())
  {
    connectToMQTT();
  }
  client.subscribe(mqtt_topic);
  client.publish(mqtt_topic, "Hello from ESP32");

  // FIREBASE
  setupFirebase();

  // GATE
  pinMode(in_trig, OUTPUT);
  pinMode(in_echo, INPUT);
  pinMode(out_trig, OUTPUT);
  pinMode(out_echo, INPUT);

  setupServoTimer();

  // FIRE
  pinMode(temp_sensor, INPUT);
  pinMode(button, INPUT);
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(fire_relay, OUTPUT);
  pinMode(smoke_sensor, INPUT);

  LCD.setCursor(0, 0);
  LCD.print("      SMART     ");
  LCD.setCursor(0, 1);
  LCD.print(" PARKING SYSTEM ");
  delay(2000);
  LCD.clear();
}

// GATE SYSTEM
const unsigned long passingTimeout = 3000;

unsigned long startPassingTime = 0;
bool isGateInOpened = false;
bool isPassing = false;
bool isInSensorExist = false;

unsigned long startPassingOutTime = 0;
bool isGateOutOpened = false;
bool isPassingOut = false;
bool isOutSensorExist = false;

// FIRE PROTECTION SYSTEM
#define SMOKE_THRESHOLD 2047
#define TEMPERATURE_THRESHOLD 70
const float BETA = 3950;

bool isFireDetected = false;
bool isSmokeDetected = false;
bool isEmergencyPressed = false;

unsigned long pressTime = 0;
unsigned long ledActiveTime = 0;
unsigned long previousMillis = 0;
const long interval = 50;
int freq = 1000;
bool holdButton = false;

// PARKING LOT
String receivedMessage = "";
int slotLeft = 3;

void loop()
{
  client.loop();

  setupServoTimer();
  handleGateFlow(in_trig, in_echo, isGateInOpened, isInSensorExist, isPassing, startPassingTime, "Entry");
  handleGateFlow(out_trig, out_echo, isGateOutOpened, isOutSensorExist, isPassingOut, startPassingOutTime, "Exit");
  handleFireProtection();
  handleParkingLot();
  handleFirebaseAction();

  LCD.setCursor(0, 0);
  LCD.print("    WELCOME!    ");
  LCD.setCursor(0, 1);
  LCD.print("Slot Left: ");
  LCD.print(slotLeft);

  String payload = "Slot Left: " + String(slotLeft);
  static String lastPayload = "";

  if (payload != lastPayload)
  {
    client.publish(mqtt_topic, payload.c_str());
    lastPayload = payload;
  }

  delay(100);
}

void handleFirebaseAction()
{
  if (Firebase.ready() && signupOk && (millis() - sendDataPreviousMillis > 5000 || sendDataPreviousMillis == 0))
  {
    sendDataPreviousMillis = millis();
    // ledState = digitalRead(led);
    // Serial.print("Current LED state: ");
    // Serial.println(ledState);

    // if (Firebase.RTDB.setBool(&fbdo, "LED/ledState", ledState))
    // {
    //   Serial.println();
    //   Serial.print(ledState);
    //   Serial.print(" - successfully saved to: " + fbdo.dataPath());
    //   Serial.println(" (" + fbdo.dataType() + ")");
    // }
    // else
    // {
    //   Serial.println("FAILED to save: " + fbdo.errorReason());
    // }

    if (Firebase.RTDB.getBool(&fbdo, "LED/ledState"))
    {
      if (fbdo.dataType() == "boolean")
      {
        ledState = fbdo.boolData();
        Serial.println("Successfully READ from " + fbdo.dataPath() + ": " + ledState + " (" + fbdo.dataType() + ")");
        digitalWrite(led, ledState);
      }
    }
    else
    {
      Serial.println("FAILED to read: " + fbdo.errorReason());
    }
  }
}

void handleGateFlow(int sensorTrig, int sensorEcho, bool &isGateOpened, bool &isSensorExist,
                    bool &isPassing, unsigned long &startPassingTime, const char *gateType)
{
  int distance_cm = getDistance(sensorTrig, sensorEcho);

  if (distance_cm < 100 && !isGateOpened) // Car approaching gate
  {
    openGate();
    isGateOpened = true;
    Serial.print("Gate Opened - ");
    Serial.println(gateType);
  }
  else if (isGateOpened) // Car has passed the gate
  {
    if (distance_cm >= 100 && !isSensorExist)
    {
      startPassingTime = millis();
      isSensorExist = true;
    }

    if (isSensorExist)
    {
      // If the car is detected at the opposite sensor
      int oppositeDistance = (gateType == "Entry") ? getDistance(out_trig, out_echo) : getDistance(in_trig, in_echo);
      if (oppositeDistance < 100)
      {
        isPassing = true;
        Serial.print("Car detected at opposite. Waiting to clear - ");
        Serial.println(gateType);
      }
      else // Car has left the opposite sensor
      {
        unsigned long d = millis() - startPassingTime;

        if (d > passingTimeout || isPassing)
        {
          closeGate();
          isPassing = false;
          isGateOpened = false;
          isSensorExist = false;
          Serial.print("Gate Closed - ");
          Serial.println(gateType);
        }
      }
    }
  }
}

void openGate()
{
  servo.write(0);
}

void closeGate()
{
  servo.write(90);
}

long getDistance(int trig, int echo)
{
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  long duration = pulseIn(echo, HIGH);
  long distance_cm = duration * 0.034 / 2;
  return distance_cm;
}

void handleFireProtection()
{
  float temperture = getTemperature(temp_sensor);
  int smokeValue = analogRead(smoke_sensor);
  int buttonState = digitalRead(button);

  while (buttonState == HIGH)
  {
    if (digitalRead(button) == LOW)
    {
      isEmergencyPressed = !isEmergencyPressed;
      ledActiveTime = millis();
      break;
    }
  }

  if (smokeValue > SMOKE_THRESHOLD)
  {
    isSmokeDetected = true;
  }
  else
  {
    isSmokeDetected = false;
    isFireDetected = false;
  }

  if (isSmokeDetected && temperture > TEMPERATURE_THRESHOLD)
  {
    isFireDetected = true;
  }
  else
  {
    isFireDetected = false;
  }

  if (isSmokeDetected || isEmergencyPressed)
  {
    activateAlarm();
  }
  else
  {
    deactivateAlarm();
  }

  if (isFireDetected)
  {
    digitalWrite(fire_relay, HIGH);
  }
  else
  {
    digitalWrite(fire_relay, LOW);
  }
}

float getTemperature(int temp_sensor)
{
  int analogValue = analogRead(temp_sensor);
  float celsius = 1 / (log(1 / (4095. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;
  return celsius;
}

void activateAlarm()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;

    freq += 50;
    if (freq >= 1500)
    {
      freq = 1000;
    }

    tone(buzzer, freq);
  }

  if (currentMillis - ledActiveTime >= 500)
  {
    digitalWrite(led, !digitalRead(led));
    ledActiveTime = currentMillis;
  }
}

void deactivateAlarm()
{
  digitalWrite(led, LOW);
  noTone(buzzer);
}

// PARKING LOT SYSTEM
void handleParkingLot()
{
  while (Serial.available() > 0)
  {
    char incomingChar = Serial.read(); // Read each character from the buffer

    if (incomingChar == '\n')
    { // Check if the user pressed Enter (new line character)
      // Print the message
      Serial.println(receivedMessage);
      std::string message = receivedMessage.c_str();
      if (message.find("IN") != -1)
      {
        if (slotLeft > 0)
        {
          slotLeft--;
        }
      }
      else if (message.find("OUT") != -1)
      {
        if (slotLeft < 3)
        {
          slotLeft++;
        }
      }
      // Clear the message buffer for the next input
      receivedMessage = "";
    }
    else
    {
      // Append the character to the message string
      receivedMessage += incomingChar;
    }
  }
}