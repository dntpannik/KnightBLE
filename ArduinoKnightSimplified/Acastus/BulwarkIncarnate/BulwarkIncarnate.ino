#include <ArduinoBLE.h>
#include <Adafruit_TLC59711.h>
#include <DFRobotDFPlayerMini.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

char* modelName = "Bulwark Incarnate";


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

//-----------------------//
//---   Servo Board   ---//
//-----------------------//
#define SERVOMIN  15 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

static bool SetServo(uint8_t channel, int* currentPosition, int* delta, uint16_t servoStep) {
    if (abs(*delta) > 0) {
      int direction = (*delta > 0) - (*delta < 0);

      Serial.print("move servo: ");
      Serial.print(channel);
      Serial.print(" (Cur: ");
      Serial.print(*currentPosition);
      Serial.print(", D: ");
      Serial.print(*delta);
      Serial.print(", Dir: ");
      Serial.print(direction);

      //If current delta is less than the servo step then just set position to delta
      if (abs(*delta) < servoStep) {
        (*currentPosition) += *delta;
        (*delta) = 0;
      } else {
        //Increment the position by the servo step
        (*currentPosition) += servoStep * direction; //Update position
        (*delta) -= servoStep * direction; //Update delta
      }

      pwm.setPWM(channel, 0, *currentPosition); //Move servo

      Serial.print(", New Pos: ");
      Serial.print(*currentPosition);
      Serial.print(", New D: ");
      Serial.print(*delta);
      Serial.println(")");

      return abs(*delta) <= 0; //Movement not complete if delta isn't 0
    } else {
      return true; //Movement complete
    }
}

static void DisableServo(uint8_t channel) {
  pwm.setPWM(channel, 0, 4096);
}

//----------------------//
//---   LED Board    ---//
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
//Channels, 0

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

  setLedValue(0, 0);

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

        setLedValue(0, 254);
    
        break;
      }
      default:
      {
        Serial.print("LED Off: ");
        Serial.println(eyePeripheralName);
        eyePeripheralEnabled = false;

        setLedValue(0, 0);
                
        break;
      }
    }
  }
}

//---   Cleanup .  ---//
static void CleanupEyePeripheral(unsigned long updateMillis) {
  setLedValue(0, 0);
  eyePeripheralEnabled = false;
}

//--------------------//
//---   Actions    ---//
//--------------------//
char* ActionPeripheralName = "Actions";
uint16_t ActionPeripheralOrder = 6;
char* ActionPeripheralActionNames = "Horn|Auto Cannon|Missles|Twin Magma Lascannon|Titanic Feet|Ricochet|Explosion";

bool hornActionActive = false;
bool autoCannonActionActive = false;
bool missleBayActionActive = false;
bool twinMagmaLascannonActionActive = false;
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

  //Initialize Actions
  InitializeMissleBayAction();
  InitializeMagmaLascannonAction();

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
        missleBayActionActive = true;
        break;
      case 3:
        twinMagmaLascannonActionActive = true;
        break;
      case 4:
        titanicFeetActionActive = true;
        break;
      case 5:
        ricochetActionActive = true;
        break;
      case 6:
        explosionActionActive = true;
        break;  
    }
  }

  //Update all active actions
  if (hornActionActive == true) {
    hornActionActive = PerformHornAction(updateMillis);
  }

  if (autoCannonActionActive == true) {
    autoCannonActionActive = PerformAutoCannonAction(updateMillis);
  }

  if (missleBayActionActive == true) {
    missleBayActionActive = PerformMissleBayAction(updateMillis);
  }

  if (twinMagmaLascannonActionActive == true) {
    twinMagmaLascannonActionActive = PerformMagmaLascannonAction(updateMillis);
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
  CleanupMissleBayAction();
  CleanupMagmaLascannonAction();
  DisableAutoCannonAction();
}


//---   Horn   ---//
const uint8_t hornAudioTrack = 1;
static bool PerformHornAction(unsigned long updateMillis) {
  playTrack(hornAudioTrack);

  return false;
}

//---   Auto Cannon    ---//
static int autoCannonPin1 = 4;
static int autoCannonPin2 = 5;
bool autoCannonFiring = false;
long autoCannonStartTime = 0;
bool autoCannonLedState = false;
long autoCannonLastFireMillis = 0; 
const long autoCannonFinalFireMillis = 2200; 
const long autoCannonInitialDelay = 200;
const long autoCannonFireInterval = 60;
const long autoCannonLedOnTime = 30;
const uint8_t autoCannonAudioTrack = 2;

