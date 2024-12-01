#include "parkinglot.h"

ParkingLot::ParkingLot() {
    slotLeft = 3;
    receivedMessage = "";
    isCheckin = false;
    isCheckout = false;
    lot = new int(3);
}

String ParkingLot::getSlotLeft(){
    return String(slotLeft);
}

void ParkingLot::handleParkingLot(){
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
                isCheckin = true;
                if (slotLeft > 0)
                {
                    slotLeft--;
                }
            }
            else if (message.find("OUT") != -1)
            {
                isCheckout = true;
                if (slotLeft < 3)
                {
                    slotLeft++;
                }
            }
            // Clear the message buffer for the next input
            // receivedMessage = "";
        }
        else
        {
            // Append the character to the message string
            receivedMessage += incomingChar;
        }
    }
}

ParkingLot::~ParkingLot() {
}