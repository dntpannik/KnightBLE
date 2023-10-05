#ifndef ActionPeripheral_h
#define ActionPeripheral_h

#include <ArduinoBLE.h>
#include <functional>
#include <vector>
#include "Peripheral.h"

typedef bool (*fn)( unsigned long );

class ActionPeripheral : public Peripheral {
    public:
        ActionPeripheral(char* peripheralName, uint16_t order, char* actionNames, std::vector<fn> actions, char* serviceId);
        void Initialize();
        bool Update();
        void Cleanup();
        void DebugOut();
 
    private: 
        char* _name;
        uint16_t _order;
        BLEStringCharacteristic _nameCharacteristic;
        char* _actionNames;
        std::vector<fn> _actions;
        std::vector<bool> _activeActions;
        BLEService _actionService;
        BLECharacteristic _actionTriggerCharacteristic;
};

#endif
