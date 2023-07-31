#ifndef SliderPeripheral_h
#define SliderPeripheral_h

#include <ArduinoBLE.h>
#include "Peripheral.h"

typedef void (*sfn)( unsigned long, uint16_t, uint16_t, uint16_t );

class SliderPeripheral : public Peripheral {
    public:
        SliderPeripheral(char* peripheralName, char* sliderName, uint16_t initialValue, uint16_t minValue, uint16_t maxValue, uint16_t stepValue, sfn action, char* serviceId);
        void Initialize();
        bool Update();
        void Cleanup();
        void DebugOut();

    private:
        char* _name;
        BLEStringCharacteristic _nameCharacteristic;
        char* _sliderName;
        uint16_t _value;
        uint16_t _minValue;
        uint16_t _maxValue;
        uint16_t _stepValue;
        sfn _action;
        BLEService _sliderService;
        BLECharacteristic _sliderCharacteristic;
};

#endif
