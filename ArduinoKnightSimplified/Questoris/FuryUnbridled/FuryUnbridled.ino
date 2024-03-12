#include <ArduinoBLE.h>
#include <Adafruit_TLC59711.h>
#include <DFRobotDFPlayerMini.h>

char* modelName = "Fury Unbridled";


BLEService modelService("585ff59a-3e23-4405-ad70-1d6d87f7ae6d");

//------------------------------//
//---   Characteristic Ids   ---//
//------------------------------//
static const char *PulseCharacteristic = "ceebd2e5-af02-4623-88a1-82603ed74be2";

static const char *NameCharacteristic = "5b7a52b5-af5b-4451-921b-4cf039193c58";

static const char *ToggleCharacteristic = "9929eb1f-c8c6-42d3-99a2-61d1d6e157bb";
static const char *RGBCharacteristic = "44c5f429-9c6d-460a-a9ab-0fd36ca26cd7";
static const char *SliderCharacteristic = "9a84a830-23c4-4685-b526-3e42fe867b6b";
static const char *ActionCharacteristic = "6955ecc8-f7be-4efa-b122-ed0fef08fc28";
static const char *ToggleActionCharacteristic = "89032d0b-aee6-48ef-8e9d-46a56957719b";
static const char *RandomActionCharacteristic = "f916183a-154c-4380-bca4-327d4df9fddc";

static const char *NameDescriptor = "b936f62f-6127-4463-9abd-8f965602c737";
static const char *OrderDescriptor = "af93f94e-248c-4076-930e-a66a55877bb2";
static const char *MinValueDescriptor = "0e20fe55-86f5-400f-a95b-8d194416731b";
static const char *MaxValueDescriptor = "89db4977-95c5-42f8-8990-ede5ef227c6d";
static const char *StepValueDescriptor = "ef7548a1-9942-4f00-a0cf-cf744b3d15da";


//----------------------//
//---   LED Board .  ---//
//----------------------//

//---   Definitions .  ---//
#define NUM_TLC59711  1
#define dataPin       2
#define clockPin      3
Adafruit_TLC59711 LedManager = Adafruit_TLC59711(NUM_TLC59711, clockPin, dataPin);

//---   Methods .  ---//
void setLedValue (int ledPin,int pwmVal) {
  int pwm = pwmVal * 257;
  int chan = ledPin;

  LedManager.setPWM(chan, pwm);
  LedManager.write();
}

void setRGB(int rgbGroup, int redVal, int greenVal, int blueVal) {
  int red = redVal * 256;
  int green = greenVal * 256;
  int blue = blueVal * 256;
  const int rChannel = 3 * rgbGroup;

  LedManager.setLED(rgbGroup, red, green, blue);
  LedManager.write();
}

//------------------------//
//---   Audio Board    ---//
//------------------------//

//---   Definitions    ---//
DFRobotDFPlayerMini AudioManager = DFRobotDFPlayerMini();

//---   Methods    ---//
void setVolume(uint8_t vol) {
  if (vol > 30) {
    vol = 30;
  }

  AudioManager.volume(vol);
}

void playTrack(uint8_t trackNum) {
  AudioManager.playMp3Folder(trackNum);
}

//-----------------//
//---   Eyes    ---//
//-----------------//

char* eyePeripheralName = "Eyes";
uint16_t eyePeripheralOrder = 1;
bool eyePeripheralEnabled = false;
//Channels, 6, 7

BLEStringCharacteristic eyePeripheralNameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40);
BLEByteCharacteristic eyePeripheralToggleCharacteristic(ToggleCharacteristic, BLERead | BLEWrite | BLENotify);
BLEService eyePeripheralService("6641ed28-d551-430b-9222-407194f31bee");

static void InitializeEyePeripheral() {
  BLE.setAdvertisedService(eyePeripheralService);

  //Order descriptor
  BLEDescriptor orderDescriptor(OrderDescriptor, reinterpret_cast<uint8_t*>(&eyePeripheralOrder), sizeof(uint16_t));
  eyePeripheralNameCharacteristic.addDescriptor(orderDescriptor);

  //Initialize PropertyCharacteristics
  eyePeripheralNameCharacteristic.writeValue(eyePeripheralName);

  setLedValue(6, 0);
  setLedValue(7, 0);
  eyePeripheralToggleCharacteristic.writeValue(0);

  //Add characteristics to service
  eyePeripheralService.addCharacteristic(eyePeripheralNameCharacteristic);
  eyePeripheralService.addCharacteristic(eyePeripheralToggleCharacteristic);

  BLE.addService(eyePeripheralService);
 
  Serial.print(eyePeripheralName);
  Serial.println(" initialized");
}

//---   Update   ---//
static void UpdateEyePeripheral(unsigned long updateMillis) {
  bool updated = false;
  
  if (eyePeripheralToggleCharacteristic.written()) {
    updated = true;  

    switch (eyePeripheralToggleCharacteristic.value()) {
      case 01:
      {
        Serial.print("LED On: ");
        Serial.println(eyePeripheralName);
        eyePeripheralEnabled = true;

        setLedValue(6, 254);
        setLedValue(7, 254);
    
        break;
      }
      default:
      {
        Serial.print("LED Off: ");
        Serial.println(eyePeripheralName);
        eyePeripheralEnabled = false;

        setLedValue(6, 0);
        setLedValue(7, 0);
                
        break;
      }
    }
  }
}

