#ifndef ToggleLedPeripheral_h
#define ToggleLedPeripheral_h

#include <ArduinoBLE.h>
#include "Peripheral.h"

class ToggleLedPeripheral : public Peripheral {
  public:
        ToggleLedPeripheral(char* peripheralName, int pin, char* serviceId);
        void Initialize();
        bool Update();
        void Cleanup();
        void DebugOut();
   
    private:
        char* _name;
        BLEStringCharacteristic _nameCharacteristic;
        int _pin;
        BLEService _ledService;
        BLEByteCharacteristic _toggleCharacteristic;
};

#endif
