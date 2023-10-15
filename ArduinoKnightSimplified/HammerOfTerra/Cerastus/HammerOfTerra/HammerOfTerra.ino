#include <ArduinoBLE.h>
#include <Adafruit_TLC59711.h>
#include <DFRobotDFPlayerMini.h>

char* modelName = "Hammer of Terra";


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
//Channels, 0, 1, 2

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
  setLedValue(1, 0);
  setLedValue(2, 0);

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
        setLedValue(1, 254);
        setLedValue(2, 254);
    
        break;
      }
      default:
      {
        Serial.print("LED Off: ");
        Serial.println(eyePeripheralName);
        eyePeripheralEnabled = false;

        setLedValue(0, 0);
        setLedValue(1, 0);
        setLedValue(2, 0);
                
        break;
      }
    }
  }
}

//---   Cleanup .  ---//
static void CleanupEyePeripheral(unsigned long updateMillis) {
  setLedValue(0, 0);
  setLedValue(1, 0);
  setLedValue(2, 0);
  eyePeripheralEnabled = false;
}

//-------------------//
//---   Toggles   ---//
//-------------------//
char* togglePeripheralName = "Actions";
uint16_t togglePeripheralOrder = 5;
char* togglePeripheralActionNames = "Vents";

bool ventToggleActive = false;

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
        ventToggleActive = !ventToggleActive;
        break;
    }

    //If not active then run the disable method
    if (ventToggleActive == false) {
      DisableVentToggle(updateMillis);
    }
  }

  //Update all active actions
  if (ventToggleActive == true) {
    UpdateVentToggle(updateMillis);
  }
}

//---   Cleanup   ---//
static void CleanupTogglePeripheral(unsigned long updateMillis) {
  DisableVentToggle(updateMillis);
}

//---   Vents   ---//
static long ventPreviousUpdateTime = 0;
const int ventChannel1 = 3;
const int ventChannel2 = 4;
int ventUpdateDelay = 50;
int ventMinUpdateDelay = 50;
int ventMaxUpdateDelay = 150;
int ventBaseValue = 150;
int ventRandomValue = 80;

static void UpdateVentToggle(unsigned long updateMillis) {
  //If the LEDs have updated recently then return
  if (updateMillis - ventPreviousUpdateTime < ventUpdateDelay) {
    return;
  }

  ventPreviousUpdateTime = updateMillis;
  
  //Genereate new LED values
  int ledValue = ventBaseValue + random(1, ventRandomValue);
  
  setLedValue(ventChannel1, ledValue);
  setLedValue(ventChannel2, ledValue);

  //Generate new update delay
  ventUpdateDelay = random(ventMinUpdateDelay, ventMaxUpdateDelay);   
}

static void DisableVentToggle(unsigned long updateMillis) {
  setLedValue(ventChannel1, 0);
  setLedValue(ventChannel2, 0);
}

//--------------------//
//---   Actions    ---//
//--------------------//
char* ActionPeripheralName = "Actions";
uint16_t ActionPeripheralOrder = 6;
char* ActionPeripheralActionNames = "Horn|Bolt Cannons|Titanic Feet|Ricochet|Explosion";

bool hornActionActive = false;
bool boltCannonsActionActive = false;
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
        boltCannonsActionActive = true;
        break;
      case 2:
        titanicFeetActionActive = true;
        break;
      case 3:
        ricochetActionActive = true;
        break;
      case 4:
        explosionActionActive = true;
        break;    
    }
  }

  //Update all active actions
  if (hornActionActive == true) {
    hornActionActive = PerformHornAction(updateMillis);
  }

  if (boltCannonsActionActive == true) {
    boltCannonsActionActive = PerformBoltCannonsAction(updateMillis);
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
  DisableBoltCannonsAction();
}


//---   Horn   ---//
const uint8_t hornAudioTrack = 1;
static bool PerformHornAction(unsigned long updateMillis) {
  playTrack(hornAudioTrack);

  return false;
}

//---   Bolt Cannons .  ---//
static long boltCannonPreviousUpdateTime = 0;
const int boltCannonStartPin = 7;
const int boltCannonEndPin = 11;
bool boltCannonFiring = false;
bool boltCannonLedState = false;
int boltCannonCurrentFirePin = 7;
long boltCannonStartTime = 0;
long boltCannonLastFireMillis = 0;
const long boltCannonFinalFireMillis = 2200;
const long boltCannonInitialDelay = 200;
const long boltCannonFireInterval = 60;
const long boltCannonLedOnTime = 30;
const uint8_t boltCannonAudioTrack = 2;

static bool PerformBoltCannonsAction(unsigned long updateMillis) {
  if (!boltCannonFiring) {
    boltCannonFiring = true;
    boltCannonStartTime = updateMillis;
    boltCannonLastFireMillis = boltCannonStartTime - boltCannonFireInterval + boltCannonInitialDelay;

    playTrack(boltCannonAudioTrack);
  }

  if (updateMillis - boltCannonLastFireMillis >= boltCannonFireInterval) {
    if (boltCannonLedState == false) { //Only update if the LED needs to be toggled
      boltCannonLastFireMillis = updateMillis;
      setLedValue(boltCannonCurrentFirePin, 254);

      boltCannonLedState = true;
    } else {
      if (boltCannonLedState == true) {
        //Turn off LED after enough time has elapsed
        if (updateMillis - boltCannonLastFireMillis >= boltCannonLedOnTime) {
          setLedValue(boltCannonCurrentFirePin, 0);

          //Increment the bolt cannon pin. Wrap if too high
          boltCannonCurrentFirePin += 1;
          if (boltCannonCurrentFirePin > boltCannonEndPin) {
            boltCannonCurrentFirePin = boltCannonStartPin;
          }

          boltCannonLedState = false;
        }
      }
    }

    //If firing is complete reset
    if (updateMillis - boltCannonStartTime >= boltCannonFinalFireMillis) {

      //Turn off all pins
      for (int i = boltCannonStartPin; i <= boltCannonEndPin; i++) {
        setLedValue(i, 0);
      }
      
      //Reset flags
      boltCannonFiring = false;
      boltCannonLedState = false;
      boltCannonCurrentFirePin = boltCannonStartPin;
      return false;
    }

    return true; //Still active
  }  
}

static bool DisableBoltCannonsAction() {
  for (int i = boltCannonStartPin; i <= boltCannonEndPin; i++) {
    setLedValue(i, 0);
  }
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
    }
  }
}