#include "firesystem.h"

FireSystem::FireSystem() {}

void FireSystem::setupFireSystem()
{
    pinMode(temp_sensor, INPUT);
    pinMode(button, INPUT);
    pinMode(led, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(fire_relay, OUTPUT);
    pinMode(smoke_sensor, INPUT);
}

void FireSystem::handleFireProtection()
{
  float temperture = getTemperature();
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

  if (isSmokeDetected && temperature > TEMPERATURE_THRESHOLD)
  {
    if (!isFireDetected)
    {
      isFireDetected = true;
      isFireDetectionUpdate = true;
    }
  }
  else
  {
    if (isFireDetected)
    {
      isFireDetected = false;
      isFireDetectionUpdate = true;
    }
  }


  if (isSmokeDetected || isEmergencyPressed || isSirenActive)
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

float FireSystem::getTemperature()
{
  int analogValue = analogRead(temp_sensor);
  float celsius = 1 / (log(1 / (4095. / analogValue - 1)) / BETA + 1.0 / 298.15) - 273.15;
  return celsius;
}

void FireSystem::activateAlarm()
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

void FireSystem::deactivateAlarm()
{
  digitalWrite(led, LOW);
  noTone(buzzer);
}

FireSystem::~FireSystem() {}
