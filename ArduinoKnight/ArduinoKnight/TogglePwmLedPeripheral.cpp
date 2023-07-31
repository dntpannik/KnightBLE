#include "Arduino.h"
#include <ArduinoBLE.h>
#include "TogglePwmLedPeripheral.h"
#include "PropertyCharacteristics.h"
#include "LedBoardManager.h"

TogglePwmLedPeripheral::TogglePwmLedPeripheral(char* peripheralName, int pin, int brightness, char* serviceId) :
    _name(peripheralName),
    _nameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40),
    _pin(pin),
    _enabled(false),
    _brightness(brightness),
    _ledService(serviceId),
    _toggleCharacteristic(ToggleCharacteristic, BLERead | BLEWrite | BLENotify){
    }
    
void TogglePwmLedPeripheral::Initialize() {
     BLE.setAdvertisedService(_ledService);

    //Add characteristics to service
    _ledService.addCharacteristic(_nameCharacteristic);
    _ledService.addCharacteristic(_toggleCharacteristic);

    //Initialize PropertyCharacteristics
    _nameCharacteristic.writeValue(_name);

    LedBoardManager* manager = LedBoardManager::getInstance();
    manager->setValue(_pin, 0);        
    _toggleCharacteristic.writeValue(0);

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
                manager->setValue(_pin, _brightness);
                break;
            }
            default:
            {
                Serial.print("LED Off: ");
                Serial.println(_name);
                _enabled = false;

                LedBoardManager* manager = LedBoardManager::getInstance();
                manager->setValue(_pin, 0);
                break;
            }
        }
    }

    return updated;
}

void TogglePwmLedPeripheral::Cleanup() {
    //Set the pin to low

    LedBoardManager* manager = LedBoardManager::getInstance();
    manager->setValue(_pin, 0);
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
