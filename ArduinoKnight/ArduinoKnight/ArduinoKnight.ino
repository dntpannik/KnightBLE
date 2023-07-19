#include <DFRobotDFPlayerMini.h>
#include <ArduinoBLE.h>

#include "LedPeripheral.h"

char name[]= "BLEName";

LedPeripheral led("585ff59a-3e23-4405-ad70-1d6d87f7ae6d", 4);
Peripheral* peripherals[1] = {
  &led
};

void setup() {
  Serial.begin(9600);

  //---   Initialize BLE   ---//
  if(!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
    while (1);
  }

  BLE.setLocalName(name);

  //---   Initialize Peripherals   ---//
  // put your setup code here, to run once:
  for (Peripheral* peripheral : peripherals) {
    peripheral->Initialize();
  }

  //---   Advertise   ---//
  BLE.advertise();
  Serial.println("BLE Peripheral Engaged");
}

void loop() {
  //Listen for BLE peripherals to conect
  BLEDevice central = BLE.central();

  //If a center is connected then can search for writes
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    //While the central is still connected to peripheral:
    while (central.connected()) {
      for (Peripheral* peripheral : peripherals) {
        peripheral->Update();
      }
    }
  }
}
