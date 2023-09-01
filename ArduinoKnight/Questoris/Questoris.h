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

//---------------------------//
//---   Audio Tracks      ---//
//--- 01: Horn            ---//
//--- 03: Gattling Cannon ---//
//--- 05: Flamer          ---//
//--- 07: AutoCannon      ---//
//--- 09: Missles         ---//
//--- 11: Las Cannon      ---//
//--- 13: Melta           ---//
//--- 15: Multi Laser     ---//
//--- 17: Battle Cannon   ---//
//--- 19: ChainSword      ---//
//--- 21: Missle Pod      ---//
//--- 23: Thermal Cannon  ---//
//--- 25: Titanic Feed    ---//
//--- 27: Auto Cannon     ---//
//---------------------------//

//---   Horn   ---//
const uint8_t hornAudioTrack = 1;
static bool PlayHorn(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(hornAudioTrack);

  return false;
}

//---   AvengerGatlingCannon   ---//
static int gatlingLed = 10;
static int gatlingMotorPin = D4;
bool gatlingFiring = false;
long gatlingStartTime = 0;
bool gatlingLedState = false;
long gatlingLastFireMillis = 0; 
const long gatlingFinalFireMillis = 2760; 
const long gatlingInitialDelay = 100;
const long gatlingFireInterval = 500;
const uint8_t gatlingAudioTrack = 3;
static bool FireAvengerGatlingCannon(unsigned long updateMillies) {
  LedBoardManager* ledManager = LedBoardManager::getInstance();
  if (!gatlingFiring) {
     gatlingFiring = true;
     gatlingStartTime = updateMillies;
     gatlingLastFireMillis = gatlingStartTime - gatlingFireInterval + gatlingInitialDelay;
     digitalWrite(gatlingMotorPin, 1);  
     ledManager->setValue(gatlingLed, 254);
     AudioManager* audioManager = AudioManager::getInstance();
      audioManager->playTrack(gatlingAudioTrack);
  }
  
  if (updateMillies - gatlingStartTime >= gatlingFinalFireMillis) {
    ledManager->setValue(gatlingLed, 0);
    digitalWrite(gatlingMotorPin, 0);  
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
static int stubberPin = 0;
bool stubberFiring = false;
long stubberStartTime = 0;
bool stubberLedState = false;
long stubberLastFireMillis = 0; 
const long stubberFinalFireMillis = 2200; 
const long stubberInitialDelay = 200;
const long stubberFireInterval = 60;
const long stubberLedOnTime = 30;
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
      ledManager->setValue(stubberPin, 254);

      stubberLedState = true;
    }
    
  } else {
    if (stubberLedState == true) {
      //Turn off LED after enough time has elapsed
      if (updateMillies - stubberLastFireMillis >= stubberLedOnTime) {
        ledManager->setValue(stubberPin, 0);
        stubberLedState = false;
      }
    }
  }

  if (updateMillies - stubberStartTime >= stubberFinalFireMillis) {
    ledManager->setValue(stubberPin, 0);
    stubberFiring = false;
    stubberLedState = false;
    return false;
  }

  return true; //Stil active
}

//---   IronstormMisslePod   ---//
const uint8_t misslePodAudioTrack = 21;
static bool FireIronstormMisslePod(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(misslePodAudioTrack);

  return false;
}

//---   LasImpulsor   ---//
const uint8_t lasImpulsorAudioTrack = 11;
static bool FireLasImpulsor(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(lasImpulsorAudioTrack);
  return false;
}

//---   MeltaGun   ---//
const uint8_t meltaAudioTrack = 13;
static bool FireMeltaGun(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(meltaAudioTrack);
  return false;
}

//---   MultiLaser   ---//
const uint8_t multiLaserAudioTrack = 15;
static bool FireMultiLaser(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(multiLaserAudioTrack);
  return false;
}

