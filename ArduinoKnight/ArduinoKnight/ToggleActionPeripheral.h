#ifndef ToggleActionPeripheral_h
#define ToggleActionPeripheral_h

#include <ArduinoBLE.h>
#include <functional>
#include <vector>
#include "Peripheral.h"

typedef void (*togFn)( unsigned long );

class ToggleActionPeripheral : public Peripheral {
  public:
        ToggleActionPeripheral(char* peripheralName, uint16_t order, char* actionNames, std::vector<togFn> actions, std::vector<togFn> disableActions, char* serviceId);
        void Initialize();
        bool Update();
        void Cleanup();
        void DebugOut();
   
    private:
        char* _name;
        uint16_t _order;
        BLEStringCharacteristic _nameCharacteristic;
        char* _actionNames;
        std::vector<togFn> _actions;
        std::vector<togFn> _disableActions;
        std::vector<bool> _activeActions;
        BLEService _actionService;
        BLECharacteristic _actionTriggerCharacteristic;
};

#endif
