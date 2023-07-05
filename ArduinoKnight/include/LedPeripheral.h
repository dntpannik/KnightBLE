#ifndef LedPeripheral_h
#define LedPeripheral_h

#include <ArduinoBLE.h>
#include "Peripheral.h"

class LedPeripheral : public Peripheral {
    public:
        LedPeripheral(char* serviceId, char* characteristicId, int pin);
        LedPeripheral(int pin);
        const char* ServiceId();
        void Initialize();
        void Update(int elapsedTime);
   
    private:
        int _pin;
        BLEService _service;
        BLEByteCharacteristic _characteristic;
};

#endif