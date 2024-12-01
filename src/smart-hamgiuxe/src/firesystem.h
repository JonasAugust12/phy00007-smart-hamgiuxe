#include "header.h"

class FireSystem {
    private:
        // SETUP
        int temp_sensor = 32;
        int button = 35;
        int led = 18;
        int buzzer = 5;
        int fire_relay = 17;
        int smoke_sensor = 36;

        // FIRE PROTECTION SYSTEM
        #define SMOKE_THRESHOLD 2047
        #define TEMPERATURE_THRESHOLD 70
        const float BETA = 3950;

        bool isSmokeDetected = false;
        bool isEmergencyPressed = false;

        unsigned long pressTime = 0;
        unsigned long ledActiveTime = 0;
        unsigned long previousMillis = 0;
        const long interval = 50;
        int freq = 1000;
        bool holdButton = false;

    public:
        bool isSirenActive = false;
        bool isFireDetected = false;

        float temperature = 0;
        unsigned long lastTemperatureUpdate = 0;
        const long tempInterval = 60000 * 5;

        bool isFireDetectionUpdate = false;

        FireSystem();
        void setupFireSystem();
        void handleFireProtection();
        float getTemperature();
        void activateAlarm();
        void deactivateAlarm();
        ~FireSystem();
};