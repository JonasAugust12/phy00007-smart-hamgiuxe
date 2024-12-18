#include "header.h"

class ParkingLot {
    private:
        
    public:
        int slotLeft;
        String receivedMessage;
        bool isCheckin;
        bool isCheckout;
        int* lot;

        bool simulatedFlag = true;
        bool isSimulatedUpdate = false;

        ParkingLot();
        String getSlotLeft();
        void setSlotLeft(int slotLeft);
        void handleParkingLot();
        ~ParkingLot();
};