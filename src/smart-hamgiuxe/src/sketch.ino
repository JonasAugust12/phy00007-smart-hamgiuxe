#include "parkinglot.h"
#include "firesystem.h"
#include "gate.h"

// TIME
struct tm timeinfo;
;
String currentDate;
String currentMonth;
int carCountByDay;
int carCountByMonth;

// WIFI
const char *ssid = "Wokwi-GUEST";
const char *password = "";

// FIREBASE
#define DATABASE_URL "https://phy00007-smart-hamgiuxe-22326-default-rtdb.firebaseio.com/"
#define DATABASE_SECRET "FOO9eBjUDTEu5gqSYJLLlJlxHSK2GWSBaKBxq3cO"
#define FIREBASE_PROJECT_ID "phy00007-smart-hamgiuxe-22326"

WiFiClientSecure ssl, ssl2;
DefaultNetwork network;
using AsyncClient = AsyncClientClass;
AsyncClient aClient(ssl, getNetwork(network)), aClient2(ssl2, getNetwork(network));

void asyncCB(AsyncResult &aResult);
void printResult(AsyncResult &aResult);

FirebaseApp app;
RealtimeDatabase Database;
Firestore::Documents Docs;
LegacyToken dbSecret(DATABASE_SECRET);
int cnt = 1;

// REAL TIME
#define UTC_OFFSET 7 * 3600
#define UTC_OFFSET_DST 0

bool isKeyExist(JsonObject obj, const char *key)
{
  for (auto kvp : obj)
  {
    if (strcmp(kvp.key().c_str(), key) == 0)
    {
      return true;
    }
  }
  return false;
}

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

// INIT SYSTEM
Gate *gate;
FireSystem *fireSystem;
ParkingLot *parkingLot;
LiquidCrystal_I2C LCD = LiquidCrystal_I2C(0x27, 16, 2);

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

  app.getApp<Firestore::Documents>(Docs);
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
      Firebase.printf("data: %s\n", RTDB.to<const char *>());
      Serial.println("");
      String path = RTDB.to<String>();
      JsonDocument doc;
      deserializeJson(doc, path);
      Serial.println(RTDB.dataPath().c_str());
      if (RTDB.dataPath() == "/BARRIER")
      {
        bool barrierState = doc["state"];
        Serial.print("Barrier: ");
        Serial.println(barrierState);
        gate->isGateActive = barrierState;
      }

      if (RTDB.dataPath() == "/SIREN")
      {
        bool sirenState = doc["state"];
        Serial.print("Siren: ");
        Serial.println(sirenState);
        fireSystem->isSirenActive = sirenState;
      }
      // FETCH OTHER STREAM DATA HERE
      String dailyPath = "/DAILY/" + currentDate;
      if (RTDB.dataPath() == dailyPath)
      {
        carCountByDay = RTDB.to<int>();
      }

      String monthlyPath = "/MONTHLY/" + currentMonth;
      if (RTDB.dataPath() == monthlyPath)
      {
        carCountByMonth = RTDB.to<int>();
      }

      if (RTDB.dataPath() == "/ARDUINO/message" && parkingLot->simulatedFlag)
      {
        parkingLot->receivedMessage = RTDB.to<String>();
        parkingLot->isSimulatedUpdate = true;
        Firebase.printf("path: %s ", parkingLot->receivedMessage.c_str());
      }
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

        // GATE
        if (isKeyExist(doc.as<JsonObject>(), "BARRIER"))
        {
          JsonObject barrier = doc["BARRIER"];
          gate->isGateActive = barrier["state"];
        }
        if (isKeyExist(doc.as<JsonObject>(), "CAR"))
        {
          JsonObject car = doc["CAR"];
          gate->carCountInDay = car["carInDay"];
          gate->carCountInMonth = car["carInMonth"];
          gate->carCountTotal = car["carTotal"];
        }

        // FIRE PROTECTION SYSTEM
        if (isKeyExist(doc.as<JsonObject>(), "SIREN"))
        {
          JsonObject siren = doc["SIREN"];
          fireSystem->isSirenActive = siren["state"];
        }

        if (isKeyExist(doc.as<JsonObject>(), "FIRE"))
        {
          JsonObject fire = doc["FIRE"];
          fireSystem->isFireDetected = fire["state"];
        }

        // PARKING LOT
        if (isKeyExist(doc.as<JsonObject>(), "LOT"))
        {
          JsonObject lot = doc["LOT"];
          parkingLot->lot[0] = lot["lot1"];
          parkingLot->lot[1] = lot["lot2"];
          parkingLot->lot[2] = lot["lot3"];
        }

        if (isKeyExist(doc.as<JsonObject>(), "TEMPERATURE"))
        {
          JsonObject temp = doc["TEMPERATURE"];
          fireSystem->temperature = temp["value"];
        }

        if (isKeyExist(doc.as<JsonObject>(), "DAILY"))
        {
          carCountByDay = doc["DAILY"][currentDate.c_str()];
        }

        if (isKeyExist(doc.as<JsonObject>(), "MONTHLY"))
        {
          carCountByMonth = doc["MONTHLY"][currentMonth.c_str()];
        }

        if (isKeyExist(doc.as<JsonObject>(), "ARDUINO") && parkingLot->simulatedFlag)
        {
          parkingLot->receivedMessage = doc["ARDUINO"]["message"].as<String>();
        }
      }
    }

    Firebase.printf("Free Heap: %d\n", ESP.getFreeHeap());
  }
}