//---   Cleanup .  ---//
static void CleanupEyePeripheral(unsigned long updateMillis) {
  setLedValue(6, 0);
  setLedValue(7, 0);
  eyePeripheralEnabled = false;
}

//-------------------//
//---   Toggles   ---//
//-------------------//
char* togglePeripheralName = "Actions";
uint16_t togglePeripheralOrder = 5;
char* togglePeripheralActionNames = "Exhaust|Body|Carapice";

bool exhaustToggleActive = false;
bool bodyToggleActive = false;
bool carapaceToggleActive = false;

BLEStringCharacteristic togglePeripheralNameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40);
BLECharacteristic togglePeripheralTriggerCharacteristic(ToggleActionCharacteristic, BLERead | BLEWrite | BLENotify, 16);
BLEService togglePeripheralService("a6b0ba67-ba48-481a-a288-ba40b48dc4b3");

//---   Initialize   ---//
static void InitializeTogglePeripheral() {
  BLE.setAdvertisedService(togglePeripheralService);

    //Disable
    CleanupTogglePeripheral(0);

    //Order descriptor
    BLEDescriptor orderDescriptor(OrderDescriptor, reinterpret_cast<uint8_t*>(&togglePeripheralOrder), sizeof(uint16_t));
    togglePeripheralNameCharacteristic.addDescriptor(orderDescriptor);

    //Add descriptors to characteristics
     BLEDescriptor nameDescriptor(NameDescriptor, togglePeripheralActionNames);
    togglePeripheralTriggerCharacteristic.addDescriptor(nameDescriptor);

    //Initialize PropertyCharacteristics
    togglePeripheralNameCharacteristic.writeValue(togglePeripheralName);

    //Add characteristics to service
    togglePeripheralService.addCharacteristic(togglePeripheralNameCharacteristic);
    togglePeripheralService.addCharacteristic(togglePeripheralTriggerCharacteristic);
    
    BLE.addService(togglePeripheralService);
 
    Serial.print(togglePeripheralName);
    Serial.println(" initialized");
  
}

//---   Update   ---//
static void UpdateTogglePeripheral(unsigned long updateMillis) {
  if (togglePeripheralTriggerCharacteristic.written()) {

    uint16_t actionValue;
    togglePeripheralTriggerCharacteristic.readValue(actionValue);

    //Flip the active status
    switch(actionValue) {
      case 0: 
        exhaustToggleActive = !exhaustToggleActive;
        break;
      case 1:
        bodyToggleActive = !bodyToggleActive;
        break;
      case 2:
        carapaceToggleActive = !carapaceToggleActive;
        break;
    }

    //If not active then run the disable method
    if (exhaustToggleActive == false) {
      DisableExhaustToggle(updateMillis);
    }
    if (bodyToggleActive == false) {
      DisableBodyToggle(updateMillis);
    }
    if (carapaceToggleActive == false) {
      DisableCarapaceToggle(udpateMillis);
    }
  }

  //Update all active actions
  if (exhaustToggleActive == true) {
    UpdateExhaustToggle(updateMillis);
  }
  if (bodyToggleActive == true) {
    UpdateBodyToggle(updateMillis);
  }
  if (carapaceToggleActive == true) {
    UpdateCarapaceToggle(updateMillis);
  }
}

//---   Cleanup   ---//
static void CleanupTogglePeripheral(unsigned long updateMillis) {
  DisableExhaustToggle(updateMillis);
}

//---   Exhaust   ---//
static long exhaustPreviousUpdateTime = 0;
const int exhaustChannel1 = 1;
const int exhaustChannel2 = 2;
const int exhaustChannel3 = 3;
const int exhaustChannel4 = 4;
int exhaustUpdateDelay = 50;
int exhaustMinUpdateDelay = 50;
int exhaustMaxUpdateDelay = 150;
int exhaustBaseValue = 150;
int exhaustRandomValue = 80;

static void UpdateExhaustToggle(unsigned long updateMillis) {
  //If the LEDs have updated recently then return
  if (updateMillis - exhaustPreviousUpdateTime < exhaustUpdateDelay) {
    return;
  }

  exhaustPreviousUpdateTime = updateMillis;
  
  //Genereate new LED values
  int ledValue = exhaustBaseValue + random(1, exhaustRandomValue);
  
  setLedValue(exhaustChannel1, ledValue);
  setLedValue(exhaustChannel2, ledValue);
  setLedValue(exhaustChannel3, ledValue);
  setLedValue(exhaustChannel4, ledValue);

  //Generate new update delay
  exhaustUpdateDelay = random(exhaustMinUpdateDelay, exhaustMaxUpdateDelay);   
}

static void DisableExhaustToggle(unsigned long updateMillis) {
  setLedValue(exhaustChannel1, 0);
  setLedValue(exhaustChannel2, 0);
  setLedValue(exhaustChannel3, 0);
  setLedValue(exhaustChannel4, 0);
}

//---   Body   ---//
static void UpdateBodyToggle(unsigned long updateMillis) {

}

static void DisableBodyToggle(unsigned long updateMillis) {

}

//---   Carapace   ---//
static void UpdateCarapaceToggle(unsigned long updateMillis) {

}

static void DisableCarapaceToggle(unsigned long updateMillis) {

}

//--------------------//
//---   Actions    ---//
//--------------------//
char* ActionPeripheralName = "Actions";
uint16_t ActionPeripheralOrder = 6;
char* ActionPeripheralActionNames = "Horn|Auto Cannon|Lascannon|Missles|Twin Magma Lascannon|Titanic Feet|Ricochet|Explosion";

