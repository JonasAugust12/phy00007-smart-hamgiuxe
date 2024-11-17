#include <ESP32Servo.h>
#include <LiquidCrystal_I2C.h>

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
int smoke_sensor = 19;

LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 16, 2);
String receivedMessage = "";
Servo servo;

void setup()
{
  // put your setup code here, to run once:
  // Serial.begin(115200);
  Serial.begin(9600);
  Serial.println("Hello, ESP32!");
  LCD.init();
  LCD.backlight();

  // GATE
  pinMode(in_trig, OUTPUT);
  pinMode(in_echo, INPUT);
  pinMode(out_trig, OUTPUT);
  pinMode(out_echo, INPUT);
  servo.attach(servo_pin, 500, 2400);

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

const unsigned long passingTimeout = 5000;

unsigned long startPassingTime = 0;
bool isGateInOpened = false;
bool isPassing = false;
bool isInSensorExist = false;

unsigned long startPassingOutTime = 0;
bool isGateOutOpened = false;
bool isPassingOut = false;
bool isOutSensorExist = false;

int slotLeft = 10;

void loop()
{
  handleGateFlow(in_trig, in_echo, isGateInOpened, isInSensorExist, isPassing, startPassingTime, "Entry");
  handleGateFlow(out_trig, out_echo, isGateOutOpened, isOutSensorExist, isPassingOut, startPassingOutTime, "Exit");

  LCD.setCursor(0, 0);
  LCD.print("    WELCOME!    ");
  LCD.setCursor(0, 1);
  LCD.print("Slot Left: ");
  LCD.print(slotLeft);

  delay(100);
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