#include <ArduinoBLE.h>
#include <Adafruit_TLC59711.h>
#include <DFRobotDFPlayerMini.h>

char* modelName = "Unbound Scion";


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
//Channels, 0, 1

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
    
        break;
      }
      default:
      {
        Serial.print("LED Off: ");
        Serial.println(eyePeripheralName);
        eyePeripheralEnabled = false;

        setLedValue(0, 0);
        setLedValue(1, 0);
                
        break;
      }
    }
  }
}

//---   Cleanup .  ---//
static void CleanupEyePeripheral(unsigned long updateMillis) {
  setLedValue(0, 0);
  setLedValue(1, 0);
  eyePeripheralEnabled = false;
}

//-----------------//
//---   Sides    ---//
//-----------------//

char* sidePeripheralName = "Sides";
uint16_t sidePeripheralOrder = 2;
bool sidePeripheralEnabled = false;
int sideLedChannel1 = 3;
int sideLedChannel2 = 4;

BLEStringCharacteristic sidePeripheralNameCharacteristic(NameCharacteristic, BLERead | BLENotify, 40);
BLEByteCharacteristic sidePeripheralToggleCharacteristic(ToggleCharacteristic, BLERead | BLEWrite | BLENotify);
BLEService sidePeripheralService("b53e830b-4250-4cd6-accc-fed7640e6eea");

static void InitializeSidePeripheral() {
  BLE.setAdvertisedService(sidePeripheralService);

  //Order descriptor
  BLEDescriptor orderDescriptor(OrderDescriptor, reinterpret_cast<uint8_t*>(&sidePeripheralOrder), sizeof(uint16_t));
  sidePeripheralNameCharacteristic.addDescriptor(orderDescriptor);

  //Initialize PropertyCharacteristics
  sidePeripheralNameCharacteristic.writeValue(sidePeripheralName);

  setLedValue(sideLedChannel1, 0);
  setLedValue(sideLedChannel2, 0);
  
  sidePeripheralToggleCharacteristic.writeValue(0);

  //Add characteristics to service
  sidePeripheralService.addCharacteristic(sidePeripheralNameCharacteristic);
  sidePeripheralService.addCharacteristic(sidePeripheralToggleCharacteristic);

  BLE.addService(sidePeripheralService);
 
  Serial.print(sidePeripheralName);
  Serial.println(" initialized");
}

//---   Update   ---//
static void UpdateSidePeripheral(unsigned long updateMillis) {
  bool updated = false;
  
  if (sidePeripheralToggleCharacteristic.written()) {
    updated = true;  

    switch (sidePeripheralToggleCharacteristic.value()) {
      case 01:
      {
        Serial.print("LED On: ");
        Serial.println(sidePeripheralName);
        sidePeripheralEnabled = true;

        setLedValue(sideLedChannel1, 254);
        setLedValue(sideLedChannel2, 254);
    
        break;
      }
      default:
      {
        Serial.print("LED Off: ");
        Serial.println(sidePeripheralName);
        sidePeripheralEnabled = false;

        setLedValue(sideLedChannel1, 0);
        setLedValue(sideLedChannel2, 0);
                
        break;
      }
    }
  }
}

//---   Cleanup .  ---//
static void CleanupSidePeripheral(unsigned long updateMillis) {
  setLedValue(sideLedChannel1, 0);
  setLedValue(sideLedChannel2, 0);
  sidePeripheralEnabled = false;
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
const int ventChannelGroup1 = 2;
const int ventChannelGroup2 = 3;
int ventUpdateDelay = 50;
int ventMinUpdateDelay = 50;
int ventMaxUpdateDelay = 150;
int ventRedBaseValue = 254;
int ventRedRandomValue = 0;
int ventGreenBaseValue = 0;
int ventGreenRandomValue = 80;
int ventBlueBaseValue = 0;

static void UpdateVentToggle(unsigned long updateMillis) {
  //If the LEDs have updated recently then return
  if (updateMillis - ventPreviousUpdateTime < ventUpdateDelay) {
    return;
  }

  ventPreviousUpdateTime = updateMillis;
  
  //Genereate new LED values
  int red = ventRedBaseValue;
  int green = ventGreenBaseValue + random(1, ventGreenRandomValue);
  int blue = ventBlueBaseValue;

  
  setRGB(ventChannelGroup1, red, green, blue);
  setRGB(ventChannelGroup2, red, green, blue);

  //Generate new update delay
  ventUpdateDelay = random(ventMinUpdateDelay, ventMaxUpdateDelay);   
}

static void DisableVentToggle(unsigned long updateMillis) {
  setRGB(ventChannelGroup1, 0, 0, 0);
  setRGB(ventChannelGroup2, 0, 0 ,0);
}

//--------------------//
//---   Actions    ---//
//--------------------//
char* ActionPeripheralName = "Actions";
uint16_t ActionPeripheralOrder = 6;
char* ActionPeripheralActionNames = "Horn|Titanic Feet|Ricochet|Explosion";

bool hornActionActive = false;
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
        titanicFeetActionActive = true;
        break;
      case 2:
        ricochetActionActive = true;
        break;
      case 3:
        explosionActionActive = true;
        break;    
    }
  }

  //Update all active actions
  if (hornActionActive == true) {
    hornActionActive = PerformHornAction(updateMillis);
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

}


//---   Horn   ---//
const uint8_t hornAudioTrack = 1;
static bool PerformHornAction(unsigned long updateMillis) {
  playTrack(hornAudioTrack);

  return false;
}

//---   Titanic Feet   ---//
const uint8_t titanicFeetAudioTrack = 3;

static bool PerformTitanicFeetAction(unsigned long updateMillis) {
  playTrack(titanicFeetAudioTrack);
  return false;
}

//---   Ricochet   ---//
const uint8_t ricochetAudioTrack = 2;
static bool PerformRicochetAction(unsigned long updateMillis) {
  playTrack(ricochetAudioTrack);

  return false;
}

//---   Explosion   ---//
const uint8_t explosionAudioTrack = 4;
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
    CleanupSidePeripheral(updateMillis);
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
  InitializeSidePeripheral();
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
      UpdateSidePeripheral(updateMillis);
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
char* randomActionOptionNames = "Horn|Titanic Feet|Ricochet";

bool randomActionsEnabled = false;

int randomActionNumRandomActions = 3;
bool hornActionEnabled = true;
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
        titanicFeetActionEnabled = !titanicFeetActionEnabled;
        break;
      case 2:
        ricochetActionEnabled = !ricochetActionEnabled;
        break;
    }
  }

  //If there is not ctive random index and enough time has elapsed then find a new index
  if (activeRandomActionIndex == -1 && updateMillis > randomActionNextActionTimeMS && randomActionsEnabled) {
    int numActiveActions = -1;
    if (hornActionEnabled == true) { numActiveActions += 1; }
    if (titanicFeetActionEnabled == true) { numActiveActions += 1; }
    if (ricochetActionEnabled == true) { numActiveActions += 1; }

    //Find the action index
    if (numActiveActions > -1) {
      int index = random(0, numActiveActions);

      if (hornActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 0; }
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
      actionActive = PerformTitanicFeetAction(updateMillis);
      break;
    case 2:
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
