#ifndef TogglePwmLedPeripheral_h
#define TogglePwmLedPeripheral_h

#include <ArduinoBLE.h>
#include "Peripheral.h"

class TogglePwmLedPeripheral : public Peripheral {
    public:
        TogglePwmLedPeripheral(char* peripheralName, int pin, int brightness, char* serviceId);
        void Initialize();
        bool Update();
        void Cleanup();
        void DebugOut();
   
    private:
        char* _name;
        BLEStringCharacteristic _nameCharacteristic;
        int _pin;
        bool _enabled;
        int _brightness;
        BLEService _ledService;
        BLEByteCharacteristic _toggleCharacteristic;
};

#endif
