#include <DFRobotDFPlayerMini.h>
#include <ArduinoBLE.h>

#include "ToggleLedPeripheral.h"
#include "TogglePwmLedPeripheral.h"
#include "RgbLedPeripheral.h"
#include "SliderPeripheral.h"
#include "ActionPeripheral.h"
#include "PropertyCharacteristics.h"
#include "LedBoardManager.h"
#include "AudioManager.h"

BLEService modelService("585ff59a-3e23-4405-ad70-1d6d87f7ae6d");


void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
  
  // while the serial stream is not open, do nothing:
  //while (!Serial) ;
   
  Serial.println("Starting Up");
  //---   Initialize BLE   ---//
  if(!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
    while (1);
  }

  //---   Intialize LED Board   ---//
  LedBoardManager* manager = LedBoardManager::getInstance();
  manager->Initialize();

  //---   Initialize Audio   ---//
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->Initialize();
  
  //---   Initialize Peripherals   ---//
  // put your setup code here, to run once:
  for (int i = 0; i < numPeripherals; i++) {
    peripherals[i]->Initialize();
  }


  //---   Initialize Model Service   ---//  
  BLE.setLocalName(name);
  BLE.setAdvertisedService(modelService);

  //---   Add Service   ---
   BLE.addService(modelService);
    
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
      bool updated = false;
      
      //for (Peripheral* peripheral : peripherals) {
      for (int i = 0; i < numPeripherals; i++) {
        bool peripheralUpdated = peripherals[i]->Update();
        
        if (peripheralUpdated) {
          updated = true;
        }
      }

      if (updated && VerboseDebuggingEnabled) {
        for (int i = 0; i < numPeripherals; i++) {
          peripherals[i]->DebugOut();
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    for (int i = 0; i < numPeripherals; i++) {
      peripherals[i]->Cleanup();
    }
  }
}
