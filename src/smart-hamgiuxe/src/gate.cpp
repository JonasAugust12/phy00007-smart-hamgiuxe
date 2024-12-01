#include "gate.h"

Gate::Gate() {}

void Gate::setupServoTimer()
{
    // Cấu hình ESP32Servo để sử dụng timer cụ thể
    ESP32PWM::allocateTimer(SERVO_TIMER_GROUP);

    // Thiết lập thông số cho servo
    servo.setPeriodHertz(50);           // Tần số tiêu chuẩn cho servo là 50Hz
    servo.attach(servo_pin, 500, 2400); // Thiết lập min/max pulse width
}

void Gate::setupGate()
{
    pinMode(in_trig, OUTPUT);
    pinMode(in_echo, INPUT);
    pinMode(out_trig, OUTPUT);
    pinMode(out_echo, INPUT);
    setupServoTimer();
}

void Gate::handleGate()
{
    setupServoTimer();
    if (isGateActive){
      isCarEntered = handleGateFlow(in_trig, in_echo, isGateInOpened, isInSensorExist, isPassing, startPassingTime, "Entry");
      handleGateFlow(out_trig, out_echo, isGateOutOpened, isOutSensorExist, isPassingOut, startPassingOutTime, "Exit");
    }
}

bool Gate::handleGateFlow(int sensorTrig, int sensorEcho, bool &isGateOpened, bool &isSensorExist,
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
            carCountTotal += 1;
            return true;
          }
        }
      }
    }
  }
  return false;
}

void Gate::openGate()
{
  servo.write(0);
}

void Gate::closeGate()
{
  servo.write(90);
}

long Gate::getDistance(int trig, int echo)
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

// FIREBASE

void Gate::updateCarCount(
  void (*operation)(AsyncClientClass, const String, object_t, AsyncResultCallback, const String),
  AsyncClientClass &aClient, 
  const String path, 
  AsyncResultCallback callback, 
  const String dbSecret
)
{
  JsonWriter writer;
  object_t CAR, carInDay, carInMonth;

  writer.create(carInDay, "carInDay", carCountInDay);
  writer.create(carInMonth, "carInMonth", carCountInMonth);
  writer.join(CAR, 2, carInDay, carInMonth);

  operation(aClient, path, CAR, callback, dbSecret);
}


void Gate::checkCarCountReset()
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


Gate::~Gate() {}