static bool PerformAutoCannonAction(unsigned long updateMillis) {
 if (!autoCannonFiring) {
     autoCannonFiring = true;
     autoCannonStartTime = updateMillis;
     autoCannonLastFireMillis = autoCannonStartTime - autoCannonFireInterval + autoCannonInitialDelay;

      playTrack(autoCannonAudioTrack);
  }

  if (updateMillis - autoCannonLastFireMillis >= autoCannonFireInterval) {
    if (autoCannonLedState == false) { //Only update if the LED needs to be toggled
      autoCannonLastFireMillis = updateMillis;
      setLedValue(autoCannonPin1, 254);
      setLedValue(autoCannonPin2, 254);

      autoCannonLedState = true;
    }
    
  } else {
    if (autoCannonLedState == true) {
      //Turn off LED after enough time has elapsed
      if (updateMillis - autoCannonLastFireMillis >= autoCannonLedOnTime) {
        setLedValue(autoCannonPin1, 0);
        setLedValue(autoCannonPin2, 0);
        autoCannonLedState = false;
      }
    }
  }

  if (updateMillis - autoCannonStartTime >= autoCannonFinalFireMillis) {
    setLedValue(autoCannonPin1, 0);
    setLedValue(autoCannonPin2, 0);
    autoCannonFiring = false;
    autoCannonLedState = false;
    return false;
  }

  return true; //Stil active
}

static void DisableAutoCannonAction() {
  setLedValue(autoCannonPin1, 0);
  setLedValue(autoCannonPin2, 0);
  autoCannonFiring = false;
  autoCannonLedState = false;
}

//---   Missle Bay   ---//
//Constants
const uint8_t missleBayAudioTrack = 4;
const uint8_t missleBayServoPin = 8;
Servo missleBayServo;
const uint8_t missleBayActionServoClosedPosition = 135;
const uint8_t missleBayActionServoOpenPosition = 50;
const uint8_t missleBayActionServoMSPerStep = 10;
const uint16_t missleBayActionServoPauseDurationMS = 1000;
const uint16_t missleBayActionAudioDuration = 9000;

//Action Status
bool missleBayActive = false;
unsigned long missleBayActionStartTimeMS = 0;
unsigned long missleBayActionPreviousServoUpdateTimeMS = 0;
int missleBayActionServoCurrentPosition = missleBayActionServoClosedPosition;
bool missleBayActionBayOpen = false;

unsigned long missleBayActionAudioStartTimeMS = 0;
bool missleBayActionAudioTriggered = false;
unsigned long missleBayActionCloseStartTimeMS = 0;

static void InitializeMissleBayAction() {
  missleBayServo.attach(missleBayServoPin);
  missleBayServo.write(missleBayActionServoClosedPosition);
  missleBayServo.detach();
}

static bool PerformMissleBayAction(unsigned long updateMillis) {
    

    //Check if the action just started
    if (missleBayActive == false) {
      Serial.println("Starting Missle Bay Action");
      missleBayActive = true;
      missleBayActionStartTimeMS = updateMillis;
      missleBayActionPreviousServoUpdateTimeMS = updateMillis;

      missleBayActionBayOpen = false;
      missleBayActionAudioStartTimeMS = 0;
      missleBayActionAudioTriggered = false;

      missleBayActionCloseStartTimeMS = 0;

      missleBayServo.attach(missleBayServoPin);
      missleBayServo.write(missleBayActionServoClosedPosition);
      return true;
    }

    //1) Open bay
    if (missleBayActionBayOpen == false && updateMillis - missleBayActionPreviousServoUpdateTimeMS >= missleBayActionServoMSPerStep) {
      Serial.print("Writing Servo Position: ");
      Serial.println(missleBayActionServoCurrentPosition);
      missleBayActionPreviousServoUpdateTimeMS = updateMillis;
      missleBayActionServoCurrentPosition -= 1;
      missleBayServo.write(missleBayActionServoCurrentPosition);

      if (missleBayActionServoCurrentPosition <= missleBayActionServoOpenPosition) {
        missleBayActionAudioStartTimeMS = updateMillis + missleBayActionServoPauseDurationMS;
        missleBayActionBayOpen = true;
        missleBayServo.detach();

        Serial.println("Missle Pod Open");
      }

      return true;
    }

    //2) Play sound
    if (missleBayActionBayOpen == true && missleBayActionAudioTriggered == false && updateMillis >= missleBayActionAudioStartTimeMS) {
      missleBayActionAudioTriggered = true;
      playTrack(missleBayAudioTrack);

      Serial.println("Starting Audio");

      //Start the clock on closing the missle bay
      missleBayActionCloseStartTimeMS = updateMillis + missleBayActionAudioDuration + missleBayActionServoPauseDurationMS;
      missleBayActionPreviousServoUpdateTimeMS = missleBayActionCloseStartTimeMS;

      return true;
    }

    //3) Close Bay
    if (missleBayActionAudioTriggered == true && updateMillis >= missleBayActionCloseStartTimeMS) {
      if (missleBayServo.attached() == false) {
        missleBayServo.attach(missleBayServoPin);
      }

      if (updateMillis - missleBayActionPreviousServoUpdateTimeMS >= missleBayActionServoMSPerStep) {
        Serial.print("updateMillis: ");
        Serial.println(updateMillis);
        Serial.print("Writing Servo Position: ");
        Serial.println(missleBayActionServoCurrentPosition);

        missleBayActionPreviousServoUpdateTimeMS = updateMillis;
        missleBayActionServoCurrentPosition += 1;
        missleBayServo.write(missleBayActionServoCurrentPosition);

        if (missleBayActionServoCurrentPosition >= missleBayActionServoClosedPosition) {
          Serial.println("Finished Missle Pod Action: ");
          CleanupMissleBayAction();
          return false;
        }
      }
    }

  return true;
}

