#include "Arduino.h"
#include <ArduinoBLE.h>
#include "LedPeripheral.h"
#include "PropertyCharacteristics.h"
#include "UUID.h"

LedPeripheral::LedPeripheral(const char* serviceId, int pin) :
    _pin(pin),
    _service(serviceId),
    _ledControlCharacteristic(BoolCharacteristic, BLERead | BLEWrite | BLENotify){
    }
    
void LedPeripheral::Initialize() {
    Serial.print(_service.uuid());
    Serial.print(_ledControlCharacteristic.uuid());
    
    BLE.setAdvertisedService(_service);
    _service.addCharacteristic(this->_ledControlCharacteristic);

    BLE.addService(_service);
    
    //Set the pin to low
    digitalWrite(this->_pin, 0);    
    _ledControlCharacteristic.writeValue(0);
}

const char* LedPeripheral::ServiceId() {
    return _service.uuid();
}

void LedPeripheral::Update() {
    if (_ledControlCharacteristic.written()) {
        switch (_ledControlCharacteristic.value()) {
            case 01:
                Serial.print("LED On - Service ");
                Serial.println(_service.uuid());
                digitalWrite(_pin, 1);
                break;
            default:
                Serial.print("LED Off - Service ");
                Serial.println(_service.uuid());
                digitalWrite(_pin, 0);
                break;
        }
    }
}

//GUID: Boolean
