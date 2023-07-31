#ifndef Questoris_h
#define Questoris_h

#include <functional>
#include "ToggleLedPeripheral.h"
#include "TogglePwmLedPeripheral.h"
#include "RgbLedPeripheral.h"
#include "SliderPeripheral.h"
#include "ActionPeripheral.h"
#include "PropertyCharacteristics.h"
#include "LedBoardManager.h"
#include "AudioManager.h"

//------------------------//
//---   Audio Tracks   ---//
//--- 1: Horn          ---//
//--- 2: Chain Sword   ---//
//--- 3: Thermal Spear ---//
//--- 4: Bolter        ---//
//--- 15: AutoCannon    ---//
//------------------------//

//---   Horn   ---//
const uint8_t hornAudioTrack = 1;
static bool PlayHorn(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(hornAudioTrack);

  return false;
}

//---   AvengerGatlingCannon   ---//
bool gatlingFiring = false;
long gatlingStartTime = 0;
bool gatlingLedState = false;
long gatlingLastFireMillis = 0; 
const long gatlingFinalFireMillis = 2760; 
const long gatlingChannel = 0;
const long gatlingInitialDelay = 100;
const long gatlingFireInterval = 500;
const uint8_t gatlingAudioTrack = 3;
static bool FireAvengerGatlingCannon(unsigned long updateMillies) {
  if (!gatlingFiring) {
     gatlingFiring = true;
     gatlingStartTime = updateMillies;
     gatlingLastFireMillis = gatlingStartTime - gatlingFireInterval + gatlingInitialDelay;

     AudioManager* audioManager = AudioManager::getInstance();
      audioManager->playTrack(gatlingAudioTrack);
  }

  LedBoardManager* ledManager = LedBoardManager::getInstance();
  if (updateMillies - gatlingLastFireMillis >= gatlingFireInterval) {
    if (gatlingLedState == false) { //Only update if the LED needs to be toggled
      gatlingLastFireMillis = updateMillies;
      ledManager->setValue(gatlingChannel, 254);

      gatlingLedState = true;
    }
    
  } else {
    if (gatlingLedState == true) {
      ledManager->setValue(gatlingChannel, 0);
      gatlingLedState = false;
    }
  }

  if (updateMillies - gatlingStartTime >= gatlingFinalFireMillis) {
    ledManager->setValue(gatlingChannel, 0);
    gatlingFiring = false;
    gatlingLedState = false;
    return false;
  }

  return true; //Stil active
}

//---   HeavyFlamer   ---//
static bool FireHeavyFlamer(unsigned long updateMillies) {
    AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(5);

  return false;
  return false;
}

//---   HeavyStubber   ---//
bool stubberFiring = false;
long stubberStartTime = 0;
bool stubberLedState = false;
long stubberLastFireMillis = 0; 
const long stubberFinalFireMillis = 2200; 
const long stubberChannel = 0;
const long stubberInitialDelay = 100;
const long stubberFireInterval = 60;
const uint8_t stubberAudioTrack = 7;


static bool FireHeavyStubber(unsigned long updateMillies) {
  if (!stubberFiring) {
     stubberFiring = true;
     stubberStartTime = updateMillies;
     stubberLastFireMillis = stubberStartTime - stubberFireInterval + stubberInitialDelay;

     AudioManager* audioManager = AudioManager::getInstance();
      audioManager->playTrack(stubberAudioTrack);
  }

  LedBoardManager* ledManager = LedBoardManager::getInstance();
  if (updateMillies - stubberLastFireMillis >= stubberFireInterval) {
    if (stubberLedState == false) { //Only update if the LED needs to be toggled
      stubberLastFireMillis = updateMillies;
      ledManager->setValue(gatlingChannel, 254);

      stubberLedState = true;
    }
    
  } else {
    if (stubberLedState == true) {
      ledManager->setValue(stubberChannel, 0);
      stubberLedState = false;
    }
  }

  if (updateMillies - stubberStartTime >= stubberFinalFireMillis) {
    ledManager->setValue(stubberChannel, 0);
    stubberFiring = false;
    stubberLedState = false;
    return false;
  }

  return true; //Stil active
}

//---   IronstormMisslePod   ---//
static bool FireIronstormMisslePod(unsigned long updateMillies) {
  Serial.println("5");
    AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(5);

  return false;
}

//---   LasImpulsor   ---//
static bool FireLasImpulsor(unsigned long updateMillies) {
    Serial.println("6");
    AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(6);
  return false;
}

