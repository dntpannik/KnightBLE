#include "Arduino.h"
#include <ArduinoBLE.h>
#include <functional>
#include <vector>
#include "ToggleActionPeripheral.h"
#include "PropertyCharacteristics.h"

ToggleActionPeripheral::ToggleActionPeripheral(char* peripheralName, uint16_t order, char* actionNames, std::vector<togFn> actions, std::vector<togFn> disableActions, char* serviceId) :
    _name(peripheralName),
    _order(order),
    _nameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40),
    _actionNames(actionNames),
    _actions(actions),
    _disableActions(disableActions),
    _actionService(serviceId),
    _activeActions(actions.size(), false),
    _actionTriggerCharacteristic(ActionCharacteristic, BLERead | BLEWrite | BLENotify, 16) {
}

void ToggleActionPeripheral::Initialize() {
    BLE.setAdvertisedService(_actionService);

    //Disable
    for (int i = 0; i < _activeActions.size(); i++) {
      _disableActions[i](0);
    }

    //Order descriptor
    BLEDescriptor orderDescriptor(OrderDescriptor, reinterpret_cast<uint8_t*>(&_order), sizeof(uint16_t));
    _nameCharacteristic.addDescriptor(orderDescriptor);

    //Add descriptors to characteristics
     BLEDescriptor nameDescriptor(NameDescriptor, _actionNames);
    _actionTriggerCharacteristic.addDescriptor(nameDescriptor);

    //Initialize PropertyCharacteristics
    _nameCharacteristic.writeValue(_name);

    //Add characteristics to service
    _actionService.addCharacteristic(_nameCharacteristic);
    _actionService.addCharacteristic(_actionTriggerCharacteristic);
    
    BLE.addService(_actionService);
 
    Serial.print(_name);
    Serial.println(" initialized");
 }

 bool ToggleActionPeripheral::Update() {
    bool updated = false;  

    if (_actionTriggerCharacteristic.written()) {
        updated = true;  

        uint16_t actionValue;
        _actionTriggerCharacteristic.readValue(actionValue);

        //Flip the active status
        _activeActions[actionValue] = !_activeActions[actionValue];

        Serial.print("actionValue: ");
        Serial.println(actionValue);

        Serial.print("Value: ");
        Serial.println(_activeActions[actionValue]);
      
        //If not active then run the disable method
        if (_activeActions[actionValue] == false) {
          Serial.println("Disabling");
          //_disableActions[actionValue](millis());
        }
    }

    //Update all active actions
    for (int i = 0; i < _activeActions.size(); i++) {
      if (_activeActions[i] == true) {
        Serial.print("Here: ");
        Serial.println(i);
        
        _actions[i](millis());
        updated = true;
      }
    }


    return updated;
}

void ToggleActionPeripheral::Cleanup() {
}

void ToggleActionPeripheral::DebugOut() {
     
}
