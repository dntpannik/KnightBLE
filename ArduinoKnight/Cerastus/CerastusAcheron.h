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
//--- 02: Flamer         ---//
//--- 03: Chain Swored   ---//
//--- 04: Titanic Feet   ---//
//--- 05: Ricochet       ---//
//--- 06: Explosion      ---//
//--------------------------//

//---   Horn   ---//
const uint8_t hornAudioTrack = 1;
static bool PlayHorn(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(hornAudioTrack);

  return false;
}

//---   Flamer   ---//
const uint8_t flamerAudioTrack = 2;
static bool PlayFlamer(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(flamerAudioTrack);

  return false;
}

//---   ChainSword   ---//
const uint8_t chainSwordAudioTrack = 3;
static bool PlayChainSword(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(chainSwordAudioTrack);

  return false;
}

//---   TitanicFeet   ---//
const uint8_t titanicFeetAudioTrack = 4;

static bool FireTitanicFeet(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(titanicFeetAudioTrack);
  return false;
}

//---   Ricochet   ---//
const uint8_t ricochetAudioTrack = 5;
static bool PlayRicochet(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(ricochetAudioTrack);

  return false;
}

//---   Explosion   ---//
const uint8_t explosionAudioTrack = 6;
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
const int ventPin1 = 10;
const int ventPin2 = 11;
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

//---   Flames   ---//
static long flamePreviousUpdateTime = 0;
const int flamePin1 = 6;
const int flamePin2 = 7;
const int flamePin3 = 8;
const int flamePin4 = 9;
int flameUpdateDelay = 50;
int flameMinUpdateDelay = 50;
int flameMaxUpdateDelay = 150;
int flameBaseValue = 220;
int flameRandomValue = 30;

static void FlameAction(unsigned long updateMillies) {

  //If the LEDs have updated recently then return
  if (updateMillies - flamePreviousUpdateTime < flameUpdateDelay) {
    return;
  }

  flamePreviousUpdateTime = updateMillies;
  
  //Genereate new LED values
  LedBoardManager* ledManager = LedBoardManager::getInstance();
  
  int ledValue = flameBaseValue + random(1, flameRandomValue);

  ledManager->setValue(flamePin1, ledValue);
  ledManager->setValue(flamePin2, ledValue);
  ledManager->setValue(flamePin3, ledValue);
  ledManager->setValue(flamePin4, ledValue);


  //Generate new update delay
  flameUpdateDelay = random(flameMinUpdateDelay, flameMaxUpdateDelay);        
}

static void FlameDisableAction(unsigned long updateMillies) {
  LedBoardManager* ledManager = LedBoardManager::getInstance();

  ledManager->setValue(flamePin1, 0);
  ledManager->setValue(flamePin2, 0);
  ledManager->setValue(flamePin3, 0);
  ledManager->setValue(flamePin4, 0);
}

char* ToggleActionNames = "Flames|Vents";
static std::vector<togFn> ToggleActions = {
  FlameAction,
  VentAction
};

static std::vector<togFn> ToggleDisableActions = {
  FlameDisableAction,
  VentDisableAction
};

char* ActionNames = "Horn|Flamer|Chain Sowrd|Titanic Feet|Ricochet|Explosion";

static std::vector<fn> Actions = { 
  PlayHorn,
  PlayFlamer,
  PlayChainSword,
  PlayRicochet,
  FireTitanicFeet,
  PlayExplosion 
};

static std::vector<int> EyeChannels = { 0, 1, 2 };

TogglePwmLedPeripheral eyeLeds("Eyes", 1, EyeChannels, 254, "6641ed28-d551-430b-9222-407194f31bee");
SliderPeripheral volumeSlider("Volume", 3,  "", 50, 0, 100, 5, VolumeSliderAction, "b4daa255-bb66-4628-af14-ccb3600ddfd2");
ToggleActionPeripheral toggleActions("Toggles", 4, ToggleActionNames, ToggleActions, ToggleDisableActions, "a6b0ba67-ba48-481a-a288-ba40b48dc4b3");
ActionPeripheral actions("Actions", 5, ActionNames, Actions, "d47a3f8d-e29d-4964-a7f8-b870e99ca792");

static const int CerastusPeripheralsCount = 4;
static Peripheral* CerastusPeripherals[CerastusPeripheralsCount] = {
  &eyeLeds,
  &volumeSlider,
  &toggleActions,
  &actions
};

#endif
