#ifndef Cerastus_h
#define Cerastus_h

#include <functional>
#include <ArduinoBLE.h>
#include "ToggleActionPeripheral.h"
#include "ToggleLedPeripheral.h"
#include "TogglePwmLedPeripheral.h"
#include "RgbLedPeripheral.h"
#include "SliderPeripheral.h"
#include "ActionPeripheral.h"
#include "PropertyCharacteristics.h"
#include "LedBoardManager.h"
#include "AudioManager.h"

//--------------------------//
//---   Audio Tracks     ---//
//--- 01: Horn           ---//
//--- 02: Bolt Gun       ---//
//--- 03: Titanic Feet   ---//
//--- 04: Ricochet       ---//
//--- 05: Explosion      ---//
//--------------------------//

//---   Horn   ---//
const uint8_t hornAudioTrack = 1;
static bool PlayHorn(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(hornAudioTrack);

  return false;
}

//---   Bolt Cannon .  ---//
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

static bool FireBoltCannons(unsigned long updateMillies) {
  if (!boltCannonFiring) {
    boltCannonFiring = true;
    boltCannonStartTime = updateMillies;
    boltCannonLastFireMillis = boltCannonStartTime - boltCannonFireInterval + boltCannonInitialDelay;

    AudioManager* audioManager = AudioManager::getInstance();
    audioManager->playTrack(boltCannonAudioTrack);
  }

  LedBoardManager* ledManager = LedBoardManager::getInstance();
  if (updateMillies - boltCannonLastFireMillis >= boltCannonFireInterval) {
    if (boltCannonLedState == false) { //Only update if the LED needs to be toggled
      boltCannonLastFireMillis = updateMillies;
      ledManager->setValue(boltCannonCurrentFirePin, 254);

      boltCannonLedState = true;
    } else {
      if (boltCannonLedState == true) {
        //Turn off LED after enough time has elapsed
        if (updateMillies - boltCannonLastFireMillis >= boltCannonLedOnTime) {
          ledManager->setValue(boltCannonCurrentFirePin, 0);

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
    if (updateMillies - boltCannonStartTime >= boltCannonFinalFireMillis) {

      //Turn off all pins
      for (int i = boltCannonStartPin; i <= boltCannonEndPin; i++) {
        ledManager->setValue(i, 0);
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

//---   TitanicFeet   ---//
const uint8_t titanicFeetAudioTrack = 3;

static bool FireTitanicFeet(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(titanicFeetAudioTrack);
  return false;
}

//---   Ricochet   ---//
const uint8_t ricochetAudioTrack = 4;
static bool PlayRicochet(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(ricochetAudioTrack);

  return false;
}

//---   Explosion   ---//
const uint8_t explosionAudioTrack = 5;
static bool PlayExplosion(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(explosionAudioTrack);

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

//---   Vent   ---//
static long ventPreviousUpdateTime = 0;
const int ventPin1 = 3;
const int ventPin2 = 4;
int ventUpdateDelay = 50;
int ventMinUpdateDelay = 50;
int ventMaxUpdateDelay = 150;
int ventBaseValue = 150;
int ventRandomValue = 80;

static void VentAction(unsigned long updateMillies) {

  //If the LEDs have updated recently then return
  if (updateMillies - ventPreviousUpdateTime < ventUpdateDelay) {
    return;
  }

  ventPreviousUpdateTime = updateMillies;
  
  //Genereate new LED values
  LedBoardManager* ledManager = LedBoardManager::getInstance();
  
  int ledValue = ventBaseValue + random(1, ventRandomValue);

  //Serial.print("Vent: ");
  //Serial.println(ledValue);

  ledManager->setValue(ventPin1, ledValue);
  ledManager->setValue(ventPin2, ledValue);

  //Generate new update delay
  ventUpdateDelay = random(ventMinUpdateDelay, ventMaxUpdateDelay);        
}

static void VentDisableAction(unsigned long updateMillies) {
  LedBoardManager* ledManager = LedBoardManager::getInstance();

  ledManager->setValue(ventPin1, 0);
  ledManager->setValue(ventPin2, 0);
}

char* ToggleActionNames = "Vents";
static std::vector<togFn> ToggleActions = {
  VentAction
};

static std::vector<togFn> ToggleDisableActions = {
  VentDisableAction
};

char* ActionNames = "Horn|Bolt Cannons|Titanic Feet|Ricochet|Explosion";

static std::vector<fn> Actions = { 
  PlayHorn,
  FireBoltCannons,
  PlayRicochet,
  FireTitanicFeet,
  PlayExplosion 
};

static std::vector<int> EyePins = { 0, 1, 2 };
static std::vector<int> BoltCannonPins = { 7, 8, 9, 10, 11};
static std::vector<int> VentPins = {3, 4};

TogglePwmLedPeripheral eyeLeds("Eyes", 1, EyePins, 254, "6641ed28-d551-430b-9222-407194f31bee");
TogglePwmLedPeripheral ventLeds("Vents", 2, VentPins, 254, "f0d54526-51c2-4796-a476-ccc6d5651ca5");
TogglePwmLedPeripheral boltCannonLeds("Bolt Cannons", 3, BoltCannonPins, 254, "5b83977a-d2b6-496a-b164-edf0949973a0");
SliderPeripheral volumeSlider("Volume", 4,  "", 50, 0, 100, 5, VolumeSliderAction, "b4daa255-bb66-4628-af14-ccb3600ddfd2");
ToggleActionPeripheral toggleActions("Toggles", 5, ToggleActionNames, ToggleActions, ToggleDisableActions, "a6b0ba67-ba48-481a-a288-ba40b48dc4b3");
ActionPeripheral actions("Actions", 6, ActionNames, Actions, "d47a3f8d-e29d-4964-a7f8-b870e99ca792");

TogglePwmLedPeripheral boltCannon1("Bolt Cannon 1", 7, 7, 254, "f6bea8d2-73f6-4e8e-bf57-82ca3bfe9fcb");
TogglePwmLedPeripheral boltCannon2("Bolt Cannon 2", 8, 8, 254, "5f382efa-eca3-4d10-bfef-0bf20fe0cb16");
TogglePwmLedPeripheral boltCannon3("Bolt Cannon 3", 9, 9, 254, "d076a194-55bf-4575-836f-fd1ac91f9f28");
TogglePwmLedPeripheral boltCannon4("Bolt Cannon 4", 10, 10, 254, "44de1617-b03d-4281-b176-36c90910f8b3");
TogglePwmLedPeripheral boltCannon5("Bolt Cannon 5", 11, 11, 254, "3a426bb1-6dd5-467c-8f61-5114a0bf9fb1");

static const int CerastusPeripheralsCount = 11;
static Peripheral* CerastusPeripherals[CerastusPeripheralsCount] = {
  &eyeLeds,
  &ventLeds,
  &boltCannonLeds,
  &volumeSlider,
  &toggleActions,
  &actions,
  &boltCannon1,
  &boltCannon2,
  &boltCannon3,
  &boltCannon4,
  &boltCannon5
};

#endif