bool hornActionActive = false;

bool autoCannonActionActive = false;
bool lascannonActionActive = false;
bool missleActionActive = false;
bool twinMagmaLascannonActionActive.= false;

bool titanicFeetActionActive = false;

bool ricochetActionActive = false;
bool explosionActionActive = false;

BLEStringCharacteristic ActionPeripheralNameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40);
BLEService ActionPeripheralService("d47a3f8d-e29d-4964-a7f8-b870e99ca792");
BLECharacteristic ActionPeripheralTriggerCharacteristic(ActionCharacteristic, BLERead | BLEWrite | BLENotify, 16);

//---   Initialize .  ---//
static void InitializeActionPeripheral() {
  BLE.setAdvertisedService(ActionPeripheralService);

  //Initialize Pins
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);

  //Order descriptor
  BLEDescriptor orderDescriptor(OrderDescriptor, reinterpret_cast<uint8_t*>(&ActionPeripheralOrder), sizeof(uint16_t));
  ActionPeripheralNameCharacteristic.addDescriptor(orderDescriptor);

  //Add descriptors to characteristics
  BLEDescriptor nameDescriptor(NameDescriptor, ActionPeripheralActionNames);
  ActionPeripheralTriggerCharacteristic.addDescriptor(nameDescriptor);

  //Initialize PropertyCharacteristics
  ActionPeripheralNameCharacteristic.writeValue(ActionPeripheralName);

  //Add characteristics to service
  ActionPeripheralService.addCharacteristic(ActionPeripheralNameCharacteristic);
  ActionPeripheralService.addCharacteristic(ActionPeripheralTriggerCharacteristic);

  BLE.addService(ActionPeripheralService);
 
  Serial.print(ActionPeripheralName);
  Serial.println(" initialized");
}

//---   Update   ---//
static void UpdateActionPeripheral(unsigned long updateMillis) {
  bool updated = false;
  
  if (ActionPeripheralTriggerCharacteristic.written()) {
    updated = true;  

    uint16_t actionValue;
    ActionPeripheralTriggerCharacteristic.readValue(actionValue);

    //Mark action as active
    switch(actionValue) {
      case 0: 
        hornActionActive = true;
        break;
      case 1:
        autoCannonActionActive = true;
        break;
      case 2:
        lascannonActionActive = true;
        break;
      case 3:
        missleActionActive = true;
        break;
      case 4:
        twinMagmaLascannonActionActive = true;
        break;
      case 5:
        titanicFeetActionActive = true;
        break;
      case 6:
        ricochetActionActive = true;
        break;
      case 7:
        explosionActionActive = true;
        break;  
    }
  }

  //Update all active actions
  if (hornActionActive == true) {
    hornActionActive = PerformHornAction(updateMillis);
  }

  if (avengerGatlingCannonActionActive == true) {
    avengerGatlingCannonActionActive = PerformAvengerGatlingCannonAction(updateMillis);
  }

  if (rapidFireBattleCannonActionActive == true) {
    rapidFireBattleCannonActionActive = PerformRapidFireBattleCannonAction(updateMillis);
  }

  if (lasImpulsorActionActive == true) {
    lasImpulsorActionActive = PerformLasImpulsorAction(updateMillis);
  }

  if (thermalCannonActionActive == true) {
    thermalCannonActionActive = PerformThermalCannonAction(updateMillis);
  }

  if (reaperChainswordActionActive == true) {
    reaperChainswordActionActive = PerformReaperChainswordAction(updateMillis);
  }

  if (meltaGunActionActive == true) {
    meltaGunActionActive = PerformMeltaGunAction(updateMillis);
  }

  if (heavyStubberActionActive == true) {
    heavyStubberActionActive = PerformHeavyStubberAction(updateMillis);
  }

  if (battleCannonStubberActionActive == true) {
    battleCannonStubberActionActive = PerformBattleCannonHeavyStubberAction(updateMillis);
  }

  if (ironstormMisslePodActionActive == true) {
    ironstormMisslePodActionActive = PerformIronstormMisslePodAction(updateMillis);
  }

  if (stormspearRocketPodActionActive == true) {
    stormspearRocketPodActionActive = PerformStormspearRocketPodAction(updateMillis);
  }

  if (twinIcarusAutoCannonActionActive == true) {
    twinIcarusAutoCannonActionActive = PerformTwinIcarusAutoCannonAction(updateMillis);
  }

  if (titanicFeetActionActive == true) {
    titanicFeetActionActive = PerformTitanicFeetAction(updateMillis);
  }

  if (ricochetActionActive == true) {
    ricochetActionActive = PerformRicochetAction(updateMillis);
  }

  if (explosionActionActive == true) {
    explosionActionActive = PerformExplosionAction(updateMillis);
  }
}

//---   Cleanup .  ---//
static void CleanupActionPeripheral(unsigned long updateMillis) {
  DisableAvengerGatlingCannonAction();
  DisableRapidFireBattleCannonAction();
  DisableLasImpulsorAction();
  DisableThermalCannonAction();
  DisableMeltaGunAction();
  DisableHeavyStubberAction();
  DisableBattleCannonStubberAction();
  DisableIronstormMisslePodAction();
  DisableStormspearRocketPodAction();
  DisableTwinIcarusAutoCannonAction();
}

