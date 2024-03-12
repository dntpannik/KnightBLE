#include <ArduinoBLE.h>
#include <Adafruit_TLC59711.h>
#include <DFRobotDFPlayerMini.h>

char* modelName = "Throne Breaker";


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
#define NUM_TLC59711  2
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
int eyeChannel1 = 12;
int eyeChannel2 = 13;
int eyeChannel3 = 14;
int eyeChannel4 = 15;

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

  setLedValue(eyeChannel1, 0);
  setLedValue(eyeChannel2, 0);
  setLedValue(eyeChannel3, 0);
  setLedValue(eyeChannel4, 0);

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

        setLedValue(eyeChannel1, 254);
        setLedValue(eyeChannel2, 254);
        setLedValue(eyeChannel3, 254);
        setLedValue(eyeChannel4, 254);
    
        break;
      }
      default:
      {
        Serial.print("LED Off: ");
        Serial.println(eyePeripheralName);
        eyePeripheralEnabled = false;

        setLedValue(eyeChannel1, 0);
        setLedValue(eyeChannel2, 0);
        setLedValue(eyeChannel3, 0);
        setLedValue(eyeChannel4, 0);
                
        break;
      }
    }
  }
}

//---   Cleanup .  ---//
static void CleanupEyePeripheral(unsigned long updateMillis) {
  setLedValue(eyeChannel1, 0);
  setLedValue(eyeChannel2, 0);
  setLedValue(eyeChannel3, 0);
  setLedValue(eyeChannel4, 0);
  eyePeripheralEnabled = false;
}

//-------------------//
//---   Toggles   ---//
//-------------------//
char* togglePeripheralName = "Actions";
uint16_t togglePeripheralOrder = 5;
char* togglePeripheralActionNames = "Left Arm Lights|Right Arm Lights|Exhaust";

bool leftArmLightsToggleActive = false;
bool rightArmLightsToggleActive = false;
bool exhaustToggleActive = false;

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
        leftArmLightsToggleActive = !leftArmLightsToggleActive;
        break;
      case 1: 
        rightArmLightsToggleActive = !rightArmLightsToggleActive;
        break;
      case 2:
        exhaustToggleActive = !exhaustToggleActive;
    }

    //If not active then run the disable method
    if (leftArmLightsToggleActive == false) {
      DisableLeftArmToggle(updateMillis);
    }
    if (rightArmLightsToggleActive == false) {
      DisableRightArmToggle(updateMillis);
    }
    if (exhaustToggleActive == false) {
      DisableExhaustToggle(updateMillis);
    }
  }

  //Update all active actions
  if (leftArmLightsToggleActive == true) {
    UpdateLeftArmToggle(updateMillis);
  }
  if (rightArmLightsToggleActive == true) {
    UpdatRightArmToggle(updateMillis);
  }
    if (exhaustToggleActive == true) {
    UpdateExhaustToggle(updateMillis);
  }
}

//---   Cleanup   ---//
static void CleanupTogglePeripheral(unsigned long updateMillis) {
  DisableLeftArmToggle(updateMillis);
  DisableRightArmToggle(updateMillis);
    DisableExhaustToggle(updateMillis);
}

//---   Left Arm Lights   ---//
static long leftArmPreviousUpdateTime = 0;
const int leftArmChannel = 1;
int leftArmUpdateDelay = 50;
int leftArmMinUpdateDelay = 50;
int leftArmMaxUpdateDelay = 150;
int leftArmBaseValue = 150;
int leftArmRandomValue = 80;

static void UpdateLeftArmToggle(unsigned long updateMillis) {
  //If the LEDs have updated recently then return
  if (updateMillis - leftArmPreviousUpdateTime < leftArmUpdateDelay) {
    return;
  }

  leftArmPreviousUpdateTime = updateMillis;
  
  //Genereate new LED values
  int ledValue = leftArmBaseValue + random(1, leftArmRandomValue);
  
  setLedValue(leftArmChannel, ledValue);

  //Generate new update delay
  leftArmUpdateDelay = random(leftArmMinUpdateDelay, leftArmMaxUpdateDelay);   
}

static void DisableLeftArmToggle(unsigned long updateMillis) {
  setLedValue(leftArmChannel, 0);
}

//---   Right Arm Lights   ---//
static long rightArmPreviousUpdateTime = 0;
const int rightArmChannel1 = 4;
const int rightArmChannel2 = 10;
int rightArmUpdateDelay = 50;
int rightArmMinUpdateDelay = 50;
int rightArmMaxUpdateDelay = 150;
int rightArmBaseValue = 150;
int rightArmRandomValue = 80;