static void CleanupMissleBayAction() {
  missleBayActive = false;
  missleBayActionBayOpen = false;
  missleBayActionStartTimeMS = 0;
  missleBayActionAudioStartTimeMS = 0;
  missleBayActionAudioTriggered = false;
  missleBayActionCloseStartTimeMS = 0;

  missleBayServo.attach(missleBayServoPin);
  missleBayServo.write(missleBayActionServoClosedPosition);
  missleBayServo.detach();
}

//---------------------------//
//---   Magma Lascannon   ---//
//---------------------------//
//---   Constants   ---//
//Audio
const uint8_t magmaLascannonAudioTrack = 5;

//Servos
const uint8_t rightArmServoChannel = 2;
const uint8_t leftArmServoChannel = 1;

const uint8_t leftArmServoPin = 6;
const uint8_t rightArmServoPin = 5;
const uint8_t magmaLascannonSlowServoMSPerStep = 0; //How many MS between servo increments for slow movement
const uint8_t magmaLascannonFastServoMSPerStep = 0;
const uint16_t magmaLascannonSlowServoStep = 8; //How far servo should move in a single increment
const uint16_t magmaLascannonFastServoStep = 10;

//LEDs
const uint8_t rightTopGunLedPin = 6;
const uint8_t rightBottomGunLedPin = 7;
const uint8_t leftTopGunLedPin = 10;
const uint8_t leftBottomGunLedPin = 11;

//Warmup
const int16_t rightArmServoOffsetFrom90 = 10;
const int16_t leftArmServoOffsetFrom90 = -15;
const uint16_t rightArmServoRestPosition = map(rightArmServoOffsetFrom90 + 80, 0, 180, SERVOMIN, SERVOMAX); //Position where the action begins
const uint16_t leftArmServoRestPosition = map(leftArmServoOffsetFrom90 + 100, 0, 180, SERVOMIN, SERVOMAX);
const uint16_t magmaLascannonInitialRestingPositionDelayMS = 1000; //Delay between when the servos reach resting position and move to firing position
const uint16_t rightArmServoFirePosition = map(rightArmServoOffsetFrom90 + 90, 0, 180, SERVOMIN, SERVOMAX); //Position where firing occures
const uint16_t leftArmServoFirePosition = map(leftArmServoOffsetFrom90 + 90, 0, 180, SERVOMIN, SERVOMAX);
const uint16_t magmaLascannonInitialFirePositionDelayMS = 2000; //Delay between when the guns reach initial firing position and first fire

//Gun Firing
const uint16_t magmaLascannonAudioDelayMS = 350; //Delay between when the audio starts and the LED enables
const uint16_t magmaLascannonLedOnTimeMS = 250;
const uint16_t rightMagmaLascannonRecoilPosition = map(rightArmServoOffsetFrom90 + 115, 0, 180, SERVOMIN, SERVOMAX); //Position of maximum recoil
const uint16_t leftMagmaLascannonRecoilPosition = map(leftArmServoOffsetFrom90 + 70, 0, 180, SERVOMIN, SERVOMAX);
const uint16_t magmaLascannonRecoilDelayMS = 250; //Delay between when the gun reaches its recoil position and when it returns to firing position
const uint16_t magmaLascannonDelayBetweenGuns = 500; //Delay between when one gun finishes its firing sequence and the next gun starts

//---   Action Variables   ---//
bool magmaLascannonActive = false;
unsigned long magmaLascannonStartTimeMS = 0;
unsigned long magmaLascannonPreviousServoUpdateTimeMS = 0;
int magmaLascannonRightServoCurrentPosition = 0;
int magmaLascannonLeftServoCurrentPosition = 0;

int magmaLascannonRightServoCurrentMoveDelta = 0; //How far the right servo has to move for the current part of the action
int magmaLascannonLeftServoCurrentMoveDelta = 0;

unsigned long magmaLascannonTimerStartMS = 0; //Timer used for calculating delays

//---   Action Stage Flags   ---//
//Stages
bool magmaLascannonInitialRestingReached; //Indicates whether the initial resting position at the start of the motion has been hit
bool magmaLascannonInitialRestingDelayReached; //Indicates whether the desired delay between reaching initial resting position and moving to shorting position has been hit
bool magmaLascannonInitialFiringPositionReached;
bool magmaLascannonInitialFiringDelayReached; //Indicates whether the desired delay between reaching initial firing position and the shooting has been hit
bool magmaLascannonTopRightGunFired;
bool magmaLascannonTopLeftGunFired;
bool magmaLascannonBottomRightGunFired;
bool magmaLascannonBottomLeftGunFired;
bool magmaLascannonFinalRestingReached; //Indicates whether the final resting position has been hit