//-----------------------------------//
//---   Audio Tracks              ---//
//--- 01: Horn                    ---//
//--- 02: Gattling Cannon         ---//
//--- 03: Battle Cannon           ---//
//--- 04: Las Impulsor            ---//
//--- 05: Thermal Cannon          ---//
//--- 06: Chainsword              ---//
//--- 07: Melta Gun               ---//
//--- 08: Heavy Stubber           ---//
//--- 09: Ironstorm Missle Pod    ---//
//--- 10: Stormspear Rocket Pod   ---//
//--- 11: Twin Icarus             ---//
//--- 12: Titanic Feet            ---//
//--- 13: Ricochet                ---//
//--- 14: Explosion               ---//
//-----------------------------------//


//---   Horn   ---//
const uint8_t hornAudioTrack = 1;
static bool PerformHornAction(unsigned long updateMillis) {
  playTrack(hornAudioTrack);

  return false;
}

//---   Avenger Gatling Cannon   ---//
static int gatlingLed = 10;
static int gatlingMotorPin = 4;
bool gatlingFiring = false;
long gatlingStartTime = 0;
bool gatlingLedState = false;
long gatlingLastFireMillis = 0; 
const long gatlingFinalFireMillis = 2760; 
const long gatlingInitialDelay = 100;
const long gatlingFireInterval = 500;
const uint8_t gatlingAudioTrack = 2;
static bool PerformAvengerGatlingCannonAction(unsigned long updateMillies) {

  if (!gatlingFiring) {
     gatlingFiring = true;
     gatlingStartTime = updateMillies;
     gatlingLastFireMillis = gatlingStartTime - gatlingFireInterval + gatlingInitialDelay;
     digitalWrite(gatlingMotorPin, 1);  
     setLedValue(gatlingLed, 254);
     playTrack(gatlingAudioTrack);
  }
  
  if (updateMillies - gatlingStartTime >= gatlingFinalFireMillis) {
    setLedValue(gatlingLed, 0);
    digitalWrite(gatlingMotorPin, 0);  
    gatlingFiring = false;
    gatlingLedState = false;
    return false;
  }

  return true; //Stil active
}

static void DisableAvengerGatlingCannonAction() {
  setLedValue(gatlingLed, 0);
  digitalWrite(gatlingMotorPin, 0);
  gatlingFiring = false;
  gatlingLedState = false;
}

//---   Rapid Fire Battle Cannon   ---//
static int battleCannonLed = 5;
bool battleCannonFiring = false;
long battleCannonStartTime = 0;
bool battleCannonLedState = false;
long battleCannonLastFireMillis = 0; 
const long battleCannonFinalFireMillis = 3200; 
const long battleCannonInitialDelay = 500;
const long battleCannonFireInterval = 1300;
const long battleCannonLedOnTime = 100;
const uint8_t battleCannonAudioTrack = 3;
static bool PerformRapidFireBattleCannonAction(unsigned long updateMillies) {
if (!battleCannonFiring) {
     battleCannonFiring = true;
     battleCannonStartTime = updateMillies;
     battleCannonLastFireMillis = battleCannonStartTime - battleCannonFireInterval + battleCannonInitialDelay;

     playTrack(battleCannonAudioTrack);
  }

  if (updateMillies - battleCannonLastFireMillis >= battleCannonFireInterval) {
    if (battleCannonLedState == false) { //Only update if the LED needs to be toggled
      battleCannonLastFireMillis = updateMillies;
      digitalWrite(battleCannonLed, 1); 

      battleCannonLedState = true;
    }
    
  } else {
    if (battleCannonLedState == true) {
      //Turn off LED after enough time has elapsed
      if (updateMillies - battleCannonLastFireMillis >= battleCannonLedOnTime) {
        digitalWrite(battleCannonLed, 0); 
        battleCannonLedState = false;
      }
    }
  }

  if (updateMillies - battleCannonStartTime >= battleCannonFinalFireMillis) {
    digitalWrite(battleCannonLed, 0); 
    battleCannonFiring = false;
    battleCannonLedState = false;
    return false;
  }

  return true; //Stil active
}

static void DisableRapidFireBattleCannonAction() {
  digitalWrite(battleCannonLed, 0); 
  battleCannonFiring = false;
  battleCannonLedState = false;
}

//---   Las Impulsor   ---//
static int lasChamberPin = 5;
static int lasTipPin = 9;
bool lasFiring = false;
long lasStartTime = 0;
long lasPreviousUpdateTime = 0;

const long lasInitialDelay = 600;
const long lasFireStartMillis = 2800;
const long lasCooldownStartMillis = 5500;
const long lasFinalFireMillis = 6800; 
const long lasUpdateTimerMillis = 100;

