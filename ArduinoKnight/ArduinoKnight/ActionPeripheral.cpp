#include "Arduino.h"
#include <ArduinoBLE.h>
#include <functional>
#include <vector>
#include "ActionPeripheral.h"
#include "PropertyCharacteristics.h"

ActionPeripheral::ActionPeripheral(char* peripheralName, char* actionNames, std::vector<fn> actions, char* serviceId) :
    _name(peripheralName),
    _nameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40),
    _actionNames(actionNames),
    _actions(actions),
    _actionService(serviceId),
    _activeActions(actions.size(), false),
    _actionTriggerCharacteristic(ActionCharacteristic, BLERead | BLEWrite | BLENotify, 16) {
}
    
void ActionPeripheral::Initialize() {
    BLE.setAdvertisedService(_actionService);

    //Add descriptors to characteristics
     BLEDescriptor nameDescriptor(NameDescriptor, _actionNames);
    _actionTriggerCharacteristic.addDescriptor(nameDescriptor);

    //Add characteristics to service
    _actionService.addCharacteristic(_nameCharacteristic);
    _actionService.addCharacteristic(_actionTriggerCharacteristic);

    //Initialize PropertyCharacteristics
    _nameCharacteristic.writeValue(_name);
    
    BLE.addService(_actionService);
 
    Serial.print(_name);
    Serial.println(" initialized");
 }

bool ActionPeripheral::Update() {
    bool updated = false;  

    if (_actionTriggerCharacteristic.written()) {
        updated = true;  

        uint16_t actionValue;
        _actionTriggerCharacteristic.readValue(actionValue);

        Serial.print("Triggered ActionId: ");
        Serial.println(actionValue);

        //Mark action as active
        _activeActions[actionValue] = true;
          
        //fn actionFunction = _actions[actionValue];
        //ctionFunction(millis());
    }

    //Update all active actions and update active status
    for (int i = 0; i < _activeActions.size(); i++) {
      if (_activeActions[i] == true) {
        _activeActions[i] = _actions[i](millis());
      }
    }


    return updated;
}

void ActionPeripheral::Cleanup() {
}

void ActionPeripheral::DebugOut() {
     
}
