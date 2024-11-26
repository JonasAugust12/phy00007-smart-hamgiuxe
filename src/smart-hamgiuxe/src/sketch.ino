#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <string>
#include <FirebaseClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

// WIFI
const char *ssid = "Wokwi-GUEST";
const char *password = "";

// FIREBASE
#define DATABASE_URL "https://phy00007-smart-hamgiuxe-22326-default-rtdb.firebaseio.com"
#define DATABASE_SECRET "FOO9eBjUDTEu5gqSYJLLlJlxHSK2GWSBaKBxq3cO"

WiFiClientSecure ssl, ssl2;
DefaultNetwork network;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl, getNetwork(network)), aClient2(ssl2, getNetwork(network));

void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);

FirebaseApp app;
RealtimeDatabase Database;
LegacyToken dbSecret(DATABASE_SECRET);

bool ledState = false;

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

// REAL TIME
#define UTC_OFFSET     7 * 3600
#define UTC_OFFSET_DST 0


// GATE SYSTEM
bool isGateActive = true;
int carCountInDay = 0;
int carCountInMonth = 0;
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

bool isKeyExist(JsonObject obj, const char* key) {
  for (auto kvp : obj) {
    if (strcmp(kvp.key().c_str(), key) == 0) {
      return true;
    }
  }
  return false;
}

void setupWifi()
{
  delay(10);
  Serial.print("Connecting to WiFi...");
  WiFi.begin(ssid, password, 6);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
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
  ssl.setInsecure();
  ssl2.setInsecure();
  initializeApp(aClient2, app, getAuth(dbSecret), asyncCB, "authTask");
  app.getApp<RealtimeDatabase>(Database);
  Database.url(DATABASE_URL);
  
  Database.get(aClient2, "/", asyncCB, false, "initTask");

  Database.setSSEFilters("put,patch");
  Database.get(aClient, "/", asyncCB, true, "streamTask");
}

void asyncCB(AsyncResult &aResult)
{
  // WARNING!
  // Do not put your codes inside the callback and printResult.

  printResult(aResult);
}

void printResult(AsyncResult &aResult)
{
  if (aResult.isEvent())
  {
    Firebase.printf("Event task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.appEvent().message().c_str(), aResult.appEvent().code());
  }

  if (aResult.isDebug())
  {
    Firebase.printf("Debug task: %s, msg: %s\n", aResult.uid().c_str(), aResult.debug().c_str());
  }

  if (aResult.isError())
  {
    Firebase.printf("Error task: %s, msg: %s, code: %d\n", aResult.uid().c_str(), aResult.error().message().c_str(), aResult.error().code());
  }

  if (aResult.available())
  {
    RealtimeDatabaseResult &RTDB = aResult.to<RealtimeDatabaseResult>();
    if (RTDB.isStream())
    {
      Serial.println("----------------------------");
      Firebase.printf("task: %s ", aResult.uid().c_str());
      Firebase.printf("event: %s ", RTDB.event().c_str());
      Firebase.printf("path: %s ", RTDB.dataPath().c_str());
      Serial.println("");

      if (RTDB.dataPath() == "/BARRIER/state")
      {
        bool barrierState = RTDB.to<bool>();
        isGateActive = barrierState;
      }



      // FETCH OTHER STREAM DATA HERE



    }
    else
    {
      Serial.println("----------------------------");
      Firebase.printf("task: %s, payload: %s\n", aResult.uid().c_str(), aResult.c_str());

      if (aResult.uid() == "initTask" || RTDB.dataPath() == "/")
      {
        String payload = RTDB.to<String>();

        JsonDocument doc;
        deserializeJson(doc, payload);

        if (isKeyExist(doc.as<JsonObject>(), "BARRIER"))
        {
          JsonObject barrier = doc["BARRIER"];
          isGateActive = barrier["state"];
        }
        if (isKeyExist(doc.as<JsonObject>(), "CAR"))
        {
          JsonObject car = doc["CAR"];
          carCountInDay = car["carInDay"];
          carCountInMonth = car["carInMonth"];
        }



        // FETCH OTHER INIT DATA HERE



      }
    }

    Firebase.printf("Free Heap: %d\n", ESP.getFreeHeap());
  }
}

void setupNTP() {
  configTime(UTC_OFFSET, UTC_OFFSET_DST, "asia.pool.ntp.org", "time.google.com", "pool.ntp.org");
  Serial.print("Syncing time...");
  struct tm timeinfo;
  for (int i = 0; i < 10; i++) { // Retry 10 times
    if (getLocalTime(&timeinfo)) {
      Serial.println("");
      Serial.println("Time synchronized successfully");
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      return;
    }
    delay(500); // Wait before retrying
    Serial.print(".");
  }
  if (time(nullptr) <= 0) {
    Serial.println(" Failed to obtain time.");
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  LCD.init();
  LCD.backlight();

  // WIFI & TIME
  setupWifi();
  setupNTP();

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
  delay(1000);
  LCD.clear();
}

unsigned long ms = 0;
void loop()
{
  checkCarCountReset();
  setupServoTimer();

  app.loop();
  Database.loop();

  bool isCarEntered = false;
  if (isGateActive)
  {
    isCarEntered = handleGateFlow(in_trig, in_echo, isGateInOpened, isInSensorExist, isPassing, startPassingTime, "Entry");
    handleGateFlow(out_trig, out_echo, isGateOutOpened, isOutSensorExist, isPassingOut, startPassingOutTime, "Exit");
  }

  if (app.ready() && isCarEntered)
  {
    updateCarCount();
  }
  handleFireProtection();
  handleParkingLot();

  LCD.setCursor(0, 0);
  LCD.print("    WELCOME!    ");
  LCD.setCursor(0, 1);
  LCD.print("Slot Left: ");
  LCD.print(slotLeft);

  String payload = "Slot Left: ";
  payload.concat(String(slotLeft));
  static String lastPayload = "";

  delay(100);
  
}

void updateCarCount()
{
  JsonWriter writer;
  object_t CAR, carInDay, carInMonth;

  writer.create(carInDay, "carInDay", carCountInDay);
  writer.create(carInMonth, "carInMonth", carCountInMonth);
  writer.join(CAR, 2, carInDay, carInMonth);

  Database.set<object_t>(aClient2, "/CAR", CAR, asyncCB, "carTask");
}

void checkCarCountReset()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }

  if (timeinfo.tm_hour == 0 && timeinfo.tm_min == 0 && timeinfo.tm_sec >= 0 && timeinfo.tm_sec <= 5)
  {
    carCountInDay = 0;
  }

  if (timeinfo.tm_mday == 1 && timeinfo.tm_hour == 0 && timeinfo.tm_min == 0 && timeinfo.tm_sec >= 0 && timeinfo.tm_sec <= 5)
  {
    carCountInMonth = 0;
  }
}

bool handleGateFlow(int sensorTrig, int sensorEcho, bool &isGateOpened, bool &isSensorExist,
                    bool &isPassing, unsigned long &startPassingTime, const char *gateType)
{
  int distance_cm = getDistance(sensorTrig, sensorEcho);
  if (distance_cm > 0 && distance_cm < 100 && !isGateOpened) // Car approaching gate
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
          if (d <= passingTimeout && gateType == "Entry")
          {
            carCountInDay += 1;
            carCountInMonth += 1;
            return true;
          }
        }
      }
    }
  }
  return false;
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