const uint8_t lasAudioTrack = 4;
static bool PerformLasImpulsorAction(unsigned long updateMillies) {


  if (!lasFiring) {
     lasFiring = true;
     lasStartTime = updateMillies;
     lasPreviousUpdateTime = lasStartTime - 2 * lasUpdateTimerMillis; //Set the previous update time to before the start time so it updates first tick

     playTrack(lasAudioTrack);
  }

  //If the LEDs have updated recently then return
  if (updateMillies - lasPreviousUpdateTime < lasUpdateTimerMillis) {
    return true;
  }
  
  lasPreviousUpdateTime = updateMillies;
  unsigned long elapsedTime = updateMillies - lasStartTime;

  //Completed firing
  if (elapsedTime >= lasFinalFireMillis) {
    digitalWrite(lasChamberPin, 0);
    setLedValue(lasTipPin, 0);
    lasFiring = false;
    return false; //Finished
  }

  //Initial delay
  if (elapsedTime < lasInitialDelay) {
    return true; //Still active
  }

  //Warming up
  if (elapsedTime < lasFireStartMillis) { 
     digitalWrite(lasChamberPin, 1); 
     return true;
  }

  //Firing
  if (elapsedTime < lasCooldownStartMillis) {
    setLedValue(lasTipPin, 254);
    return true;
  }

  //Cooldown
  if (elapsedTime < lasFinalFireMillis) {
    setLedValue(lasTipPin, elapsedTime / (lasFinalFireMillis - lasCooldownStartMillis) * 254);
    return true;
  }
 
  return true; //Still active
}

static void DisableLasImpulsorAction() {
  digitalWrite(lasChamberPin, 0);
  setLedValue(lasTipPin, 0);
  lasFiring = false;
}

//---   Thermal Cannon   ---//
const uint8_t thermalCannonAudioTrack = 5;

static bool PerformThermalCannonAction(unsigned long updateMillies) {
  playTrack(thermalCannonAudioTrack);
  return false;
}

static void DisableThermalCannonAction() {

}

//---   Reaper Chainsword   ---//
const uint8_t chainswordAudioTrack = 6;

static bool PerformReaperChainswordAction(unsigned long updateMillies) {
  playTrack(chainswordAudioTrack);
  return false;
}

//---   Melta Gun   ---//
const uint8_t meltaAudioTrack = 7;
static bool PerformMeltaGunAction(unsigned long updateMillies) {
  playTrack(meltaAudioTrack);
  return false;
}

static void DisableMeltaGunAction() {

}

//---   Heavy Stubber   ---//
static int stubberPin = 0;
bool stubberFiring = false;
long stubberStartTime = 0;
bool stubberLedState = false;
long stubberLastFireMillis = 0; 
const long stubberFinalFireMillis = 2200; 
const long stubberInitialDelay = 200;
const long stubberFireInterval = 60;
const long stubberLedOnTime = 30;
const uint8_t stubberAudioTrack = 8;


static bool PerformHeavyStubberAction(unsigned long updateMillies) {
  if (!stubberFiring) {
     stubberFiring = true;
     stubberStartTime = updateMillies;
     stubberLastFireMillis = stubberStartTime - stubberFireInterval + stubberInitialDelay;

      playTrack(stubberAudioTrack);
  }

  if (updateMillies - stubberLastFireMillis >= stubberFireInterval) {
    if (stubberLedState == false) { //Only update if the LED needs to be toggled
      stubberLastFireMillis = updateMillies;
      setLedValue(stubberPin, 254);

      stubberLedState = true;
    }
    
  } else {
    if (stubberLedState == true) {
      //Turn off LED after enough time has elapsed
      if (updateMillies - stubberLastFireMillis >= stubberLedOnTime) {
        setLedValue(stubberPin, 0);
        stubberLedState = false;
      }
    }
  }

  if (updateMillies - stubberStartTime >= stubberFinalFireMillis) {
    setLedValue(stubberPin, 0);
    stubberFiring = false;
    stubberLedState = false;
    return false;
  }

  return true; //Stil active
}

static void DisableHeavyStubberAction() {
  setLedValue(stubberPin, 0);
  stubberFiring = false;
  stubberLedState = false;
}

//---   Battle Cannon Stubber   ---//
static int battleStubberPin = 9;
bool battleStubberFiring = false;
long battleStubberStartTime = 0;
bool battleStubberLedState = false;
long battleStubberLastFireMillis = 0; 
const long battleStubberFinalFireMillis = 2200; 
const long battleStubberInitialDelay = 200;
const long battleStubberFireInterval = 60;
const long battleStubberLedOnTime = 30;
const uint8_t battleStubberAudioTrack = 8;

static bool PerformBattleCannonHeavyStubberAction(unsigned long updateMillies) {
  if (!battleStubberFiring) {
     battleStubberFiring = true;
     battleStubberStartTime = updateMillies;
     battleStubberLastFireMillis = battleStubberStartTime - battleStubberFireInterval + battleStubberInitialDelay;

     playTrack(battleStubberAudioTrack);
  }

  if (updateMillies - battleStubberLastFireMillis >= battleStubberFireInterval) {
    if (battleStubberLedState == false) { //Only update if the LED needs to be toggled
      battleStubberLastFireMillis = updateMillies;
      setLedValue(battleStubberPin, 254);

      battleStubberLedState = true;
    }
    
  } else {
    if (battleStubberLedState == true) {
      //Turn off LED after enough time has elapsed
      if (updateMillies - battleStubberLastFireMillis >= battleStubberLedOnTime) {
        setLedValue(battleStubberPin, 0);
        battleStubberLedState = false;
      }
    }
  }

  if (updateMillies - battleStubberStartTime >= battleStubberFinalFireMillis) {
    setLedValue(battleStubberPin, 0);
    battleStubberFiring = false;
    battleStubberLedState = false;
    return false;
  }

  return true; //Stil active
}


static void DisableBattleCannonStubberAction() {
  setLedValue(battleStubberPin, 0);
  battleStubberFiring = false;
  battleStubberLedState = false;
}

