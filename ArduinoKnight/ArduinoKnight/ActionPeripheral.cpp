#include "Arduino.h"
#include <ArduinoBLE.h>
#include <functional>
#include <vector>
#include "ActionPeripheral.h"
#include "PropertyCharacteristics.h"

using namespace std::placeholders;

ActionPeripheral::ActionPeripheral(char* peripheralName, uint16_t order, char* actionNames, std::vector<fn> actions, char* serviceId) :
    _name(peripheralName),
    _order(order),
    _nameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40),
    _actionNames(actionNames),
    _actions(actions),
    _actionService(serviceId),
    _activeActions(actions.size(), false),
    _actionTriggerCharacteristic(ActionCharacteristic, BLERead | BLEWrite | BLENotify, 16) {
}

void ActionPeripheral::ActionTriggerCharacteristicWritten(BLEDevice central, BLECharacteristic characteristic) {
  Serial.print("actionTriggerCharacteristic event, written: ");

  uint16_t actionValue;
  //_actionTriggerCharacteristic.readValue(actionValue);

  //Mark action as active
  //_activeActions[actionValue] = true;
}
    
void ActionPeripheral::Initialize() {
    BLE.setAdvertisedService(_actionService);

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

    // assign event handlers for characteristic
    auto eventHandler = std::bind(&ActionPeripheral::ActionTriggerCharacteristicWritten, this); 
    _actionTriggerCharacteristic.setEventHandler(BLEWritten, eventHandler);
    
    BLE.addService(_actionService);
 
    Serial.print(_name);
    Serial.println(" initialized");
 }

bool ActionPeripheral::Update() {
    bool updated = false;  

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