//Gun Firing
bool magmaLascannonGunAudioStarted;
bool magmaLascannonGunAudioDelayReached;
bool magmaLascannonGunLedEnabled;
bool magmaLascannonGunLedDisabled;
bool magmaLascannonGunRecoilReached;
bool magmaLascannonGunRecoilDelayReached;
bool magmaLascannonGunReturnToFiringPositionReached;
bool magmaLascannonGunFinishDelayReached;

static void InitializeMagmaLascannonAction() {

}

//Resets all flags controlling the movement of a magma lascannon gun action
static void ResetMagmaLascannonGunFlags() {
  magmaLascannonGunAudioStarted = false;
  magmaLascannonGunAudioDelayReached = false;
  magmaLascannonGunLedEnabled = false;
  magmaLascannonGunLedDisabled = false;
  magmaLascannonGunRecoilReached = false;
  magmaLascannonGunRecoilDelayReached = false;
  magmaLascannonGunReturnToFiringPositionReached = false;
  magmaLascannonGunFinishDelayReached = false;
}

static bool PerformMagmaLascannonAction(unsigned long updateMillis) {

  //Initial Arm Actions:
  //  1) Start at -10 degrees from horizontal
  //  2) Short Pause
  //  3) Raise arms to 0 degrees from horizontal slowly
  //  4) Short Pause
  //  5) Top Right Gun
  //  6) Top Left Gun
  //  7) Bottom Right Gun
  //  8) Bottom Left Gun
  //  9) Move to Resting

  //Check if the action just started - Initialize variables
  if (magmaLascannonActive == false) {
    Serial.println("Starting Magma Lascannon Action");
    //Timings
    magmaLascannonActive = true;
    magmaLascannonStartTimeMS = updateMillis;
    magmaLascannonPreviousServoUpdateTimeMS = updateMillis;

    //Initialize sevo
    magmaLascannonRightServoCurrentPosition = rightArmServoRestPosition;
    magmaLascannonRightServoCurrentMoveDelta = rightArmServoRestPosition - magmaLascannonRightServoCurrentPosition;
    pwm.setPWM(rightArmServoChannel, 0, rightArmServoRestPosition);
    DisableServo(rightArmServoChannel);

    magmaLascannonLeftServoCurrentPosition = leftArmServoRestPosition;
    magmaLascannonLeftServoCurrentMoveDelta = leftArmServoRestPosition - magmaLascannonLeftServoCurrentPosition;
    pwm.setPWM(leftArmServoChannel, 0, leftArmServoRestPosition);
    DisableServo(leftArmServoChannel);

    Serial.print("Current: ");
    Serial.print(magmaLascannonLeftServoCurrentPosition);
    Serial.print(" Goal: ");
    Serial.print(leftArmServoRestPosition);
    Serial.print(" Delta: ");
    Serial.println(magmaLascannonLeftServoCurrentMoveDelta);

    Serial.print("Resting Position: ");
    Serial.println(leftArmServoRestPosition);
    Serial.print("Firing Position: ");
    Serial.println(leftArmServoFirePosition);
    Serial.print("Recoil Position: ");
    Serial.println(leftMagmaLascannonRecoilPosition);

    //Reset flags
    magmaLascannonInitialRestingReached = true;
    magmaLascannonInitialRestingDelayReached = false;
    magmaLascannonInitialFiringPositionReached = false;
    magmaLascannonInitialFiringDelayReached = false;
    magmaLascannonTopRightGunFired = false;
    magmaLascannonTopLeftGunFired = false;
    magmaLascannonBottomRightGunFired = false;
    magmaLascannonBottomLeftGunFired = false;
    magmaLascannonFinalRestingReached = false;

    return true;
  }

  //1) Move to starting position
  if (magmaLascannonInitialRestingReached == false) {
    //If too early to update servos just return
    if (ShouldUpdateMagmaLascannonServo(updateMillis, magmaLascannonSlowServoMSPerStep) == false) 
      return true;

    //Move servos
    bool rightComplete = SetServo(rightArmServoChannel, &magmaLascannonRightServoCurrentPosition, &magmaLascannonRightServoCurrentMoveDelta, magmaLascannonSlowServoStep);
    bool leftComplete = SetServo(leftArmServoChannel, &magmaLascannonLeftServoCurrentPosition, &magmaLascannonLeftServoCurrentMoveDelta, magmaLascannonSlowServoStep);

    magmaLascannonInitialRestingReached = rightComplete && leftComplete; //Initial resting position reached if both right/left servos are in position

    //If position reached then start the delay timer for first firing
    if (magmaLascannonInitialRestingReached) {
        Serial.println("Finished move to resting position");
        magmaLascannonTimerStartMS = updateMillis;

        DisableServo(rightArmServoChannel);
        DisableServo(leftArmServoChannel);
    }

    return true;
  }

  //2) Short Pause
  if (magmaLascannonInitialRestingDelayReached == false) {
    magmaLascannonInitialRestingDelayReached = MagmaLascannonHasTimeElapsed(updateMillis, magmaLascannonTimerStartMS, magmaLascannonInitialRestingPositionDelayMS);

    //If pause completed then update deltas to firing position
    if (magmaLascannonInitialRestingDelayReached) {
        Serial.println("Finished delay between resting position and start of move to firing position");
        magmaLascannonRightServoCurrentMoveDelta = rightArmServoFirePosition - magmaLascannonRightServoCurrentPosition;
        magmaLascannonLeftServoCurrentMoveDelta = leftArmServoFirePosition - magmaLascannonLeftServoCurrentPosition;
    }
    return true;
  }

  //3) Move to firing position
  if (magmaLascannonInitialFiringPositionReached == false) {
    //If too early to update servos just return
    if (ShouldUpdateMagmaLascannonServo(updateMillis, magmaLascannonSlowServoMSPerStep) == false) 
      return true;

      bool rightComplete = SetServo(rightArmServoChannel, &magmaLascannonRightServoCurrentPosition, &magmaLascannonRightServoCurrentMoveDelta, magmaLascannonSlowServoStep);
      bool leftComplete = SetServo(leftArmServoChannel, &magmaLascannonLeftServoCurrentPosition, &magmaLascannonLeftServoCurrentMoveDelta, magmaLascannonSlowServoStep);

      magmaLascannonInitialFiringPositionReached = rightComplete && leftComplete; //Firing position reached if both right/left servos are in position

      //If position reached then start the delay timer for first firing
      if (magmaLascannonInitialFiringPositionReached) {
        Serial.println("Finished move to firing position");
        magmaLascannonTimerStartMS = updateMillis;

        DisableServo(rightArmServoChannel);
        DisableServo(leftArmServoChannel);
      }

      return true;
  }

  //4) Short Pause
  if (magmaLascannonInitialFiringDelayReached == false) {
    magmaLascannonInitialFiringDelayReached = MagmaLascannonHasTimeElapsed(updateMillis, magmaLascannonTimerStartMS, magmaLascannonInitialFirePositionDelayMS);

    //If delay finished then Fire guns
    if (magmaLascannonInitialFiringDelayReached) {
      Serial.println("Finished delay between move to firing position and start of firing top right gun");

      ResetMagmaLascannonGunFlags();
      Serial.println("---   Firing top right gun  ---");
    }

    return true;
  }

  //5) Fire Top Right Gun
  if (magmaLascannonTopRightGunFired == false) {
    magmaLascannonTopRightGunFired = MagmaLascannonFireGun(rightArmServoChannel, rightTopGunLedPin, rightArmServoFirePosition, rightMagmaLascannonRecoilPosition, &magmaLascannonRightServoCurrentPosition, &magmaLascannonRightServoCurrentMoveDelta, updateMillis);

    if (magmaLascannonTopRightGunFired) {
      ResetMagmaLascannonGunFlags();
      Serial.println("---   Firing top left gun  ---");
    }

    return true;
  }

  //6) Fire Top Left Gun
  if (magmaLascannonTopLeftGunFired == false) {
    magmaLascannonTopLeftGunFired = MagmaLascannonFireGun(leftArmServoChannel, leftTopGunLedPin, leftArmServoFirePosition, leftMagmaLascannonRecoilPosition, &magmaLascannonLeftServoCurrentPosition, &magmaLascannonLeftServoCurrentMoveDelta, updateMillis);

    if (magmaLascannonTopLeftGunFired) {
      ResetMagmaLascannonGunFlags();
      Serial.println("---   Firing bottom right gun  ---");
    }

    return true;
  }

  //7) Fire Bottom Right Gun
  if (magmaLascannonBottomRightGunFired == false) {
    magmaLascannonBottomRightGunFired = MagmaLascannonFireGun(rightArmServoChannel, rightBottomGunLedPin, rightArmServoFirePosition, rightMagmaLascannonRecoilPosition, &magmaLascannonRightServoCurrentPosition, &magmaLascannonRightServoCurrentMoveDelta, updateMillis);

    if (magmaLascannonBottomRightGunFired) {
      ResetMagmaLascannonGunFlags();
      Serial.println("---   Firing bottom left gun  ---");
    }

    return true;
  }

  //8) Fire Bottom Left Gun
  if (magmaLascannonBottomLeftGunFired == false) {
    magmaLascannonBottomLeftGunFired = MagmaLascannonFireGun(leftArmServoChannel, leftBottomGunLedPin, leftArmServoFirePosition, leftMagmaLascannonRecoilPosition, &magmaLascannonLeftServoCurrentPosition, &magmaLascannonLeftServoCurrentMoveDelta, updateMillis);
 
    if (magmaLascannonBottomLeftGunFired) {
      ResetMagmaLascannonGunFlags();

      //Set deltas to resting position
      magmaLascannonRightServoCurrentMoveDelta = rightArmServoRestPosition - magmaLascannonRightServoCurrentPosition;
      magmaLascannonLeftServoCurrentMoveDelta = leftArmServoRestPosition - magmaLascannonLeftServoCurrentPosition;
    }

    return true;
  }
  
  //9) Move to resting position
  if (magmaLascannonFinalRestingReached == false) {
    //If too early to update servos just return
    if (ShouldUpdateMagmaLascannonServo(updateMillis, magmaLascannonSlowServoMSPerStep) == false) 
      return true;

      bool rightComplete = SetServo(rightArmServoChannel, &magmaLascannonRightServoCurrentPosition, &magmaLascannonRightServoCurrentMoveDelta, magmaLascannonSlowServoStep);
      bool leftComplete = SetServo(leftArmServoChannel, &magmaLascannonLeftServoCurrentPosition, &magmaLascannonLeftServoCurrentMoveDelta, magmaLascannonSlowServoStep);

      magmaLascannonFinalRestingReached = rightComplete && leftComplete; //Firing position reached if both right/left servos are in position

      //If position reached then start then action is complete
      if (magmaLascannonFinalRestingReached) {
        Serial.println("Finished move to firing resting position");
        DisableServo(rightArmServoChannel);
        DisableServo(leftArmServoChannel);
        magmaLascannonActive = false;

        return false;
      }

      return true;
  }

  return false; //Should never be hit
}