//---   Ironstorm Missle Pod   ---//
const uint8_t misslePodAudioTrack = 9;
static bool PerformIronstormMisslePodAction(unsigned long updateMillies) {
  playTrack(misslePodAudioTrack);

  return false;
}

static void DisableIronstormMisslePodAction() {

}

//---   Stormspear Rocket Pod   ---//
const uint8_t rocketPodAudioTrack = 10;

static bool PerformStormspearRocketPodAction(unsigned long updateMillies) {
  playTrack(rocketPodAudioTrack);
  return false;
}

static void DisableStormspearRocketPodAction() {

}

//---   Twin Icarus Auto Cannon   ---//
static int twinIcLeftLed = 5;
static int twinIcRightLed = 11;
bool twinIcFiring = false;
long twinIcStartTime = 0;
bool twinIcLedState = false;
bool twinIcPrevFiredSide = false; //False = Left, True = Right
long twinIcLastFireMillis = 0; 
const long twinIcFinalFireMillis = 2760; 
const long twinIcChannel = 0;
const long twinIcInitialDelay = 100;
const long twinIcFireInterval = 500;
const long twinIcLedOnTime = 100;
const uint8_t twinIcAudioTrack = 11;
static bool PerformTwinIcarusAutoCannonAction(unsigned long updateMillies) {
 if (!twinIcFiring) {
     twinIcFiring = true;
     twinIcPrevFiredSide = false;
     twinIcStartTime = updateMillies;
     twinIcLastFireMillis = twinIcStartTime - twinIcFireInterval + twinIcInitialDelay;

     playTrack(twinIcAudioTrack);
  }

  if (updateMillies - twinIcLastFireMillis >= twinIcFireInterval) {
    if (twinIcLedState == false) { //Only update if the LED needs to be toggled
      twinIcLastFireMillis = updateMillies;

      if (twinIcPrevFiredSide == true) {
        setLedValue(twinIcLeftLed, 254);
        twinIcPrevFiredSide = false;
      } else {
        setLedValue(twinIcRightLed, 254);
        twinIcPrevFiredSide = true;
      }
      
      twinIcLedState = true;
    }
    
  } else {
    if (twinIcLedState == true) {
      //Turn off LED after enough time has elapsed
      if (updateMillies - twinIcLastFireMillis >= twinIcLedOnTime) {
        setLedValue(twinIcLeftLed, 0);
        setLedValue(twinIcRightLed, 0);
        twinIcLedState = false;
      }
    }
  }

  if (updateMillies - twinIcStartTime >= twinIcFinalFireMillis) {
    setLedValue(twinIcLeftLed, 0);
    setLedValue(twinIcRightLed, 0);
    twinIcFiring = false;
    twinIcLedState = false;
    return false;
  }

  return true; //Stil active
}

static void DisableTwinIcarusAutoCannonAction() {
  setLedValue(twinIcLeftLed, 0);
  setLedValue(twinIcRightLed, 0);
  twinIcFiring = false;
  twinIcLedState = false;
}

//---   Titanic Feet   ---//
const uint8_t titanicFeetAudioTrack = 12;

static bool PerformTitanicFeetAction(unsigned long updateMillis) {
  playTrack(titanicFeetAudioTrack);
  return false;
}

//---   Ricochet   ---//
const uint8_t ricochetAudioTrack = 13;
static bool PerformRicochetAction(unsigned long updateMillis) {
  playTrack(ricochetAudioTrack);

  return false;
}

//---   Explosion   ---//
const uint8_t explosionAudioTrack = 14;
static bool PerformExplosionAction(unsigned long updateMillis) {
  playTrack(explosionAudioTrack);

  return false;
}

//--------------------//
//---   Handlers   ---//
//--------------------//
void blePeripheralConnectHandler(BLEDevice central) {
    Serial.println("Connected event, central: ");
    Serial.println(central.address());
}

void blePeripheralDisconnectHandler(BLEDevice central) {
    Serial.println("Disconnected event, central: ");
    Serial.println(central.address());

    unsigned long updateMillis = millis();
    CleanupEyePeripheral(updateMillis);
    CleanupTogglePeripheral(updateMillis);
    CleanupActionPeripheral(updateMillis);
    CleanupRandomActionPeripheral(updateMillis);
}


//-----------------//
//---   Setup   ---//
//-----------------//
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

  BLE.setConnectionInterval(0x000C, 0x000C);
  BLE.setSupervisionTimeout(3200);

  BLE.setEventHandler(BLEConnected, blePeripheralConnectHandler);
  BLE.setEventHandler(BLEDisconnected, blePeripheralDisconnectHandler);

  //---   Intialize LED Board   ---//
  LedManager.begin();
  LedManager.write();

  //---   Initialize Audio .  ---//
  if (!AudioManager.begin(Serial1)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    //while(true);
  }
  Serial.println(F("DFPlayer Mini online."));

  //---   Initilalize Peripherals    ---//
  InitializeEyePeripheral();
  InitializeTogglePeripheral();
  InitializeActionPeripheral();
  InitializeRandomActionPeripheral();


  //---   Initialize Services   ---//
  BLE.setLocalName(modelName);
  BLE.setAdvertisedService(modelService);

  BLE.addService(modelService);

  BLE.advertise();
  Serial.println("BLE Peripheral Engaged");
}

//----------------//
//---   Loop   ---//
//----------------//

