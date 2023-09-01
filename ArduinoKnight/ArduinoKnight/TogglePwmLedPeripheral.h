#ifndef TogglePwmLedPeripheral_h
#define TogglePwmLedPeripheral_h

#include <ArduinoBLE.h>
#include <vector>
#include "Peripheral.h"

class TogglePwmLedPeripheral : public Peripheral {
    public:
        TogglePwmLedPeripheral(char* peripheralName, uint16_t order, int pin, int brightness, char* serviceId);
        TogglePwmLedPeripheral(char* peripheralName, uint16_t order, std::vector<int> pins, int brightness, char* serviceId);
        void Initialize();
        bool Update();
        void Cleanup();
        void DebugOut();
   
    private:
        char* _name;
        BLEStringCharacteristic _nameCharacteristic;
        uint16_t _order;
        std::vector<int> _pins;
        bool _enabled;
        int _brightness;
        BLEService _ledService;
        BLEByteCharacteristic _toggleCharacteristic;
};

#endif
