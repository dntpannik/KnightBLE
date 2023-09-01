
#ifndef RgbLedPeripheral_h
#define RgbLedPeripheral_h

#include <ArduinoBLE.h>
#include "Peripheral.h"

class RgbLedPeripheral : public Peripheral {
    public:
        RgbLedPeripheral(char* peripheralName, uint16_t order, int channel, char* _initialColor, char* serviceId);
        void Initialize();
        bool Update();
        void Cleanup();
        void DebugOut();

    private:
        char* _name;
        uint16_t _order;
        BLEStringCharacteristic _nameCharacteristic;
        bool _enabled;
        int _channel;
        char* _initialColor;
        BLEService _ledService;
        BLEByteCharacteristic _toggleCharacteristic;
        BLEStringCharacteristic _rgbCharacteristic;
};

#endif
