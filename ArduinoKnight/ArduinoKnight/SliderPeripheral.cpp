#include "Arduino.h"
#include <ArduinoBLE.h>
#include "SliderPeripheral.h"
#include "PropertyCharacteristics.h"

SliderPeripheral::SliderPeripheral(char* peripheralName, char* sliderName, uint16_t initialValue, uint16_t minValue, uint16_t maxValue, uint16_t stepValue, sfn action, char* serviceId) :
    _name(peripheralName),
    _nameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40),
    _sliderName(sliderName),
    _value(initialValue),
    _minValue(minValue),
    _maxValue(maxValue),
    _stepValue(stepValue),
    _action(action),
    _sliderService(serviceId),
    _sliderCharacteristic(SliderCharacteristic, BLERead | BLEWrite | BLENotify, 16) {
    }
    
void SliderPeripheral::Initialize() {

    BLE.setAdvertisedService(_sliderService);

    //Add descriptors to characteristics
     BLEDescriptor nameDescriptor(NameDescriptor, _sliderName);
    _sliderCharacteristic.addDescriptor(nameDescriptor);

    uint8_t minValueArray[2]={ _minValue & 0xff, _minValue >> 8 };
    BLEDescriptor minValueDescriptor(MinValueDescriptor, minValueArray, 2);
    _sliderCharacteristic.addDescriptor(minValueDescriptor);

    uint8_t maxValueArray[2]={ _maxValue & 0xff, _maxValue >> 8 };
    BLEDescriptor maxValueDescriptor(MaxValueDescriptor, maxValueArray, 2);
    _sliderCharacteristic.addDescriptor(maxValueDescriptor);

    uint8_t stepValueArray[2]={ _stepValue & 0xff, _stepValue >> 8 };
    BLEDescriptor stepValueDescriptor(StepValueDescriptor, stepValueArray, 2);
    _sliderCharacteristic.addDescriptor(stepValueDescriptor);

    //Add characteristics to service
    _sliderService.addCharacteristic(_nameCharacteristic);
    _sliderService.addCharacteristic(_sliderCharacteristic);

    //Initialize PropertyCharacteristics
    _nameCharacteristic.writeValue(_name);
    _sliderCharacteristic.writeValue(_value);

    BLE.addService(_sliderService);
 
    Serial.print(_name);
    Serial.println(" initialized");
 }

bool SliderPeripheral::Update() {
    bool updated = false;
  
    if (_sliderCharacteristic.written()) {
        updated = true;
        
        _sliderCharacteristic.readValue(_value);

        Serial.print("Slider Updated: ");
        Serial.print(_name);
        Serial.print(" Value: ");
        Serial.print(_value);
        Serial.print(" ServiceId: ");
        Serial.println(_sliderService.uuid());

        _action(millis(), _value, _minValue, _maxValue);
    }

    return updated;
}

void SliderPeripheral::Cleanup() {
}

void SliderPeripheral::DebugOut() {
     Serial.println("---   Debug Output Start   ---");
     Serial.print("Name: ");
     Serial.println(_name);
     
     Serial.print("SliderService UUID: ");
     Serial.println(_sliderService.uuid());

     Serial.print("SliderName: ");
     Serial.println(_sliderName);
     
     Serial.print("Value: ");
     Serial.println(_value);

     Serial.print("MinValue: ");
     Serial.println(_minValue);

     Serial.print("MaxValue: ");
     Serial.println(_maxValue);

     Serial.print("StepValue: ");
     Serial.println(_stepValue);

     Serial.print("NameCharacteristic UUID: ");
     Serial.println(_nameCharacteristic.uuid());
     Serial.print("NameCharacteristic Value: ");
     Serial.println(_nameCharacteristic.value());
     
     Serial.println("------------------------------"); 
}
