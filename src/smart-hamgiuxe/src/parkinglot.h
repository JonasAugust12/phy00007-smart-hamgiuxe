#include "header.h"

class ParkingLot {
    private:
        int slotLeft;
        
    public:
        String receivedMessage;
        bool isCheckin;
        bool isCheckout;
        int* lot;

        bool simulatedFlag = true;
        bool isSimulatedUpdate = false;

        ParkingLot();
        String getSlotLeft();
        void handleParkingLot();
        ~ParkingLot();
};