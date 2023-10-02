#ifndef ArduinoKnight_h
#define ArduinoKnight_h

#include <DFRobotDFPlayerMini.h>
#include "ToggleLedPeripheral.h"
#include "TogglePwmLedPeripheral.h"
#include "RgbLedPeripheral.h"
#include "SliderPeripheral.h"
#include "ActionPeripheral.h"
#include "PropertyCharacteristics.h"
#include "LedBoardManager.h"
#include "AudioManager.h"

#define RSSI_REASONABLE_SIGNAL  -70  
#define MaxRSSIRepeat 5

int rssiRepeatCounter = 0;
int rssiPrevious = 0;

char* modelName= "NoName";

bool VerboseDebuggingEnabled = false;
bool connectionActive = false;

int numPeripherals = 0;
Peripheral** peripherals = {};

BLEService modelService("585ff59a-3e23-4405-ad70-1d6d87f7ae6d");


void blePeripheralConnectHandler(BLEDevice central) {
    Serial.println("Connected event, central: ");
    Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
    Serial.println("Disconnected event, central: ");
    Serial.println(central.address());

    for (int i = 0; i < numPeripherals; i++) {
      peripherals[i]->Cleanup();
    }
}

void enableWDT() {
  //Configure WDT on nRF52840.
  NRF_WDT->CONFIG         = 0x01;             // Configure WDT to run when CPU is asleep
  NRF_WDT->CRV            = 2 * 32768;        // Set timeout for 2 seconds
  NRF_WDT->RREN           = 0x01;             // Enable the RR[0] reload register
  NRF_WDT->TASKS_START    = 1;                // Start WDT    
}

void resetWDT() {
  // Reload the WDTs RR[0] reload register
  NRF_WDT->RR[0] = WDT_RR_RR_Reload; 
}

void RunSetup(char* name, bool verboseEnabled, int peripheralCount, Peripheral** peripheralArray) {
  modelName = name;
  VerboseDebuggingEnabled = verboseEnabled;
  numPeripherals = peripheralCount;
  peripherals = peripheralArray;

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

  BLE.setConnectionInterval(0x000C, 0x000C);

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

  //---   Subscribe to Events   ---//
  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  //---   Initialize Model Service   ---//  
  BLE.setLocalName(modelName);
  BLE.setAdvertisedService(modelService);

  //---   Add Service   ---
   BLE.addService(modelService);
    
  //---   Advertise   ---//
  BLE.advertise();
  Serial.println("BLE Peripheral Engaged");

  enableWDT();
}

void RunLoop() {
  static long prvTime = 0;
  BLE.poll();

  //If there's no active connection then reset the WDT timer
  if (connectionActive == false) {
    resetWDT();
  }

  if (millis() - prvTime >= 500) {
    prvTime = millis();
    if (BLE.connected()) {
      connectionActive = true;
      
      //If the RSSI is sufficient weak then reset WDT timer
      int rssiValue = BLE.rssi();
      Serial.print("RSSI: ");
      Serial.println(rssiValue);

      if (rssiValue == rssiPrevious) {
        rssiRepeatCounter += 1;
        Serial.print("RSSI Repeat: ");
        Serial.println(rssiRepeatCounter);
      } else {
        rssiPrevious = rssiValue;
        rssiRepeatCounter = 0;
      }
      
      if (rssiRepeatCounter < MaxRSSIRepeat) {
        resetWDT();
      } else {
        Serial.println("RSSI Repeat Triggered - Don't Reset Timer");
      }
    
      for (int i = 0; i < numPeripherals; i++) {
        peripherals[i]->Update();
      }
    }
    else {
    }
  } 
}

#endif
