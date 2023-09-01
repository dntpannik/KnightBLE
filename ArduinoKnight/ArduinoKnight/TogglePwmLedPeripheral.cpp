#include "Arduino.h"
#include <ArduinoBLE.h>
#include "TogglePwmLedPeripheral.h"
#include "PropertyCharacteristics.h"
#include "LedBoardManager.h"

TogglePwmLedPeripheral::TogglePwmLedPeripheral(char* peripheralName, uint16_t order, int pin, int brightness, char* serviceId) :
    _name(peripheralName),
    _order(order),
    _nameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40),
    _pins(1, pin),
    _enabled(false),
    _brightness(brightness),
    _ledService(serviceId),
    _toggleCharacteristic(ToggleCharacteristic, BLERead | BLEWrite | BLENotify){
    }

TogglePwmLedPeripheral::TogglePwmLedPeripheral(char* peripheralName, uint16_t order, std::vector<int> pins, int brightness, char* serviceId) :
    _name(peripheralName),
    _order(order),
    _nameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40),
    _pins(pins),
    _enabled(false),
    _brightness(brightness),
    _ledService(serviceId),
    _toggleCharacteristic(ToggleCharacteristic, BLERead | BLEWrite | BLENotify){
    }
    
void TogglePwmLedPeripheral::Initialize() {
     BLE.setAdvertisedService(_ledService);

    //Order descriptor
    BLEDescriptor orderDescriptor(OrderDescriptor, reinterpret_cast<uint8_t*>(&_order), sizeof(uint16_t));
    _nameCharacteristic.addDescriptor(orderDescriptor);

    //Initialize PropertyCharacteristics
    _nameCharacteristic.writeValue(_name);

    LedBoardManager* manager = LedBoardManager::getInstance();
    for (int i = 0; i < _pins.size(); i++) {
      manager->setValue(_pins[i], 0);
    } 
    _toggleCharacteristic.writeValue(0);

    //Add characteristics to service
    _ledService.addCharacteristic(_nameCharacteristic);
    _ledService.addCharacteristic(_toggleCharacteristic);

    BLE.addService(_ledService);
 
    Serial.print(_name);
    Serial.println(" initialized");
 }

bool TogglePwmLedPeripheral::Update() {
    bool updated = false;  

    if (_toggleCharacteristic.written()) {
        updated = true;  
    
        switch (_toggleCharacteristic.value()) {
            case 01:
            {
                Serial.print("LED On: ");
                Serial.println(_name);
                _enabled = true;

                LedBoardManager* manager = LedBoardManager::getInstance();
                for (int i = 0; i < _pins.size(); i++) {
                  Serial.println(_pins[i]);
                  manager->setValue(_pins[i], _brightness);
                } 
    
                break;
            }
            default:
            {
                Serial.print("LED Off: ");
                Serial.println(_name);
                _enabled = false;

                LedBoardManager* manager = LedBoardManager::getInstance();
                for (int i = 0; i < _pins.size(); i++) {
                  manager->setValue(_pins[i], 0);
                }
                break;
            }
        }
    }

    return updated;
}

void TogglePwmLedPeripheral::Cleanup() {
    //Set the pin to low

    LedBoardManager* manager = LedBoardManager::getInstance();
    for (int i = 0; i < _pins.size(); i++) {
      manager->setValue(_pins[i], 0);
    }
    _enabled = false;
}

void TogglePwmLedPeripheral::DebugOut() {
     Serial.println("---   Debug Output Start   ---");
     Serial.print("Name: ");
     Serial.println(_name);
     
     Serial.print("LEDService UUID: ");
     Serial.println(_ledService.uuid());
     
     Serial.print("Brightness: ");
     Serial.println(_brightness);

     Serial.print("NameCharacteristic UUID: ");
     Serial.println(_nameCharacteristic.uuid());
     Serial.print("NameCharacteristic Value: ");
     Serial.println(_nameCharacteristic.value());
     
     Serial.print("ToggleCharacteristic UUID: ");
     Serial.println(_toggleCharacteristic.uuid());
     Serial.print("ToggleCharacteristic Written: ");
     Serial.println(_toggleCharacteristic.written());
     Serial.print("ToggleCharacteristic Value: ");
     Serial.println(_toggleCharacteristic.value());
     Serial.println("------------------------------");
}