void setupNTP()
{
  configTime(UTC_OFFSET, UTC_OFFSET_DST, "asia.pool.ntp.org", "time.google.com", "pool.ntp.org");
  Serial.print("Syncing time...");
  // struct tm timeinfo;
  for (int i = 0; i < 10; i++)
  { // Retry 10 times
    if (getLocalTime(&timeinfo))
    {
      Serial.println("");
      Serial.println("Time synchronized successfully");
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
      currentDate = String(timeinfo.tm_mday) + "-" + String(timeinfo.tm_mon + 1) + "-" + String(timeinfo.tm_year + 1900);
      currentMonth = String(timeinfo.tm_mon + 1) + "-" + String(timeinfo.tm_year + 1900);
      return;
    }
    delay(500); // Wait before retrying
    Serial.print(".");
  }
  if (time(nullptr) <= 0)
  {
    Serial.println(" Failed to obtain time.");
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");

  // WIFI & TIME
  setupWifi();
  setupNTP();

  // FIREBASE
  setupFirebase();

  // GATE SYSTEM
  LCD.init();
  LCD.backlight();
  LCD.setCursor(0, 0);
  LCD.print("      SMART     ");
  LCD.setCursor(0, 1);
  LCD.print(" PARKING SYSTEM ");
  delay(2000);
  LCD.clear();

  gate = new Gate();
  gate->setupGate();

  // FIRE PROTECTION SYSTEM
  fireSystem = new FireSystem();
  fireSystem->setupFireSystem();

  // PARKING LOT SYSTEM
  parkingLot = new ParkingLot();
}

unsigned long ms = 0;
void loop()
{
  if (getLocalTime(&timeinfo))
  {
    currentDate = String(timeinfo.tm_mday) + "-" + String(timeinfo.tm_mon + 1) + "-" + String(timeinfo.tm_year + 1900);
    currentMonth = String(timeinfo.tm_mon + 1) + "-" + String(timeinfo.tm_year + 1900);
  }

  gate->checkCarCountReset();

  app.loop();
  Database.loop();
  Docs.loop();

  // GATE
  gate->handleGate();
  // FIRE PROTECTION
  fireSystem->handleFireProtection();
  // PARKING LOT
  parkingLot->handleParkingLot();

  LCD.setCursor(0, 0);
  LCD.print("    WELCOME!    ");
  LCD.setCursor(0, 1);
  LCD.print("Slot Left: ");
  LCD.print(parkingLot->getSlotLeft());

  // SEND DATA TO WEBSITE
  if (app.ready())
  {
    if (gate->isCarEntered)
    {
      updateCarCount();
      updateHistory();
    }

    if (parkingLot->isCheckin)
      updateParkingLogCheckin();

    if (parkingLot->isCheckout)
      updateParkingLogCheckout();

    updateTemperature();

    if (fireSystem->isFireDetectionUpdate)
      updateFireDetection();

    if (fireSystem->isSirenUpdate)
      updateSiren();
  }

  delay(100);
}

void updateCarCount()
{
  JsonWriter writer;
  object_t CAR, carInDay, carInMonth, carTotal;

  writer.create(carInDay, "carInDay", gate->carCountInDay);
  writer.create(carInMonth, "carInMonth", gate->carCountInMonth);
  writer.create(carTotal, "carTotal", gate->carCountTotal);
  writer.join(CAR, 3, carInDay, carInMonth, carTotal);

  Database.set<object_t>(aClient2, "/CAR", CAR, asyncCB, "carTask");
}

void updateParkingLogCheckin()
{
  String documentPath = "Parking/";
  documentPath += +gate->carCountTotal;

  String message = parkingLot->receivedMessage;
  String lot_str = message.substring(0, message.indexOf(" "));
  String checkin_str = message.substring(message.indexOf("-") + 1, message.indexOf("IN") - 1);

  Values::StringValue lot(lot_str);
  Values::StringValue checkin(checkin_str);
  Values::StringValue checkout("None");

  Document<Values::Value> doc("lot", Values::Value(lot));
  doc.add("checkin", Values::Value(checkin));
  doc.add("checkout", Values::Value(checkout));

  Docs.createDocument(
      aClient2,
      Firestore::Parent(FIREBASE_PROJECT_ID),
      documentPath,
      DocumentMask(),
      doc,
      asyncCB,
      "createDocumentTask");

  JsonWriter writer;
  object_t LOT, lot1, lot2, lot3;
  parkingLot->lot[stoi(lot_str.c_str()) - 1] = gate->carCountTotal;
  writer.create(lot1, "lot1", parkingLot->lot[0]);
  writer.create(lot2, "lot2", parkingLot->lot[1]);
  writer.create(lot3, "lot3", parkingLot->lot[2]);
  writer.join(LOT, 3, lot1, lot2, lot3);
  Database.set<object_t>(aClient2, "/LOT", LOT, asyncCB, "updateLotMarking");

  parkingLot->receivedMessage = "";
  parkingLot->isCheckin = false;
}

void updateParkingLogCheckout()
{
  String documentPath = "Parking/";

  String message = parkingLot->receivedMessage;
  String lot_str = message.substring(0, message.indexOf(" "));
  String checkout_str = message.substring(message.indexOf("-") + 1, message.indexOf("OUT") - 1);

  documentPath += parkingLot->lot[stoi(lot_str.c_str()) - 1];

  Values::StringValue lot(lot_str);
  Values::StringValue checkout(checkout_str);

  Document<Values::Value> doc("checkout", Values::Value(checkout));

  PatchDocumentOptions options(
      DocumentMask("checkout"),
      DocumentMask(),
      Precondition());

  Docs.patch(
      aClient2,
      Firestore::Parent(FIREBASE_PROJECT_ID),
      documentPath,
      options,
      doc,
      asyncCB,
      "patchDocumentTask");

  JsonWriter writer;
  object_t LOT, lot1, lot2, lot3;
  parkingLot->lot[stoi(lot_str.c_str()) - 1] = 0;
  writer.create(lot1, "lot1", parkingLot->lot[0]);
  writer.create(lot2, "lot2", parkingLot->lot[1]);
  writer.create(lot3, "lot3", parkingLot->lot[2]);
  writer.join(LOT, 3, lot1, lot2, lot3);
  Database.set<object_t>(aClient2, "/LOT", LOT, asyncCB, "updateLotMarking");

  parkingLot->receivedMessage = "None";
  parkingLot->isCheckout = false;

  if (parkingLot->simulatedFlag)
  {
    object_t ARDUINO_SIMULATED, messageObj;
    writer.create(messageObj, "message", parkingLot->receivedMessage);
    writer.join(ARDUINO_SIMULATED, 1, messageObj);
    Database.set<object_t>(aClient2, "/ARDUINO", ARDUINO_SIMULATED, asyncCB, "arduinoTask");
  }
}

void updateTemperature()
{
  unsigned long currentMillis = millis();
  float currentTemperature = fireSystem->getTemperature();
  if (currentMillis - fireSystem->lastTemperatureUpdate >= fireSystem->tempInterval || abs(currentTemperature - fireSystem->temperature) > 1)
  {
    fireSystem->lastTemperatureUpdate = currentMillis;
    fireSystem->temperature = currentTemperature;
    JsonWriter writer;
    object_t TEMPERATURE, value;

    writer.create(value, "value", currentTemperature);
    writer.join(TEMPERATURE, 1, value);
    Database.set<object_t>(aClient2, "/TEMPERATURE", TEMPERATURE, asyncCB, "tempTask");
  }
}

void updateFireDetection()
{
  JsonWriter writer;
  object_t FIRE, state;

  writer.create(state, "state", fireSystem->isFireDetected);
  writer.join(FIRE, 1, state);

  Database.set<object_t>(aClient2, "/FIRE", FIRE, asyncCB, "fireTask");
  fireSystem->isFireDetectionUpdate = false;
}

void updateSiren()
{
  JsonWriter writer;
  object_t SIREN, state;

  writer.create(state, "state", fireSystem->isSirenActive);
  writer.join(SIREN, 1, state);

  Database.set<object_t>(aClient2, "/SIREN", SIREN, asyncCB, "sirenTask");
  fireSystem->isSirenUpdate = false;
}

void updateHistory()
{
  carCountByDay += 1;
  carCountByMonth += 1;

  JsonWriter writer;
  object_t DAILY, dateObj;
  writer.create(dateObj, currentDate, carCountByDay);
  writer.join(DAILY, 1, dateObj);

  object_t MONTHLY, monthObj;
  writer.create(monthObj, currentMonth, carCountByMonth);
  writer.join(MONTHLY, 1, monthObj);

  Database.update<object_t>(aClient2, "/DAILY", DAILY, asyncCB, "dailyHistoryTask");
  Database.update<object_t>(aClient2, "/MONTHLY", MONTHLY, asyncCB, "monthlyHistoryTask");
}