void loop() {
  static long prvTime = 0;
  BLE.poll();

  unsigned long updateMillis = millis();

  if (updateMillis - prvTime >= 10) {
    prvTime = millis();
    if (BLE.connected()) {
      UpdateEyePeripheral(updateMillis);
      UpdateTogglePeripheral(updateMillis);
      UpdateActionPeripheral(updateMillis);
      UpdateRandomActionPeripheral(updateMillis);
    }
  }
}

//-------------------------//
//---   Random Action   ---//
//-------------------------//
char* randomActionPeripheralName = "Random Action Mode";
uint32_t randomActionTimerMinValueMs = 2*60000;
uint32_t randomActionTimerMaxValueMs = 4*60000;
uint16_t randomActionPeripheralOrder = 7;
char* randomActionOptionNames = "Horn|Avenger Gatling Cannon|Rapid Fire Battle Cannon|Las-Impulsor|Thermal Cannon|Reaper Chainsword|MeltaGun|Heavy Stubber|Battle Cannon Stubber|Ironstorm Missle Pod|Stormspear Rocket Pod|Twin Icarus Autocannon|Titanic Feet|Ricochet";

bool randomActionsEnabled = false;

int randomActionNumRandomActions = 14;

bool hornRandomActionActive = false;

bool avengerGatlingCannonRandomActionActive = false;
bool rapidFireBattleCannonRandomActionActive = false;
bool lasImpulsorRandomActionActive = false;
bool thermalCannonRandomActionActive = false;
bool reaperChainswordRandomActionActive = false;

bool meltaGunRandomActionActive = false;
bool heavyStubberRandomActionActive = false;
bool battleCannonStubberRandomActionActive = false;

bool ironstormMisslePodRandomActionActive = false;
bool stormspearRocketPodRandomActionActive = false;
bool twinIcarusAutoCannonRandomActionActive = false;

bool titanicFeetRandomActionActive = false;

bool ricochetRandomActionActive = false;


int randomActionNextActionTimeMS = 0;
int activeRandomActionIndex = -1;

BLEStringCharacteristic randomActionPeripheralNameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40);
BLEByteCharacteristic randomActionEnabledCharacteristic(ToggleCharacteristic, BLERead | BLEWrite | BLENotify);
BLECharacteristic randomActionPeripheralTriggerCharacteristic(RandomActionCharacteristic, BLERead | BLEWrite | BLENotify, 16);
BLEService randomActionPeripheralService("e60367d7-b293-4501-9a85-58370f7718bf");

//---   Initialize   --//
static void InitializeRandomActionPeripheral() {
  BLE.setAdvertisedService(randomActionPeripheralService);

  //Order  descriptor
  BLEDescriptor orderDescriptor(OrderDescriptor, reinterpret_cast<uint8_t*>(&randomActionPeripheralOrder), sizeof(uint16_t));
  randomActionPeripheralNameCharacteristic.addDescriptor(orderDescriptor);

  //Add descriptors to characteristics
  BLEDescriptor actionNameDescriptor(NameDescriptor, randomActionOptionNames);
  randomActionPeripheralTriggerCharacteristic.addDescriptor(actionNameDescriptor);

  BLEDescriptor enabledNameDescriptor(NameDescriptor, "Enabled");
  randomActionEnabledCharacteristic.addDescriptor(enabledNameDescriptor);

  //Initialize PropertyCharacteristics
  randomActionEnabledCharacteristic.writeValue(0);
  randomActionPeripheralNameCharacteristic.writeValue(randomActionPeripheralName);

  //Add characteristics to service
  randomActionPeripheralService.addCharacteristic(randomActionPeripheralNameCharacteristic);
  randomActionPeripheralService.addCharacteristic(randomActionEnabledCharacteristic);
  randomActionPeripheralService.addCharacteristic(randomActionPeripheralTriggerCharacteristic);

  BLE.addService(randomActionPeripheralService);

  Serial.print(randomActionPeripheralName);
  Serial.println(" initialized");
}

