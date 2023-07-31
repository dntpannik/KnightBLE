#include "Arduino.h"
#include <ArduinoBLE.h>
#include "RgbLedPeripheral.h"
#include "PropertyCharacteristics.h"
#include "LedBoardManager.h"

RgbLedPeripheral::RgbLedPeripheral(char* peripheralName, int channel, char* _initialColor, char* serviceId) :
    _name(peripheralName),
    _nameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40),
    _enabled(false),
    _channel(channel),
    _initialColor(_initialColor),
    _ledService(serviceId),
    _toggleCharacteristic(ToggleCharacteristic, BLERead | BLEWrite | BLENotify),
    _rgbCharacteristic(RGBCharacteristic, BLERead | BLEWrite | BLENotify, 6) {
    }
    
void RgbLedPeripheral::Initialize() {

    BLE.setAdvertisedService(_ledService);

    //Add characteristics to service
    _ledService.addCharacteristic(_nameCharacteristic);
    _ledService.addCharacteristic(_toggleCharacteristic);
    _ledService.addCharacteristic(_rgbCharacteristic);

    //Initialize PropertyCharacteristics
    _nameCharacteristic.writeValue(_name);

    LedBoardManager* manager = LedBoardManager::getInstance();
    manager->setRGB(_channel, 0, 0, 0);
    _toggleCharacteristic.writeValue(0);

    Serial.print("Initializing RGB to: ");
    Serial.println(_initialColor);
    _rgbCharacteristic.writeValue(_initialColor);

    BLE.addService(_ledService);
 
    Serial.print(_name);
    Serial.println(" initialized");
 }

bool RgbLedPeripheral::Update() {
    bool updated = false;
  
    if (_toggleCharacteristic.written()) {
        updated = true;
        switch (_toggleCharacteristic.value()) {
            case 01:
            {
                char* hex  = (char *)_rgbCharacteristic.value().c_str();
                Serial.print("LED On: ");
                Serial.print(_name);
                Serial.print(" Color: ");
                Serial.print(hex);
                Serial.print(" ServiceId: ");
                Serial.println(_ledService.uuid());

                LedBoardManager* manager = LedBoardManager::getInstance();
                manager->setRGB(_channel, hex);
                _enabled = true;
                break;
            }
            default:
            {
                Serial.print("LED Off: ");
                Serial.print(_name);
                Serial.print(" ServiceId: ");
                Serial.println(_ledService.uuid());

                LedBoardManager* manager = LedBoardManager::getInstance();
                manager->setRGB(_channel, 0, 0, 0);
                _enabled = false;
                break;
            }
        }
    }

    if (_rgbCharacteristic.written()) {
        updated = true;
        char* hex  = (char *)_rgbCharacteristic.value().c_str();
        Serial.print(_name);
        Serial.print(": Setting Color to: ");
        Serial.print(hex);
        Serial.print(" ServiceId: ");
        Serial.println(_ledService.uuid());

        if (_enabled) {
          LedBoardManager* manager = LedBoardManager::getInstance();
          manager->setRGB(_channel, hex);
        }
    }

    return updated;
}

void RgbLedPeripheral::Cleanup() {
    _enabled = false;

    LedBoardManager* manager = LedBoardManager::getInstance();
    manager->setRGB(_channel, 0, 0, 0);
}

void RgbLedPeripheral::DebugOut() {
     Serial.println("---   Debug Output Start   ---");
     Serial.print("Name: ");
     Serial.println(_name);
     
     Serial.print("LEDService UUID: ");
     Serial.println(_ledService.uuid());
     
     Serial.print("Initial Color: ");
     Serial.println(_initialColor);

     Serial.print("NameCharacteristic UUID: ");
     Serial.println(_nameCharacteristic.uuid());
     Serial.print("NameCharacteristic Value: ");
     Serial.println(_nameCharacteristic.value());
     
     Serial.print("RGBCharacteristic UUID: ");
     Serial.println(_rgbCharacteristic.uuid());
     Serial.print("RGBCharacteristic Written: ");
     Serial.println(_rgbCharacteristic.written());
     Serial.print("RGBCharacteristic Value: ");
     Serial.println(_rgbCharacteristic.value());
     
     Serial.print("ToggleCharacteristic UUID: ");
     Serial.println(_toggleCharacteristic.uuid());
     Serial.print("ToggleCharacteristic Written: ");
     Serial.println(_toggleCharacteristic.written());
     Serial.print("ToggleCharacteristic Value: ");
     Serial.println(_toggleCharacteristic.value());
     Serial.println("------------------------------");
     
}
