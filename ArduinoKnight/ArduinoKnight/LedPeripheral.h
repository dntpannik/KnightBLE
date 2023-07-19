#ifndef LedPeripheral_h
#define LedPeripheral_h

#include <ArduinoBLE.h>
#include <UUID.h>
#include "Peripheral.h"

class LedPeripheral : public Peripheral {
    public:
        LedPeripheral(const char* serviceId, int pin);
        LedPeripheral(int pin);
        const char* ServiceId();
        void Initialize();
        void Update();
   
    private:
        int _pin;
        BLEService _service;
        BLEByteCharacteristic _ledControlCharacteristic;
};

#endif