//---   Update   ---//
static void UpdateRandomActionPeripheral(unsigned long updateMillis) {
  //Check if random actions have been written
  if (randomActionEnabledCharacteristic.written()) {
    randomActionsEnabled = !randomActionsEnabled;

    switch (randomActionEnabledCharacteristic.value()) {
      case 01:
        randomActionsEnabled = true;
        randomActionNextActionTimeMS = updateMillis + random(randomActionTimerMinValueMs, randomActionTimerMaxValueMs);
        break;
      default:
        randomActionsEnabled = false;
        randomActionNextActionTimeMS = 0;
    }

    //Serial.print("randomActionsEnabled: ");
    //Serial.println(randomActionsEnabled);

    //Serial.print("updateMillis: ");
    //Serial.println(updateMillis);

    //Serial.print("randomActionNextActionTimeMS: ");
    //Serial.println(randomActionNextActionTimeMS);
  }

  //Check for options changing
  if (randomActionPeripheralTriggerCharacteristic.written()) {
    uint16_t actionValue;
    randomActionPeripheralTriggerCharacteristic.readValue(actionValue);

    //Flip active status
    switch(actionValue) {
      case 0:
        hornRandomActionActive = !hornRandomActionActive;
        break;
      case 1:
        avengerGatlingCannonRandomActionActive = !avengerGatlingCannonRandomActionActive;
        break;
      case 2:
        rapidFireBattleCannonRandomActionActive = !rapidFireBattleCannonRandomActionActive;
        break;
      case 3:
        lasImpulsorRandomActionActive = !lasImpulsorRandomActionActive;
        break;
      case 4:
        thermalCannonRandomActionActive = !thermalCannonRandomActionActive;
        break;
      case 5:
        reaperChainswordRandomActionActive = !reaperChainswordRandomActionActive;
        break;
      case 6:
        meltaGunRandomActionActive = !meltaGunRandomActionActive;
        break;
      case 7:
        heavyStubberRandomActionActive = !heavyStubberRandomActionActive;
        break;
      case 8:
        battleCannonStubberRandomActionActive = !battleCannonStubberRandomActionActive;
        break;
      case 9:
        ironstormMisslePodRandomActionActive = !ironstormMisslePodRandomActionActive;
        break;
      case 10:
        stormspearRocketPodRandomActionActive = !stormspearRocketPodRandomActionActive;
        break;
      case 11:
        twinIcarusAutoCannonRandomActionActive = !twinIcarusAutoCannonRandomActionActive;
        break;
      case 12:
        titanicFeetRandomActionActive = !titanicFeetRandomActionActive;
        break;
      case 13:
        ricochetRandomActionActive = !ricochetRandomActionActive;
        break;
    }
  }

  //If there is not ctive random index and enough time has elapsed then find a new index
  if (activeRandomActionIndex == -1 && updateMillis > randomActionNextActionTimeMS && randomActionsEnabled) {
    int numActiveActions = -1;
    if (hornRandomActionActive == true) { numActiveActions += 1; }
    if (avengerGatlingCannonRandomActionActive == true) { numActiveActions += 1; }
    if (rapidFireBattleCannonRandomActionActive == true) { numActiveActions += 1; }
    if (lasImpulsorRandomActionActive == true) { numActiveActions += 1; }
    if (thermalCannonRandomActionActive == true) { numActiveActions += 1; }
    if (reaperChainswordRandomActionActive == true) { numActiveActions += 1; }
    if (meltaGunRandomActionActive == true) { numActiveActions += 1; }
    if (heavyStubberRandomActionActive == true) { numActiveActions += 1; }
    if (battleCannonStubberRandomActionActive == true) { numActiveActions += 1; }
    if (ironstormMisslePodRandomActionActive == true) { numActiveActions += 1; }
    if (stormspearRocketPodRandomActionActive == true) { numActiveActions += 1; }
    if (twinIcarusAutoCannonRandomActionActive == true) { numActiveActions += 1; }
    if (titanicFeetRandomActionActive == true) { numActiveActions += 1; }
    if (ricochetRandomActionActive == true) { numActiveActions += 1; }

    //Find the action index
    if (numActiveActions > -1) {
      int index = random(0, numActiveActions);

      if (hornRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 0; }
        index -= 1;
      }
      if (avengerGatlingCannonRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 1; }
        index -= 1;
      }
      if (rapidFireBattleCannonRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 3; }
        index -= 1;
      }
      if (lasImpulsorRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }   
      if (thermalCannonRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }  
      if (reaperChainswordRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }  
      if (meltaGunRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }  
      if (heavyStubberRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }  
      if (battleCannonStubberRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }  
      if (ironstormMisslePodRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }  
      if (stormspearRocketPodRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }  
      if (twinIcarusAutoCannonRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }  
      if (titanicFeetRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }  
      if (ricochetRandomActionActive == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }  
    }

    Serial.print("activeRandomActionIndex: ");
    Serial.println(activeRandomActionIndex);
  }

  bool actionActive = false;
  switch(activeRandomActionIndex) {
    case 0:
      actionActive = PerformHornAction(updateMillis);
      break;
    case 1:
      actionActive = PerformAvengerGatlingCannonAction(updateMillis);
      break;
    case 2:
      actionActive = PerformRapidFireBattleCannonAction(updateMillis);
      break;
    case 3:
      actionActive = PerformLasImpulsorAction(updateMillis);
      break;   
    case 4:
      actionActive = PerformThermalCannonAction(updateMillis);
      break;   
    case 5:
      actionActive = PerformReaperChainswordAction(updateMillis);
      break;   
    case 6:
      actionActive = PerformMeltaGunAction(updateMillis);
      break;   
    case 7:
      actionActive = PerformHeavyStubberAction(updateMillis);
      break;   
    case 8:
      actionActive = PerformBattleCannonHeavyStubberAction(updateMillis);
      break;   
    case 9:
      actionActive = PerformIronstormMisslePodAction(updateMillis);
      break;   
    case 10:
      actionActive = PerformStormspearRocketPodAction(updateMillis);
      break;   
    case 11:
      actionActive = PerformTwinIcarusAutoCannonAction(updateMillis);
      break;   
    case 12:
      actionActive = PerformTitanicFeetAction(updateMillis);
      break;   
    case 13:
      actionActive = PerformRicochetAction(updateMillis);
      break;   
  }

  //If an action is no longer active but the index is set then the action just finished, so find a new update time
  if (actionActive == false && activeRandomActionIndex != -1 && randomActionsEnabled) {
    randomActionNextActionTimeMS = updateMillis + random(randomActionTimerMinValueMs, randomActionTimerMaxValueMs);
    activeRandomActionIndex = -1;

    Serial.print("updateMillis: ");
    Serial.println(updateMillis);

    Serial.print("randomActionNextActionTimeMS: ");
    Serial.println(randomActionNextActionTimeMS);
  }
}

//---   Cleanup   --//
static void CleanupRandomActionPeripheral(unsigned long updateMillis) {
  
}
