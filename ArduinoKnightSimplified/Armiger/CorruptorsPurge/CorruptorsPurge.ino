#include <ArduinoBLE.h>
#include <Adafruit_TLC59711.h>
#include <DFRobotDFPlayerMini.h>

char* modelName = "Corruptors Purge";


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
int eyePin = 6;

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

  pinMode(eyePin, OUTPUT);
  digitalWrite(eyePin, 0);
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

        digitalWrite(eyePin, 1);
    
        break;
      }
      default:
      {
        Serial.print("LED Off: ");
        Serial.println(eyePeripheralName);
        eyePeripheralEnabled = false;

        digitalWrite(eyePin, 0);
                
        break;
      }
    }
  }
}

//---   Cleanup .  ---//
static void CleanupEyePeripheral(unsigned long updateMillis) {
  digitalWrite(eyePin, 0);
  eyePeripheralEnabled = false;
}

//--------------------//
//---   Actions    ---//
//--------------------//
char* ActionPeripheralName = "Actions";
uint16_t ActionPeripheralOrder = 6;
char* ActionPeripheralActionNames = "Horn|Auto Cannons|Chain Sword|Thermal Spear|Bolter|Ricochet|Explosion";

bool hornActionActive = false;
bool bolterActionActive = false;
bool autoCannonsActionActive = false;
bool chainSwordActionActive = false;
bool thermalSpearActionActive = false;
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
        autoCannonsActionActive = true;
        break;
      case 2:
        chainSwordActionActive = true;
        break;
      case 3:
        thermalSpearActionActive = true;
        break;  
      case 4:
        bolterActionActive = true;
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

  if (bolterActionActive == true) {
    bolterActionActive = PerformBolterAction(updateMillis);
  }

  if (autoCannonsActionActive == true) {
    autoCannonsActionActive = PerformAutoCannonAction(updateMillis);
  }

  if (chainSwordActionActive == true) {
    chainSwordActionActive = PerformChainSwordAction(updateMillis);
  }

  if (thermalSpearActionActive == true) {
    thermalSpearActionActive = PerformThermalSpearAction(updateMillis);
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

//---   AutoCannon   ---//
const uint8_t autoCannonAudioTrack = 2;
static bool PerformAutoCannonAction(unsigned long updateMillis) {
  playTrack(autoCannonAudioTrack);

  return false;
}


//---   Chain Sword   ---//
const uint8_t chainSwordAudioTrack = 3;
static bool PerformChainSwordAction(unsigned long updateMillis) {
  playTrack(chainSwordAudioTrack);

  return false;
}

//---   Thermal Spear   ---//
const uint8_t thermalSpearAudioTrack = 4;
static bool PerformThermalSpearAction(unsigned long updateMillis) {
  playTrack(thermalSpearAudioTrack);

  return false;
}

//---   Bolter   ---//
const uint8_t bolterAudioTrack = 5;
static bool PerformBolterAction(unsigned long updateMillis) {
  playTrack(bolterAudioTrack);

  return false;
}

//---   Ricochet   ---//
const uint8_t ricochetAudioTrack = 6;
static bool PerformRicochetAction(unsigned long updateMillis) {
  playTrack(ricochetAudioTrack);

  return false;
}

//---   Explosion   ---//
const uint8_t explosionAudioTrack = 7;
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
char* randomActionOptionNames = "Horn|Auto Cannons|Chain Sword|Thermal Spear|Bolter|Ricochet";

bool randomActionsEnabled = false;

int randomActionNumRandomActions = 6;
bool hornRandomActionEnabled = true;
bool autoCannonsRandomActionEnabled = true;
bool chainSwordRandomActionEnabled = true;
bool thermalSpearRandomActionEnabled = true;
bool bolterRandomActionEnabled = true;
bool ricochetRandomActionEnabled = true;

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

    Serial.print("randomActionsEnabled: ");
    Serial.println(randomActionsEnabled);

    Serial.print("updateMillis: ");
    Serial.println(updateMillis);

    Serial.print("randomActionNextActionTimeMS: ");
    Serial.println(randomActionNextActionTimeMS);
  }

  //Check for options changing
  if (randomActionPeripheralTriggerCharacteristic.written()) {
    uint16_t actionValue;
    randomActionPeripheralTriggerCharacteristic.readValue(actionValue);

    //Flip active status
    switch(actionValue) {
      case 0:
        hornRandomActionEnabled = !hornRandomActionEnabled;
        break;
      case 1:
        autoCannonsRandomActionEnabled = !autoCannonsRandomActionEnabled;
        break;
      case 2:
        chainSwordRandomActionEnabled = !chainSwordRandomActionEnabled;
        break;
      case 3:
        thermalSpearRandomActionEnabled = !thermalSpearRandomActionEnabled;
        break;
      case 4:
        bolterRandomActionEnabled = !bolterRandomActionEnabled;
        break;
      case 5:
        ricochetRandomActionEnabled = !ricochetRandomActionEnabled;
        break;
    }
  }

  //If there is not ctive random index and enough time has elapsed then find a new index
  if (activeRandomActionIndex == -1 && updateMillis > randomActionNextActionTimeMS && randomActionsEnabled) {
    int numActiveActions = -1;
    if (hornRandomActionEnabled == true) { numActiveActions += 1; }
    if (autoCannonsRandomActionEnabled == true) { numActiveActions += 1; }
    if (chainSwordRandomActionEnabled == true) { numActiveActions += 1; }
    if (thermalSpearRandomActionEnabled == true) { numActiveActions += 1; }
    if (bolterRandomActionEnabled == true) { numActiveActions += 1; }
    if (ricochetRandomActionEnabled == true) { numActiveActions += 1; }

    //Find the action index
    if (numActiveActions > -1) {
      int index = random(0, numActiveActions);

      if (hornRandomActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 0; }
        index -= 1;
      }
      if (autoCannonsRandomActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 1; }
        index -= 1;
      }
      if (chainSwordRandomActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 3; }
        index -= 1;
      }
      if (thermalSpearRandomActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }  
      if (bolterRandomActionEnabled == true) {
        if (index == 0) { activeRandomActionIndex = 4; }
        index -= 1;
      }  
      if (ricochetRandomActionEnabled == true) {
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
      actionActive = PerformChainSwordAction(updateMillis);
      break;
    case 3:
      actionActive = PerformThermalSpearAction(updateMillis);
      break; 
    case 4:
      actionActive = PerformBolterAction(updateMillis);
      break;  
    case 5:
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
