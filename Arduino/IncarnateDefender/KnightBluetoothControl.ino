#include <ArduinoBLE.h>
#include <DFRobotDFPlayerMini.h>

//---   Service Flag Calculations   ---//
bool shouldAddEyeService = supportsEyeLeds || supportsLeftGunLeds;
bool shouldAddSmokeStackService = supportsSmokeStacks;
bool shouldAddSpeakerService = supportsAudio;

//---   Pin Definitions   ---//
int eyeLedPin = A0;
int leftGunLedPin = A1;
int smokeStackPin = D2;
int dfPlayerRxPin = 0;
int dfPlayerTxPin = 1;

//---   Misc Variables   ---//
DFRobotDFPlayerMini myDFPlayer;

#define AUDIO_DATA_SIZE 3
typedef struct __attribute__( ( packed ) )
{
  uint8_t Track;
  uint8_t Volume;
  uint8_t Delay;
} AudioData;

typedef union
{
  AudioData values;
  uint8_t bytes[ AUDIO_DATA_SIZE ];
} AudioData_ut;


//---   Service/Characteristic Definitions   ---//
BLEService ledService("adb7bab2-df5c-4292-9f71-e2b6aa806c3b"); // BLE LED Service
    BLEByteCharacteristic eyeLedCharacteristic("d8eeaa08-db2f-48a0-99cd-aadd33194ffd", BLERead | BLEWrite | BLENotify);
    BLEByteCharacteristic leftGunLedCharacteristic("9aa13f92-1bd9-4e59-90f6-e41dfa9a81a2", BLERead | BLEWrite | BLENotify);
     
BLEService smokeStackService("9cd95a69-ee40-41be-a858-b0647c2fb955");
    BLEByteCharacteristic smokeStackCharacteristic("e9196461-d1ec-4b7d-a44a-fb76ad4b0795", BLERead | BLEWrite | BLENotify);

BLEService speakerService("ab936fb8-e5f7-4c43-b592-aab50be3c7da");
    BLECharacteristic audioInfoCharacteristic("607047f7-9dd2-477d-94fa-9a6333dba1d4", BLERead | BLEWrite | BLENotify, 40);
    BLECharacteristic trackControlCharacteristic("8ff278a1-a01e-4c46-87e7-751db82bfe24", BLERead | BLEWrite | BLENotify, 12);



//---   Setup   ---//
void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);
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

  if (supportsAudio) {
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
  if (supportsAudio) {
    Serial.println("Adding speaker characteristic");
    speakerService.addCharacteristic(audioInfoCharacteristic);
    speakerService.addCharacteristic(trackControlCharacteristic);
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
  if (supportsAudio) {
    Serial.println("Setting available track information");
    audioInfoCharacteristic.writeValue(tracks, sizeof(tracks));
  }

  //---   Misc Initialization   ---//
  if (supportsAudio) {
    Serial.println("Initializing DF Player");
    //myDFPlayer.begin(Serial1);
    if (!myDFPlayer.begin(Serial1)) {
      Serial.println(F("Unable to begin:"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
      while(true);
    }
    Serial.println(F("DFPlayer Mini online."));
    
    myDFPlayer.volume(15);
    myDFPlayer.play(1);
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
      if (supportsAudio && trackControlCharacteristic.written()) {
        Serial.println("Recevied Audio Data: ");

        AudioData_ut audioData;
        trackControlCharacteristic.readValue(audioData.bytes, sizeof audioData.bytes);

        Serial.println((String)"Track: "+audioData.values.Track);
        Serial.println((String)"Volume: "+audioData.values.Volume);
        Serial.println((String)"Delay: "+audioData.values.Delay * 5);

        delay(audioData.values.Delay * 5);
        myDFPlayer.volume(audioData.values.Volume);
        myDFPlayer.play(audioData.values.Track);
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