//---   MeltaGun   ---//
static bool FireMeltaGun(unsigned long updateMillies) {
  Serial.println("Firing Meltagun");
  return false;
}

//---   MultiLaser   ---//
static bool FireMultiLaser(unsigned long updateMillies) {
  Serial.println("Firing Multi-Laser");
  return false;
}

//---   RapidFireBattleCannon   ---//
static bool FireRapidFireBattleCannon(unsigned long updateMillies) {
  Serial.println("Firing Rapid Fire Battle Cannon");
  return false;
}

//---   ReaperChainsword   ---//
static bool FireReaperChainsword(unsigned long updateMillies) {
  Serial.println("Firing Reaper Chainsword");
  return false;
}

//---   StormspearRocketPod   ---//
static bool FireStormspearRocketPod(unsigned long updateMillies) {
  Serial.println("Firing Stormspear Rocket Pod");
  return false;
}

//---   ThermalCannon   ---//
static bool FireThermalCannon(unsigned long updateMillies) {
  Serial.println("Firing Thermal Cannon");
  return false;
}

//---   TitanicFeet   ---//
static bool FireTitanicFeet(unsigned long updateMillies) {
  Serial.println("Firing Titanic Feet");
  return false;
}

//---   TwinIcarusAutocannon   ---//
static bool FireTwinIcarusAutocannon(unsigned long updateMillies) {
  Serial.println("Firing Twin Icarus Autocannon");
  return false;
}

//---   Volume Slider   ---//
static void VolumeSliderAction(unsigned long updateMillies, uint16_t sliderValue, uint16_t sliderMinValue, uint16_t sliderMaxValue) {
  AudioManager* audioManager = AudioManager::getInstance();

  Serial.print("Max Value: ");
  Serial.println(sliderMaxValue);

  Serial.print("Min Value: ");
  Serial.println(sliderMinValue);

  Serial.print("Value: ");
  Serial.println(sliderValue);  

  //Volume is a scale from 0-30
  uint8_t scaledVolume = (sliderValue * 30) / (sliderMaxValue - sliderMinValue);
  Serial.print("Setting volume to: ");
  Serial.println(scaledVolume);
  audioManager->setVolume(scaledVolume);
}

char* ActionNames = "Horn|Avenger Gatling Cannon|Heavy Flamer|Heavy Stubber|Ironstorm Missle Pod|Las-Impulsor|MeltaGun|Multi-Laser|Rapid Fire Battle Cannon|Reaper Chainsword|Stormspear Rocket Pod|Thermal Cannon|Titanic FeetTwin Icarus Autocannon";

static std::vector<fn> Actions = { 
  PlayHorn,
  FireAvengerGatlingCannon,
  FireHeavyFlamer,
  FireHeavyStubber,
  FireIronstormMisslePod,
  FireLasImpulsor,
  FireMeltaGun,
  FireMultiLaser,
  FireRapidFireBattleCannon,
  FireReaperChainsword,
  FireStormspearRocketPod,
  FireThermalCannon,
  FireTitanicFeet,
  FireTwinIcarusAutocannon };

ToggleLedPeripheral led("Test LED", D6, "1cf09419-9b12-4536-bf18-e7e7a5a2fead");
TogglePwmLedPeripheral pwmLed("Pwm Led", 0, 254, "1cf09419-9b12-4536-bf18-e7e7a5a2fead");
TogglePwmLedPeripheral pwmLed2("LED2", 6, 254, "6641ed28-d551-430b-9222-407194f31bee");
RgbLedPeripheral rgbLed("RGB", 1, "6f32a8", "aa284a56-af19-4a4a-9930-c6ff40817e1e");
RgbLedPeripheral rgbLed2("RGB2", 3, "4fcdff", "ba38fe7a-463f-4336-96f2-36b814856d04");
SliderPeripheral volumeSlider("Volume", "", 50, 0, 100, 5, VolumeSliderAction, "b4daa255-bb66-4628-af14-ccb3600ddfd2");
ActionPeripheral actions("Actions", ActionNames, Actions, "d47a3f8d-e29d-4964-a7f8-b870e99ca792");

static const int QuestorisPeripheralsCount = 7;
static Peripheral* QuestorisPeripherals[QuestorisPeripheralsCount] = {
  &led,
  &pwmLed,
  &pwmLed2,
  &rgbLed,
  &rgbLed2,
  &volumeSlider,
  &actions
};

#endif
