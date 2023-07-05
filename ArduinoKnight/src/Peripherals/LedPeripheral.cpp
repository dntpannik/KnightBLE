#include "Arduino.h"
#include <ArduinoBLE.h>
#include "LedPeripheral.h"
#include <UUID.h>

LedPeripheral::LedPeripheral(char* serviceId, char* characteristicId, int pin) :
    _characteristic(characteristicId, BLERead | BLEWrite | BLENotify),
    _service(serviceId),
    _pin(pin) {

    }

LedPeripheral::LedPeripheral(int pin) :
    _characteristic(UUID().toCharArray(), BLERead | BLEWrite | BLENotify),
    _service(UUID().toCharArray()),
    _pin(pin){
        
    }

void LedPeripheral::Initialize() {
    _service.addCharacteristic(this->_characteristic);
    //Set the pin to low
    digitalWrite(this->_pin, 0);
}

const char* LedPeripheral::ServiceId() {
    return _service.uuid();
}

void LedPeripheral::Update(int elapsedTime) {
    if (_characteristic.written()) {
        switch (_characteristic.value()) {
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