static void UpdatRightArmToggle(unsigned long updateMillis) {
  //If the LEDs have updated recently then return
  if (updateMillis - rightArmPreviousUpdateTime < rightArmUpdateDelay) {
    return;
  }

  rightArmPreviousUpdateTime = updateMillis;
  
  //Genereate new LED values
  int ledValue = rightArmBaseValue + random(1, rightArmRandomValue);
  
  setLedValue(rightArmChannel1, ledValue);
  setLedValue(rightArmChannel2, ledValue);

  //Generate new update delay
  rightArmUpdateDelay = random(rightArmMinUpdateDelay, rightArmMaxUpdateDelay);   
}

static void DisableRightArmToggle(unsigned long updateMillis) {
  setLedValue(rightArmChannel1, 0);
  setLedValue(rightArmChannel2, 0);
}

//---   Exhaust   ---//
static long exhaustPreviousUpdateTime = 0;
const int exhaustChannel1 = 20;
const int exhaustChannel2 = 21;
const int exhaustChannel3 = 22;
const int exhaustChannel4 = 23;
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

//--------------------//
//---   Actions    ---//
//--------------------//
char* ActionPeripheralName = "Actions";
uint16_t ActionPeripheralOrder = 6;
char* ActionPeripheralActionNames = "Horn|Conflag Cannon|Twin Melta|Siege Breaker Cannons|Siege Breaker Missles|Titanic Feet|Ricochet|Explosion";

bool hornActionActive = false;
bool conflagCannonActionActive = false;
bool twinMeltaActionActive = false;
bool siegeBreakerCannonsActionActive = false;
bool siegeBreakerMisslesActionActive = false;
bool titanicFeetActionActive = false;
bool ricochetActionActive = false;
bool explosionActionActive = false;

BLEStringCharacteristic ActionPeripheralNameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40);
BLEService ActionPeripheralService("d47a3f8d-e29d-4964-a7f8-b870e99ca792");
BLECharacteristic ActionPeripheralTriggerCharacteristic(ActionCharacteristic, BLERead | BLEWrite | BLENotify, 16);