static bool MagmaLascannonFireGun(uint8_t servoChannel, uint8_t ledPin, uint16_t firingServoPosition, uint16_t recoilServoPosition, int* currentServoPosition, int* currentServoDelta, unsigned long updateMillis) {
  // Following sequence for top left, top right, bottom left, bottom right
  //  a) Gun fire Audio / Enable LED
  //  b) Delay for audio to start
  //  c) Enable LED 
  //  d) Disable after short duration
  //  e) Gun rises quickly to +30 degrees from horizontal
  //  f) Short Pause
  //  g) Gun moves to 0 degrees slowly
  //  h) Delay before firing of next gun

  //A - Play audio
  if (magmaLascannonGunAudioStarted == false) {
    Serial.println("  A: Start Audio");
    playTrack(magmaLascannonAudioTrack);
    magmaLascannonTimerStartMS = updateMillis; //Start timer
    magmaLascannonGunAudioStarted = true;

    return false;
  }

  //B - Delay to start LED
  if (magmaLascannonGunAudioDelayReached == false) {
    magmaLascannonGunAudioDelayReached = MagmaLascannonHasTimeElapsed(updateMillis, magmaLascannonTimerStartMS, magmaLascannonAudioDelayMS);
    if (magmaLascannonGunAudioDelayReached) {
      Serial.println(" B: Wait to turn on LED");
    }
    
    return false;
  }

  //C - Turn on LED
  if (magmaLascannonGunLedEnabled == false) {
    Serial.println("  C: Turn on LED");
    setLedValue(ledPin, 254);
    magmaLascannonTimerStartMS = updateMillis; //Start timer
    magmaLascannonGunLedEnabled = true;
  }
    
  //D - Turn off LED
  if (magmaLascannonGunLedDisabled == false) {
    magmaLascannonGunLedDisabled = MagmaLascannonHasTimeElapsed(updateMillis, magmaLascannonTimerStartMS, magmaLascannonLedOnTimeMS);
    if (magmaLascannonGunLedDisabled) {
      Serial.println("  D: Turn off LED");
      setLedValue(ledPin, 0);

      //Update delta for recoil
      *currentServoDelta = recoilServoPosition - *currentServoPosition;
    }

    return false;
  }

  //E - Move to recoil position
  if (magmaLascannonGunRecoilReached == false) {
  //If too early to update servos just return
  if (ShouldUpdateMagmaLascannonServo(updateMillis, magmaLascannonFastServoMSPerStep) == false) 
    return false;

    magmaLascannonGunRecoilReached = SetServo(servoChannel, currentServoPosition, currentServoDelta, magmaLascannonFastServoStep); //Passing a pointer so dont need the to use &, is already an address

    //If position reached then start the delay timer for first firing
    if (magmaLascannonGunRecoilReached) {
      Serial.println("  E: Finished move to recoil position");
      magmaLascannonTimerStartMS = updateMillis; //Start timer
      DisableServo(rightArmServoChannel);
    }

    return false;
  }

  //F - Delay at recoil before moving to firing position
  if (magmaLascannonGunRecoilDelayReached == false) {
    magmaLascannonGunRecoilDelayReached = MagmaLascannonHasTimeElapsed(updateMillis, magmaLascannonTimerStartMS, magmaLascannonRecoilDelayMS);

    if (magmaLascannonGunRecoilDelayReached) {
      Serial.println("  F: Delay at recoil position finished");

      //Update delta for firing position
      *currentServoDelta = firingServoPosition - *currentServoPosition;
    }

    return false;
  }

  //G - Move to firing position
  if (magmaLascannonGunReturnToFiringPositionReached == false) {
    //If too early to update servos just return
    if (ShouldUpdateMagmaLascannonServo(updateMillis, magmaLascannonSlowServoMSPerStep) == false) 
      return false;

    magmaLascannonGunReturnToFiringPositionReached = SetServo(servoChannel, currentServoPosition, currentServoDelta, magmaLascannonSlowServoStep); //Passing a pointer so dont need the to use &, is already an address

    //If position reached then start the delay timer for completion of gun action
    if (magmaLascannonGunReturnToFiringPositionReached) {
      Serial.println("  G: Finished move to recoil position");
      DisableServo(servoChannel);
      magmaLascannonTimerStartMS = updateMillis; //Start timer
    }

    return false;
  }

  //H - Delay before completion of gun firing event and start of next
  if (magmaLascannonGunFinishDelayReached == false) {
    magmaLascannonGunFinishDelayReached = MagmaLascannonHasTimeElapsed(updateMillis, magmaLascannonTimerStartMS, magmaLascannonDelayBetweenGuns);

    if (magmaLascannonGunFinishDelayReached) {
      Serial.println("  H: Delay at after firing");

      //return completion
      return true;
    }

    return false;
  }
}

