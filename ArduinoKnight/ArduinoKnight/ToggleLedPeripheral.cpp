#include "Arduino.h"
#include <ArduinoBLE.h>
#include "ToggleLedPeripheral.h"
#include "PropertyCharacteristics.h"

ToggleLedPeripheral::ToggleLedPeripheral(char* peripheralName, int pin, char* serviceId) :
    _name(peripheralName),
    _nameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40),
    _pin(pin),
    _ledService(serviceId),
    _toggleCharacteristic(ToggleCharacteristic, BLERead | BLEWrite | BLENotify) {
}
    
void ToggleLedPeripheral::Initialize() {
    BLE.setAdvertisedService(_ledService);

    //Add descriptors to characteristics
     BLEDescriptor toggleDescriptor(NameDescriptor, "LED Enabled");
    _toggleCharacteristic.addDescriptor(toggleDescriptor);

    //Add characteristics to service
    _ledService.addCharacteristic(_nameCharacteristic);
    _ledService.addCharacteristic(_toggleCharacteristic);

    //Initialize PropertyCharacteristics
    _nameCharacteristic.writeValue(_name);

   //Set the pin to low
    digitalWrite(this->_pin, 0);    
    this->_toggleCharacteristic.writeValue(0);

    BLE.addService(_ledService);
 
    Serial.print(_name);
    Serial.println(" initialized");
 }

bool ToggleLedPeripheral::Update() {
bool updated = false;  

    if (_toggleCharacteristic.written()) {
        updated = true;  
    
        switch (_toggleCharacteristic.value()) {
            case 01:
            {
                Serial.print("LED On: ");
                Serial.println(_name);

                digitalWrite(_pin, 1);
                break;
            }
            default:
            {
                Serial.print("LED Off: ");
                Serial.println(_name);
                
                digitalWrite(_pin, 0);
                break;
            }
        }
    }

    return updated;
}

void ToggleLedPeripheral::Cleanup() {
    //Set the pin to low
    digitalWrite(this->_pin, 0);    
    this->_toggleCharacteristic.writeValue(0);
}

void ToggleLedPeripheral::DebugOut() {
     Serial.println("---   Debug Output Start   ---");
     Serial.print("Name: ");
     Serial.println(_name);
     
     Serial.print("LEDService UUID: ");
     Serial.println(_ledService.uuid());

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