//---   Initialize .  ---//
static void InitializeActionPeripheral() {
  BLE.setAdvertisedService(ActionPeripheralService);

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
        conflagCannonActionActive = true;
        break;
      case 2:
        twinMeltaActionActive = true;
        break;
      case 3:
        siegeBreakerCannonsActionActive = true;
        break;
      case 4:
        siegeBreakerMisslesActionActive = true;
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

  if (conflagCannonActionActive == true) {
    conflagCannonActionActive = PerformConflagCannonAction(updateMillis);
  }

  if (twinMeltaActionActive == true) {
    twinMeltaActionActive = PerformTwinMeltaAction(updateMillis);
  }

  if (siegeBreakerCannonsActionActive == true) {
    siegeBreakerCannonsActionActive = PerformSiegeBreakerCannonsAction(updateMillis);
  }

  if (siegeBreakerMisslesActionActive == true) {
    siegeBreakerMisslesActionActive = PerformSiegeBreakerMisslesAction(updateMillis);
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
  CleanupConflagCannonAction();
  ResetTwinMeltaAction();
  ResetSiegeBreakerCannonAction();
}


//---   Horn   ---//
const uint8_t hornAudioTrack = 1;
static bool PerformHornAction(unsigned long updateMillis) {
  playTrack(hornAudioTrack);

  return false;
}

//---   Conflag Cannon .  ---//
const uint8_t conflagCannonAudioTrack = 2;
const int conflagCannonChannel1 = 0;
const int conflagCannonChannel2 = 2;
const int conflagCannonChannel3 = 11;

const uint16_t conflagCannonAudioInitialDelayMS = 2000;
const uint16_t conflagConnonAudioFirstLengthMS = 3500;
const uint16_t conflagCannonAudioFirstIntermissionDelayMS = 1200;
const uint16_t conflagCannonAudioSecondLengthMS = 5000;
const uint16_t conflagCannonAudioSecondIntermissionDelayMS = 1200;
const uint16_t conflagCannonAudioThirdLengthMS = 1500;

const int conflagCannonMinUpdateDelay = 50;
const int conflagCannonMaxUpdateDelay = 150;
const int conflagCannonBaseValue = 200;
const int conflagCannonRandomValue = 55;

bool conflagCannonActive;
bool conflagCannonInitialDelayComplete;
bool conflagCannonFirstLengthComplete;
bool conflagCannonFirstIntermissionComplete;
bool conflagCannonSecondLengthComplete;
bool conflagCannonSecondIntermissionComplete;
bool conflagCannonThirdLengthComplete;

unsigned long conflagCannonTimerMS = 0;
int conflagCannonUpdateDelay = 50;
uint16_t conflagCannonPreviousUpdateTimeMS = 0;

static bool PerformConflagCannonAction(unsigned long updateMillis) {

  //Check if should be active
  if (conflagCannonActive == false) {
    Serial.println("Starting Conflag Cannon Action");

    conflagCannonActive = true;
    conflagCannonPreviousUpdateTimeMS = updateMillis;

    conflagCannonInitialDelayComplete = false;
    conflagCannonFirstLengthComplete = false;
    conflagCannonFirstIntermissionComplete = false;
    conflagCannonSecondLengthComplete = false;
    conflagCannonSecondIntermissionComplete = false;
    conflagCannonThirdLengthComplete = false;

    playTrack(conflagCannonAudioTrack);

    //Start timer
    conflagCannonTimerMS = updateMillis;

    Serial.println("  Starting initial delay timer");

    return true;
  }

  //Initial delay
  if (conflagCannonInitialDelayComplete == false) {
    if (HasTimeElapsed(updateMillis, conflagCannonTimerMS, conflagCannonAudioInitialDelayMS)) {
      conflagCannonInitialDelayComplete = true;

      //Start timer for Leds On
      conflagCannonTimerMS = updateMillis;
      Serial.println("  Intial delay timer complete. Turning on Leds");
    }

    return true;
  }

  //First Led On
  if (conflagCannonFirstLengthComplete == false) {
    if (!HasTimeElapsed(updateMillis, conflagCannonTimerMS, conflagConnonAudioFirstLengthMS)) {
      UpdateConflagCannonLeds(updateMillis);

    } else {
      //Turn off leds
      setLedValue(conflagCannonChannel1, 0);
      setLedValue(conflagCannonChannel2, 0);
      setLedValue(conflagCannonChannel3, 0);

      conflagCannonFirstLengthComplete = true;

      //Start timer for audio delay
      conflagCannonTimerMS = updateMillis;

      Serial.println("  Initial firing complete. Starting timer");
    }

    return true;
  }

  //First Intermission
  if (conflagCannonFirstIntermissionComplete == false) {
    if (HasTimeElapsed(updateMillis, conflagCannonTimerMS, conflagCannonAudioFirstIntermissionDelayMS)) {
      conflagCannonFirstIntermissionComplete = true;

      //Start timer for leds on
      conflagCannonTimerMS = updateMillis;

      Serial.println("  First Intermission complete. Turning on Leds");
    }

    return true;
  }

  //Second led on
  if (conflagCannonSecondLengthComplete == false) {
    if (!HasTimeElapsed(updateMillis, conflagCannonTimerMS, conflagCannonAudioSecondLengthMS)) {
      UpdateConflagCannonLeds(updateMillis);

    } else {
      //Turn off leds
      setLedValue(conflagCannonChannel1, 0);
      setLedValue(conflagCannonChannel2, 0);
      setLedValue(conflagCannonChannel3, 0);

      conflagCannonSecondLengthComplete = true;

      //Start timer for audio delay
      conflagCannonTimerMS = updateMillis;

      Serial.println("  Second firing complete. Starting timer");
    }

    return true;
  }

  //Second Intermission
  if (conflagCannonSecondIntermissionComplete == false) {
    if (HasTimeElapsed(updateMillis, conflagCannonTimerMS, conflagCannonAudioSecondIntermissionDelayMS)) {
      conflagCannonSecondIntermissionComplete = true;

      //Start timer for leds on
      conflagCannonTimerMS = updateMillis;

      Serial.println("  Second Intermission complete. Turning on Leds");
    }

    return true;
  }

  //Third led on
  if (conflagCannonThirdLengthComplete == false) {
    if (!HasTimeElapsed(updateMillis, conflagCannonTimerMS, conflagCannonAudioThirdLengthMS)) {
      UpdateConflagCannonLeds(updateMillis);
      return true; 

    } else {
      //Turn off leds
      setLedValue(conflagCannonChannel1, 0);
      setLedValue(conflagCannonChannel2, 0);
      setLedValue(conflagCannonChannel3, 0);

      conflagCannonThirdLengthComplete = true;
      
      Serial.println("  Firing complete");
      conflagCannonActive = false;

      return false; //Complete
    }
  }

  return false; //Should never hit
}

static void UpdateConflagCannonLeds(unsigned long updateMillis) {
  //If the LEDs have updated recently then return
  if (!HasTimeElapsed(updateMillis, conflagCannonPreviousUpdateTimeMS, conflagCannonUpdateDelay)) {
    return;
  }

  conflagCannonPreviousUpdateTimeMS = updateMillis;
  
  //Genereate new LED values
  int ledValue = conflagCannonBaseValue + random(1, conflagCannonRandomValue);

  setLedValue(conflagCannonChannel1, ledValue);
  setLedValue(conflagCannonChannel2, ledValue);
  setLedValue(conflagCannonChannel3, ledValue);

  //Generate new update delay
  conflagCannonUpdateDelay = random(conflagCannonMinUpdateDelay, conflagCannonMaxUpdateDelay); 
}

static void CleanupConflagCannonAction() {
    conflagCannonActive = true;
    conflagCannonPreviousUpdateTimeMS = 0;
    conflagCannonTimerMS = 0;

    conflagCannonInitialDelayComplete = false;
    conflagCannonFirstLengthComplete = false;
    conflagCannonFirstIntermissionComplete = false;
    conflagCannonSecondLengthComplete = false;
    conflagCannonSecondIntermissionComplete = false;
    conflagCannonThirdLengthComplete = false;

  setLedValue(conflagCannonChannel1, 0);
  setLedValue(conflagCannonChannel2, 0);
  setLedValue(conflagCannonChannel3, 0);
}

//---   Twin Melta Action .  ---//
static int twinMeltaPin1 = 7;
static int twinMeltaPin2 = 8;
const uint8_t twinMeltaAudioTrack = 5;
const uint8_t twinMeltaNumberOfShots = 6;

const uint16_t twinMeltaInitialDelayMS = 50;
const uint16_t twinMeltaLedOnTimeMS = 200;
const uint16_t twinMeltaTimeBetweenShotsMS = 270;


bool twinMeltaActive;
bool twinMeltaInitialDelayComplete;
bool twinMeltaShotLedEnabled;
bool twinMeltaShotLedDisabled;

uint8_t twinMeltaCurrentShot;
bool twinMeltaSide = false; //False/True based on which pin should be enabled

unsigned long twinMeltaTimerMS = 0;

static bool PerformTwinMeltaAction(unsigned long updateMillis) {
 if (!twinMeltaActive) {
    ResetTwinMeltaAction();

    twinMeltaActive = true;
    Serial.println("Starting Twin Melta Action");

    playTrack(twinMeltaAudioTrack);

    //Start the timer
    twinMeltaTimerMS = updateMillis;
  }

  if (twinMeltaInitialDelayComplete == false)
  {
    if (HasTimeElapsed(updateMillis, twinMeltaTimerMS, twinMeltaInitialDelayMS)) {
      twinMeltaInitialDelayComplete = true;

      Serial.println("  Initial delay complete. Starting firing sequence.");
    }

    return true;
  }

  //Check if firing is complete
  if (twinMeltaCurrentShot >= twinMeltaNumberOfShots) {
    Serial.println("  Action Complete");
    ResetTwinMeltaAction();
    return false;
  }

  //Fire sequence
  //1) Turn on LED
  //2) Wait
  //3) Turn off LED
  if (twinMeltaShotLedEnabled == false) {
    Serial.println("  LED Enabled. Timer started");
    if (twinMeltaSide) {
      setLedValue(twinMeltaPin1, 254);
    } else {
      setLedValue(twinMeltaPin2, 254);
    }

    twinMeltaShotLedEnabled = true;
    twinMeltaTimerMS = updateMillis;
    return true;
  }

  if (twinMeltaShotLedDisabled == false) {
    if (HasTimeElapsed(updateMillis, twinMeltaTimerMS, twinMeltaLedOnTimeMS)) {
      Serial.println("  Turning off LED. Timer started");
      if (twinMeltaSide) {
        setLedValue(twinMeltaPin1, 0);
      } else {
        setLedValue(twinMeltaPin2, 0);
      }

      twinMeltaShotLedDisabled = true;
      twinMeltaTimerMS = updateMillis;
    }

    return true;
  }

  //If off time is elapsed then flip side, reset shot flags, increment counter
  if (HasTimeElapsed(updateMillis, twinMeltaTimerMS, twinMeltaTimeBetweenShotsMS)) {
    twinMeltaSide = !twinMeltaSide;
    twinMeltaShotLedEnabled = false;
    twinMeltaShotLedDisabled = false;

    twinMeltaCurrentShot += 1;
  }


  return true;
}

static void ResetTwinMeltaAction() {
  twinMeltaActive = false;
  
  twinMeltaInitialDelayComplete = false;
  twinMeltaShotLedEnabled = false;
  twinMeltaShotLedDisabled = false;

  twinMeltaCurrentShot = 0;
  twinMeltaSide = false;

  setLedValue(twinMeltaPin1, 0);
  setLedValue(twinMeltaPin2, 0);
}

//---   Siege Breaker Cannon Action    ---//
static int siegeBreakerCannonPin1 = 3;
static int siegeBreakerCannonPin2 = 9;
const uint8_t siegeBreakerCannonAudioTrack = 3;
const uint8_t siegeBreakerCannonNumberOfShots = 3;

const uint16_t siegeBreakerCannonInitialDelayMS = 500;
const uint16_t siegeBreakerCannonLedOnTimeMS = 300;
const uint16_t siegeBreakerCannonTimeBetweenShotsMS = 900;

bool siegeBreakerCannonActive;
bool siegeBreakerCannonInitialDelayComplete;
bool siegeBreakerCannonShotLedEnabled;
bool siegeBreakerCannonShotLedDisabled;

uint8_t siegeBreakerCannonCurrentShot;
bool siegeBreakerCannonSide = false; //False/True based on which pin should be enabled

unsigned long siegeBreakerCannonTimerMS = 0;

static bool PerformSiegeBreakerCannonsAction(unsigned long updateMillis) {
 if (!siegeBreakerCannonActive) {
    ResetSiegeBreakerCannonAction();

    siegeBreakerCannonActive = true;
    Serial.println("Starting Siege Breaker Cannon Action");

    playTrack(siegeBreakerCannonAudioTrack);

    //Start the timer
    siegeBreakerCannonTimerMS = updateMillis;
  }

  if (siegeBreakerCannonInitialDelayComplete == false)
  {
    if (HasTimeElapsed(updateMillis, siegeBreakerCannonTimerMS, siegeBreakerCannonInitialDelayMS)) {
      siegeBreakerCannonInitialDelayComplete = true;

      Serial.println("  Initial delay complete. Starting firing sequence.");
    }

    return true;
  }

  //Check if firing is complete
  if (siegeBreakerCannonCurrentShot >= siegeBreakerCannonNumberOfShots) {
    ResetSiegeBreakerCannonAction();
    return false;
  }

  //Fire sequence
  //1) Turn on LED
  //2) Wait
  //3) Turn off LED
  if (siegeBreakerCannonShotLedEnabled == false) {
    Serial.println("  LED Enabled. Timer started");
    if (siegeBreakerCannonSide) {
      setLedValue(siegeBreakerCannonPin1, 254);
    } else {
      setLedValue(siegeBreakerCannonPin2, 254);
    }

    siegeBreakerCannonShotLedEnabled = true;
    siegeBreakerCannonTimerMS = updateMillis;
    return true;
  }

  if (siegeBreakerCannonShotLedDisabled == false) {
    if (HasTimeElapsed(updateMillis, siegeBreakerCannonTimerMS, siegeBreakerCannonLedOnTimeMS)) {
      Serial.println("  Turning off LED. Timer started");
      if (siegeBreakerCannonSide) {
        setLedValue(siegeBreakerCannonPin1, 0);
      } else {
        setLedValue(siegeBreakerCannonPin2, 0);
      }

      siegeBreakerCannonShotLedDisabled = true;
      siegeBreakerCannonTimerMS = updateMillis;
    }

    return true;
  }

  //If off time is elapsed then flip side, reset shot flags, increment counter
  if (HasTimeElapsed(updateMillis, siegeBreakerCannonTimerMS, siegeBreakerCannonTimeBetweenShotsMS)) {
    siegeBreakerCannonSide = !siegeBreakerCannonSide;
    siegeBreakerCannonShotLedEnabled = false;
    siegeBreakerCannonShotLedDisabled = false;

    siegeBreakerCannonCurrentShot += 1;
  }

  return true;
}

static void ResetSiegeBreakerCannonAction() {
  siegeBreakerCannonActive = false;
  
  siegeBreakerCannonInitialDelayComplete = false;
  siegeBreakerCannonShotLedEnabled = false;
  siegeBreakerCannonShotLedDisabled = false;

  siegeBreakerCannonCurrentShot = 0;
  siegeBreakerCannonSide = false;

  setLedValue(siegeBreakerCannonPin1, 0);
  setLedValue(siegeBreakerCannonPin2, 0);
}

//---   Siege Breaker Missles Action    ---//
const uint8_t siegeBreakerMisslesAudioTrack = 4;
static bool PerformSiegeBreakerMisslesAction(unsigned long updateMillis) {
  playTrack(siegeBreakerMisslesAudioTrack);
  return false;
}

//---   Titanic Feet   ---//
const uint8_t titanicFeetAudioTrack = 3;

static bool PerformTitanicFeetAction(unsigned long updateMillis) {
  playTrack(titanicFeetAudioTrack);
  return false;
}

//---   Ricochet   ---//
const uint8_t ricochetAudioTrack = 4;
static bool PerformRicochetAction(unsigned long updateMillis) {
  playTrack(ricochetAudioTrack);

  return false;
}

//---   Explosion   ---//
const uint8_t explosionAudioTrack = 5;
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
char* randomActionOptionNames = "Horn|Conflag Cannon|Twin Melta|Siege Breaker Cannons|Siege Breaker Missles|Titanic Feet|Ricochet";

bool randomActionsEnabled = false;

int randomActionNumRandomActions = 7;
bool hornActionEnabled = true;
bool conflagCannonActionEnabled = false;
bool twinMeltaActionEnabled = false;
bool siegeBreakerCannonsActionEnabled = false;
bool siegeBreakerMisslesActionEnabled = false;
bool titanicFeetActionEnabled = true;
bool ricochetActionEnabled = true;

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
        hornActionEnabled = !hornActionEnabled;
        break;
      case 1:
        conflagCannonActionEnabled = !conflagCannonActionEnabled;
        break;
      case 2:
        twinMeltaActionEnabled = !twinMeltaActionEnabled;
        break;
      case 3:
        siegeBreakerCannonsActionEnabled = !siegeBreakerCannonsActionEnabled;
        break;
      case 4:
        siegeBreakerMisslesActionEnabled = !siegeBreakerMisslesActionEnabled;
        break;
      case 5:
        titanicFeetActionEnabled = !titanicFeetActionEnabled;
        break;
      case 6:
        ricochetActionEnabled = !ricochetActionEnabled;
        break;
    }
  }

  //If there is not ctive random index and enough time has elapsed then find a new index
  if (activeRandomActionIndex == -1 && updateMillis > randomActionNextActionTimeMS && randomActionsEnabled) {
    int numActiveActions = -1;
    if (hornActionEnabled == true) { numActiveActions += 1; }
    if (conflagCannonActionEnabled == true) { numActiveActions += 1; }
    if (twinMeltaActionEnabled == true) { numActiveActions += 1; }
    if (siegeBreakerCannonsActionEnabled == true) { numActiveActions += 1; }
    if (siegeBreakerMisslesActionEnabled == true) { numActiveActions += 1; }
    if (titanicFeetActionEnabled == true) { numActiveActions += 1; }
    if (ricochetActionEnabled == true) { numActiveActions += 1; }

    //Find the action index
    if (numActiveActions > -1) {
      int index = random(0, numActiveActions);

      if (hornActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 0; }
        index -= 1;
      }
      if (conflagCannonActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 1; }
        index -= 1;
      }
      if (twinMeltaActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 1; }
        index -= 1;
      }
      if (siegeBreakerCannonsActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 1; }
        index -= 1;
      }
      if (siegeBreakerMisslesActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 1; }
        index -= 1;
      }
      if (titanicFeetActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 3; }
        index -= 1;
      }
      if (ricochetActionEnabled == true) {
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
      actionActive = PerformConflagCannonAction(updateMillis);
      break;
    case 2:
      actionActive = PerformTwinMeltaAction(updateMillis);
      break;
    case 3:
      actionActive = PerformSiegeBreakerCannonsAction(updateMillis);
      break;
    case 4:
      actionActive = PerformSiegeBreakerMisslesAction(updateMillis);
      break;
    case 5:
      actionActive = PerformTitanicFeetAction(updateMillis);
      break;
    case 6:
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

//-------------------//
//---   Helpers   ---//
//-------------------//
static bool HasTimeElapsed(unsigned long currentTime, unsigned long startTime, uint16_t desiredDelay ) {
  return currentTime - startTime > desiredDelay;
}