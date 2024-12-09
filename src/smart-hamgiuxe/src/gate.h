#include "header.h"

#define SERVO_TIMER_GROUP 1 // Sử dụng timer group 1

class Gate {
    private:
        // SETUP
        int in_trig = 33;
        int in_echo = 34;
        int servo_pin = 16;
        int out_trig = 23;
        int out_echo = 39;
        Servo servo;

        // GATE SYSTEM// 
        const unsigned long passingTimeout = 5000;

        unsigned long startPassingTime = 0;
        bool isGateInOpened = false;
        bool isPassing = false;
        bool isInSensorExist = false;

        unsigned long startPassingOutTime = 0;
        bool isGateOutOpened = false;
        bool isPassingOut = false;
        bool isOutSensorExist = false;

    public:
        bool isGateActive = true;
        bool isCarEntered = false;
        int carCountInDay = 0;
        int carCountInMonth = 0;
        int carCountTotal = 0;

        Gate();

        // DEVICE
        void setupServoTimer();
        void setupGate();
        bool handleGateFlow(int sensorTrig, int sensorEcho, bool &isGateOpened, bool &isSensorExist,
                            bool &isPassing, unsigned long &startPassingTime, const char *gateType);
        void handleGate();
        long getDistance(int trig, int echo);
        void openGate();
        void closeGate();

        // FIREBASE
        void updateCarCount(
            void (*operation)(AsyncClientClass, const String, object_t, AsyncResultCallback, const String),
            AsyncClientClass &aClient, 
            const String path, 
            AsyncResultCallback callback, 
            const String dbSecret
        );
        void checkCarCountReset();

        ~Gate();
    
};