//Checks if a timer has elapsed
static bool MagmaLascannonHasTimeElapsed(unsigned long currentTime, unsigned long startTime, uint16_t desiredDelay ) {
  return currentTime - startTime > desiredDelay;
}

static bool ShouldUpdateMagmaLascannonServo(unsigned long updateMillis, uint8_t msPerStep) {
  bool shouldUpdate = updateMillis - magmaLascannonPreviousServoUpdateTimeMS >= msPerStep;
  if (shouldUpdate) {
    magmaLascannonPreviousServoUpdateTimeMS = updateMillis;
  }

  return shouldUpdate;
}

static void CleanupMagmaLascannonAction() {
  magmaLascannonActive = false;
  magmaLascannonStartTimeMS = 0;
  magmaLascannonPreviousServoUpdateTimeMS = 0;
  magmaLascannonRightServoCurrentPosition = 0;

  DisableServo(rightArmServoChannel);
  DisableServo(leftArmServoChannel);
}


//---   Titanic Feet   ---//
const uint8_t titanicFeetAudioTrack = 6;

static bool PerformTitanicFeetAction(unsigned long updateMillis) {
  playTrack(titanicFeetAudioTrack);
  return false;
}

//---   Ricochet   ---//
const uint8_t ricochetAudioTrack = 7;
static bool PerformRicochetAction(unsigned long updateMillis) {
  playTrack(ricochetAudioTrack);

  return false;
}

