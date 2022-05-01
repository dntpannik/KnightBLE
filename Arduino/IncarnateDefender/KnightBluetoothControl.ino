#include <ArduinoBLE.h>
//#include <SD.h>
#include <DFRobotDFPlayerMini.h>

//---   Service Flag Calculations   ---//
bool shouldAddEyeService = supportsEyeLeds || supportsLeftGunLeds;
bool shouldAddSmokeStackService = supportsSmokeStacks;
bool shouldAddSpeakerService = hasSpeakers;

//---   Misc Variables   ---//
//TMRpcm tmrpcm;
DFRobotDFPlayerMini myDFPlayer;
//---   Pin Definitions   ---//
int eyeLedPin = A0;
int leftGunLedPin = A1;
int smokeStackPin = D2;
int sdModulePin = D10;
int speakerPin = D9;

//---   Service/Characteristic Definitions   ---//
BLEService ledService("adb7bab2-df5c-4292-9f71-e2b6aa806c3b"); // BLE LED Service
    BLEByteCharacteristic eyeLedCharacteristic("d8eeaa08-db2f-48a0-99cd-aadd33194ffd", BLERead | BLEWrite | BLENotify);
    BLEByteCharacteristic leftGunLedCharacteristic("9aa13f92-1bd9-4e59-90f6-e41dfa9a81a2", BLERead | BLEWrite | BLENotify);
     
BLEService smokeStackService("9cd95a69-ee40-41be-a858-b0647c2fb955");
    BLEByteCharacteristic smokeStackCharacteristic("e9196461-d1ec-4b7d-a44a-fb76ad4b0795", BLERead | BLEWrite | BLENotify);

BLEService speakerService("ab936fb8-e5f7-4c43-b592-aab50be3c7da");
    BLEByteCharacteristic speakerCharacteristic("8ff278a1-a01e-4c46-87e7-751db82bfe24", BLERead | BLEWrite | BLENotify);



//---   Setup   ---//
void setup() {
  Serial.begin(9600);
  //while (!Serial); // Enable this if you want it to only work when the serial monitor is open

  //---   Set Pin Modes   ---//
  if (supportsEyeLeds) {
      Serial.println("Eye leds enabled");
      pinMode(eyeLedPin, OUTPUT);
  }

  if (supportsLeftGunLeds) {
      Serial.println("Left gun leds enabled");
      pinMode(leftGunLedPin, OUTPUT);
  }

  if (supportsSmokeStacks) {
      Serial.println("Smoke stacks enabled");
      pinMode(smokeStackPin, OUTPUT);
  }

  if (hasSpeakers) {
    Serial.println("Speaker enabled");
  }
  

  //Turn everything off
  TurnOffAllPins();

  //---   Begin Initialization   ---//
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");

    while (1);
  }

  // set advertised local name and service UUID:
  BLE.setLocalName(name);

  //---   Add Advertised Services   ---//
  if (shouldAddEyeService) {
    Serial.println("Advertising led service");
    BLE.setAdvertisedService(ledService);
  }
  if (shouldAddSmokeStackService) {
    Serial.println("Advertising smoke stack service");
    BLE.setAdvertisedService(smokeStackService);
  }
  if (shouldAddSpeakerService) {
    Serial.println("Advertising speaker service");
    BLE.setAdvertisedService(speakerService);
  }

  //---   Add Characteristics   ---//
  if (supportsEyeLeds) {
    Serial.println("Adding eye led characteristic");
    ledService.addCharacteristic(eyeLedCharacteristic);
  }
  if (supportsLeftGunLeds) {
    Serial.println("Adding left gun led characteristic");
    ledService.addCharacteristic(leftGunLedCharacteristic);
  }
  if (supportsSmokeStacks) {
    Serial.println("Adding smoke stack characteristic");
    smokeStackService.addCharacteristic(smokeStackCharacteristic);
  }
  if (hasSpeakers) {
    Serial.println("Adding speaker characteristic");
    speakerService.addCharacteristic(speakerCharacteristic);
  }

  //---   Add Services   ---//
  if (shouldAddEyeService) {
    Serial.println("Adding led service");
    BLE.addService(ledService);
  }
  if (shouldAddSmokeStackService) {
    Serial.println("Adding smoke stack service");
    BLE.addService(smokeStackService);  
  }
  if (shouldAddSpeakerService) {
    Serial.println("Adding speaker service");
    BLE.addService(speakerService);
  }

  //---   Set Initial Values   ---//
  if (supportsEyeLeds) {
    Serial.println("Setting eye led initial ble value");
    eyeLedCharacteristic.writeValue(0);
  }
  if (supportsLeftGunLeds) {
    Serial.println("Seting gun led initial ble value");
    leftGunLedCharacteristic.writeValue(0);
  }
  if (supportsSmokeStacks) {
    Serial.println("Setting smoke stack initial ble value");
    smokeStackCharacteristic.writeValue(0);
  }
  if (hasSpeakers) {
    Serial.println("Setting speaker initial ble value");
    speakerCharacteristic.writeValue(0);
  }

  //---   Misc Initialization   ---//
  if (hasSpeakers) {
    Serial.println("Initializing SD module");
    //tmrpcm.speakerPin = speakerPin;
    //if (!SD.begin(sdModulePin)) {
     // Serial.println("SD fail");
    //  return;
   // }

    //tmrpcm.setVolume(6);
    //tmrpcm.play("horn.wav");
  }
  
  //---   Advertise   ---//
  BLE.advertise();
  Serial.println("BLE Peripheral Engaged");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLEDevice central = BLE.central();;
  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      if (supportsEyeLeds && eyeLedCharacteristic.written()) {
        switch (eyeLedCharacteristic.value()) { 
          case 01:
            Serial.println("Eye LED on");
            digitalWrite(eyeLedPin, HIGH);
            break;
          default:
            Serial.println(F("Eye LED off"));
            digitalWrite(eyeLedPin, LOW);
            break;
        }
      }
      if (supportsLeftGunLeds && leftGunLedCharacteristic.written()) {
        switch (leftGunLedCharacteristic.value()) { 
          case 01:
            Serial.println("Left Gun LED on");
            digitalWrite(leftGunLedPin, HIGH);
            break;
          default:
            Serial.println(F("Left Gun LED off"));
            digitalWrite(leftGunLedPin, LOW);
            break;
        }
      }
      if (supportsSmokeStacks && smokeStackCharacteristic.written()) {
        switch (smokeStackCharacteristic.value()) { 
          case 01:
            Serial.println("Smoke stacks on");
            digitalWrite(smokeStackPin, HIGH);
            break;
          default:
            Serial.println(F("Smoke stacks off"));
            digitalWrite(smokeStackPin, LOW);
            break;
        }
      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
    TurnOffAllPins();
  }
}

void TurnOffAllPins() {
  if (supportsEyeLeds) {
      digitalWrite(eyeLedPin, LOW);
  }

  if (supportsLeftGunLeds) {
      digitalWrite(leftGunLedPin, LOW);
  }

  if (supportsSmokeStacks) {
      digitalWrite(smokeStackPin, LOW);
  }
}