//---   RapidFireBattleCannon   ---//
static int battleCannonLed = D5;
bool battleCannonFiring = false;
long battleCannonStartTime = 0;
bool battleCannonLedState = false;
long battleCannonLastFireMillis = 0; 
const long battleCannonFinalFireMillis = 3200; 
const long battleCannonInitialDelay = 500;
const long battleCannonFireInterval = 1300;
const long battleCannonLedOnTime = 100;
const uint8_t battleCannonAudioTrack = 17;
static bool FireRapidFireBattleCannon(unsigned long updateMillies) {
if (!battleCannonFiring) {
     battleCannonFiring = true;
     battleCannonStartTime = updateMillies;
     battleCannonLastFireMillis = battleCannonStartTime - battleCannonFireInterval + battleCannonInitialDelay;

     AudioManager* audioManager = AudioManager::getInstance();
      audioManager->playTrack(battleCannonAudioTrack);
  }

  LedBoardManager* ledManager = LedBoardManager::getInstance();
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
const uint8_t battleStubberAudioTrack = 7;

static bool FireBattleCannonHeavyStubber(unsigned long updateMillies) {
  if (!battleStubberFiring) {
     battleStubberFiring = true;
     battleStubberStartTime = updateMillies;
     battleStubberLastFireMillis = battleStubberStartTime - battleStubberFireInterval + battleStubberInitialDelay;

     AudioManager* audioManager = AudioManager::getInstance();
      audioManager->playTrack(battleStubberAudioTrack);
  }

  LedBoardManager* ledManager = LedBoardManager::getInstance();
  if (updateMillies - battleStubberLastFireMillis >= battleStubberFireInterval) {
    if (battleStubberLedState == false) { //Only update if the LED needs to be toggled
      battleStubberLastFireMillis = updateMillies;
      ledManager->setValue(battleStubberPin, 254);

      battleStubberLedState = true;
    }
    
  } else {
    if (battleStubberLedState == true) {
      //Turn off LED after enough time has elapsed
      if (updateMillies - battleStubberLastFireMillis >= battleStubberLedOnTime) {
        ledManager->setValue(battleStubberPin, 0);
        battleStubberLedState = false;
      }
    }
  }

  if (updateMillies - battleStubberStartTime >= battleStubberFinalFireMillis) {
    ledManager->setValue(battleStubberPin, 0);
    battleStubberFiring = false;
    battleStubberLedState = false;
    return false;
  }

  return true; //Stil active
}

//---   ReaperChainsword   ---//
const uint8_t chainswordAudioTrack = 19;

static bool FireReaperChainsword(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(chainswordAudioTrack);
  return false;
}

//---   StormspearRocketPod   ---//
const uint8_t rocketPodAudioTrack = 9;

static bool FireStormspearRocketPod(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(rocketPodAudioTrack);
  return false;
}

//---   ThermalCannon   ---//
const uint8_t thermalCannonAudioTrack = 23;

static bool FireThermalCannon(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(thermalCannonAudioTrack);
  return false;
}

//---   TitanicFeet   ---//
const uint8_t titanicFeetAudioTrack = 25;

static bool FireTitanicFeet(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(titanicFeetAudioTrack);
  return false;
}

//---   TwinIcarusAutocannon   ---//
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
const uint8_t twinIcAudioTrack = 3;
static bool FireTwinIcarusAutocannon(unsigned long updateMillies) {
 if (!twinIcFiring) {
     twinIcFiring = true;
     twinIcPrevFiredSide = false;
     twinIcStartTime = updateMillies;
     twinIcLastFireMillis = twinIcStartTime - twinIcFireInterval + twinIcInitialDelay;

     AudioManager* audioManager = AudioManager::getInstance();
      audioManager->playTrack(twinIcAudioTrack);
  }

  LedBoardManager* ledManager = LedBoardManager::getInstance();
  if (updateMillies - twinIcLastFireMillis >= twinIcFireInterval) {
    if (twinIcLedState == false) { //Only update if the LED needs to be toggled
      twinIcLastFireMillis = updateMillies;

      if (twinIcPrevFiredSide == true) {
        ledManager->setValue(twinIcLeftLed, 254);
        twinIcPrevFiredSide = false;
      } else {
        ledManager->setValue(twinIcRightLed, 254);
        twinIcPrevFiredSide = true;
      }
      
      twinIcLedState = true;
    }
    
  } else {
    if (twinIcLedState == true) {
      //Turn off LED after enough time has elapsed
      if (updateMillies - twinIcLastFireMillis >= twinIcLedOnTime) {
        ledManager->setValue(twinIcLeftLed, 0);
        ledManager->setValue(twinIcRightLed, 0);
        twinIcLedState = false;
      }
    }
  }

  if (updateMillies - twinIcStartTime >= twinIcFinalFireMillis) {
    ledManager->setValue(twinIcLeftLed, 0);
    ledManager->setValue(twinIcRightLed, 0);
    twinIcFiring = false;
    twinIcLedState = false;
    return false;
  }

  return true; //Stil active
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

char* ActionNames = "Horn|Avenger Gatling Cannon|Heavy Flamer|Heavy Stubber|Battle Cannon Stubber|Ironstorm Missle Pod|Las-Impulsor|MeltaGun|Multi-Laser|Rapid Fire Battle Cannon|Reaper Chainsword|Stormspear Rocket Pod|Thermal Cannon|Titanic Feet|Twin Icarus Autocannon";

static std::vector<fn> Actions = { 
  PlayHorn,
  FireAvengerGatlingCannon,
  FireHeavyFlamer,
  FireHeavyStubber,
  FireBattleCannonHeavyStubber,
  FireIronstormMisslePod,
  FireLasImpulsor,
  FireMeltaGun,
  FireMultiLaser,
  FireRapidFireBattleCannon,
  FireReaperChainsword,
  FireStormspearRocketPod,
  FireThermalCannon,
  FireTitanicFeet,
  FireTwinIcarusAutocannon 
};

static std::vector<int> ExhaustLeds = { 1, 2, 3, 4 };
static std::vector<int> EyeLeds = { 6, 7 };

TogglePwmLedPeripheral eyeLeds("Eyes", 1, EyeLeds, 254, "6641ed28-d551-430b-9222-407194f31bee");
TogglePwmLedPeripheral exhaustLeds("Exhaust", 2, ExhaustLeds, 254, "21b20b9e-7748-4765-9b0b-fec30e702965");
SliderPeripheral volumeSlider("Volume", 3,  "", 50, 0, 100, 5, VolumeSliderAction, "b4daa255-bb66-4628-af14-ccb3600ddfd2");
ActionPeripheral actions("Actions", 4, ActionNames, Actions, "d47a3f8d-e29d-4964-a7f8-b870e99ca792");

static const int QuestorisPeripheralsCount = 4;
static Peripheral* QuestorisPeripherals[QuestorisPeripheralsCount] = {
  &eyeLeds,
  &exhaustLeds,
  &volumeSlider,
  &actions
};

#endif