//---   Explosion   ---//
const uint8_t explosionAudioTrack = 8;
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

//-------------------//
//---   Toggles   ---//
//-------------------//
char* togglePeripheralName = "Actions";
uint16_t togglePeripheralOrder = 5;
char* togglePeripheralActionNames = "Exhaust|Carapice|Missle Bay";

bool exhaustToggleActive = false;
bool carapaceToggleActive = false;
bool missleBayToggleActive = false;

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
        carapaceToggleActive = !carapaceToggleActive;
        break;
      case 2:
        missleBayToggleActive = !missleBayToggleActive;
    }

    //If not active then run the disable method
    if (exhaustToggleActive == false) {
      DisableExhaustToggle(updateMillis);
    }
    if (carapaceToggleActive == false) {
      DisableCarapaceToggle(updateMillis);
    }
    if (missleBayToggleActive == false) {
      DisableMissleBayToggle(updateMillis);
    }
  }

  //Update all active actions
  if (exhaustToggleActive == true) {
    UpdateExhaustToggle(updateMillis);
  }
  if (carapaceToggleActive == true) {
    UpdateCarapaceToggle(updateMillis);
  }
  if (missleBayToggleActive == true) {
    UpdateMissleBayToggle(updateMillis);
  }
}

//---   Cleanup   ---//
static void CleanupTogglePeripheral(unsigned long updateMillis) {
  DisableExhaustToggle(updateMillis);
  DisableCarapaceToggle(updateMillis);
}

//---   Exhaust   ---//
static long exhaustPreviousUpdateTime = 0;
const int exhaustChannel1 = 8;
const int exhaustChannel2 = 9;
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

  //Generate new update delay
  exhaustUpdateDelay = random(exhaustMinUpdateDelay, exhaustMaxUpdateDelay);   
}

static void DisableExhaustToggle(unsigned long updateMillis) {
  setLedValue(exhaustChannel1, 0);
  setLedValue(exhaustChannel2, 0);
}

//---   Carapace   ---//
static long carapacePreviousUpdateTime = 0;
const int carapaceChannel1 = 1;
const int carapaceChannel2 = 2;
const int carapaceChannel3 = 3;
int carapaceUpdateDelay = 50;
int carapaceMinUpdateDelay = 50;
int carapaceMaxUpdateDelay = 150;
int carapaceBaseValue = 150;
int carapaceRandomValue = 80;

static void UpdateCarapaceToggle(unsigned long updateMillis) {
//If the LEDs have updated recently then return
  if (updateMillis - carapacePreviousUpdateTime < carapaceUpdateDelay) {
    return;
  }

  carapacePreviousUpdateTime = updateMillis;
  
  //Genereate new LED values
  int ledValue = carapaceBaseValue + random(1, carapaceRandomValue);
  
  setLedValue(carapaceChannel1, ledValue);
  setLedValue(carapaceChannel2, ledValue);
  setLedValue(carapaceChannel3, ledValue);

  //Generate new update delay
  carapaceUpdateDelay = random(carapaceMinUpdateDelay, carapaceMaxUpdateDelay); 
}

static void DisableCarapaceToggle(unsigned long updateMillis) {
  setLedValue(carapaceChannel1, 0);
  setLedValue(carapaceChannel2, 0);
  setLedValue(carapaceChannel3, 0);
}

//---   Missle Bay   ---//
static void UpdateMissleBayToggle(unsigned long updateMillis) {
  if (missleBayActionServoCurrentPosition != missleBayActionServoOpenPosition) {
      missleBayServo.attach(missleBayServoPin);
      missleBayServo.write(missleBayActionServoOpenPosition);
      missleBayActionServoCurrentPosition = missleBayActionServoOpenPosition;
      delay(500);
      missleBayServo.detach();
  }
}

static void DisableMissleBayToggle(unsigned long updateMillis) {
  if (missleBayActionServoCurrentPosition != missleBayActionServoClosedPosition) {
      missleBayServo.attach(missleBayServoPin);
      missleBayServo.write(missleBayActionServoClosedPosition);
      missleBayActionServoCurrentPosition = missleBayActionServoClosedPosition;
      delay(500);
      missleBayServo.detach();
  }
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

  //---   Initialize Servo Board   ---//
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates

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
char* randomActionOptionNames = "Horn|Auto Cannon|Missles|Twin Magma Lascannon|Titanic Feet|Ricochet";

bool randomActionsEnabled = false;

int randomActionNumRandomActions = 5;
bool hornActionEnabled = true;
bool autoCannonActionEnabled = false;
bool missleBayActionEnabled = false;
bool twinMagmaLascannonActionEnabled = false;
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
        autoCannonActionEnabled = !autoCannonActionEnabled;
        break;
      case 2:
        missleBayActionEnabled = !missleBayActionEnabled;
        break;
      case 3:
        twinMagmaLascannonActionEnabled = !twinMagmaLascannonActionEnabled;
        break;
      case 4:
        titanicFeetActionEnabled = !titanicFeetActionEnabled;
        break;
      case 5:
        ricochetActionEnabled = !ricochetActionEnabled;
        break;
    }
  }

  //If there is not ctive random index and enough time has elapsed then find a new index
  if (activeRandomActionIndex == -1 && updateMillis > randomActionNextActionTimeMS && randomActionsEnabled) {
    int numActiveActions = -1;
    if (hornActionEnabled == true) { numActiveActions += 1; }
    if (autoCannonActionEnabled == true) { numActiveActions += 1; }
    if (missleBayActionEnabled == true) { numActiveActions += 1; }
    if (twinMagmaLascannonActionEnabled == true) { numActiveActions += 1; }
    if (titanicFeetActionEnabled == true) { numActiveActions += 1; }
    if (ricochetActionEnabled == true) { numActiveActions += 1; }

    //Find the action index
    if (numActiveActions > -1) {
      int index = random(0, numActiveActions);

      if (hornActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 0; }
        index -= 1;
      }
      if (autoCannonActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 1; }
        index -= 1;
      }
      if (missleBayActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 2; }
        index -= 1;
      }
      if (twinMagmaLascannonActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 2; }
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
      actionActive = PerformAutoCannonAction(updateMillis);
      break;
    case 2:
      actionActive = PerformMissleBayAction(updateMillis);
      break;
    case 3:
      actionActive = PerformMagmaLascannonAction(updateMillis);
      break;
    case 4:
      actionActive = PerformTitanicFeetAction(updateMillis);
      break;
    case 5:
      actionActive = PerformRicochetAction(updateMillis);
      break;   
  }

  //If an action is no longer active but the index is set then the action just finished, so find a new update time
  if (actionActive == false && activeRandomActionIndex != -1 && randomActionsEnabled) {
    randomActionNextActionTimeMS = updateMillis + random(randomActionTimerMinValueMs, randomActionTimerMaxValueMs);
    activeRandomActionIndex = -1;

    //Serial.print("updateMillis: ");
    //Serial.println(updateMillis);

    //Serial.print("randomActionNextActionTimeMS: ");
    //Serial.println(randomActionNextActionTimeMS);
  }
}

//---   Cleanup   --//
static void CleanupRandomActionPeripheral(unsigned long updateMillis) {
  
}
