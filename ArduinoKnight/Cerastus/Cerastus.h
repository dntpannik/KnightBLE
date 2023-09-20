#ifndef Cerastus_h
#define Cerastus_h

#include <functional>
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
//--- 02: Ricochet       ---//
//--- 03: Titanic Feet   ---//
//--- 04: Explosion      ---//
//--------------------------//

//---   Horn   ---//
const uint8_t hornAudioTrack = 1;
static bool PlayHorn(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(hornAudioTrack);

  return false;
}

//---   Ricochet   ---//
const uint8_t ricochetAudioTrack = 2;
static bool PlayRicochet(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(ricochetAudioTrack);

  return false;
}

//---   TitanicFeet   ---//
const uint8_t titanicFeetAudioTrack = 3;

static bool FireTitanicFeet(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(titanicFeetAudioTrack);
  return false;
}

//---   Explosion   ---//
const uint8_t explosionAudioTrack = 4;
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
const int ventRedChannel1 = 6;
const int ventRedChannel2 = 9;
int ventUpdateDelay = 50;
int ventMinUpdateDelay = 5;
int ventMaxUpdateDelay = 100;
int ventRedBaseValue = 135;
int ventRedRandomValue = 120;
int ventGreenBaseValue = 135;
int ventGreenRandomValue = 120;
int ventBlueBaseValue = 1;

static void VentAction(unsigned long updateMillies) {
  Serial.println("In Method");
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(titanicFeetAudioTrack);

  Serial.print("UpdateMillies");
  Serial.println(updateMillies);

  //If the LEDs have updated recently then return
  if (updateMillies - ventPreviousUpdateTime < ventUpdateDelay) {
    return;
  }

  ventPreviousUpdateTime = updateMillies;
  
  //Genereate new LED values
  LedBoardManager* ledManager = LedBoardManager::getInstance();

  int red = ventRedBaseValue + random(1, ventRedRandomValue);
  int green = ventGreenBaseValue + random(1, ventGreenRandomValue);
  int blue = ventBlueBaseValue;

  Serial.print("Color: (");
  Serial.print(red);
  Serial.print(", ");
  Serial.print(green);
  Serial.print(", ");
  Serial.print(blue);
  Serial.println(")");
  
  ledManager->setRGB256(ventRedChannel1, red, green, blue);
  ledManager->setRGB256(ventRedChannel2, red, green, blue);

  //Generate new update delay
  ventUpdateDelay = random(ventMinUpdateDelay, ventMaxUpdateDelay);        

  Serial.print("ventUpdateDelay: ");
  Serial.println(ventUpdateDelay);
}

static void VentDisableAction(unsigned long updateMillies) {
  LedBoardManager* ledManager = LedBoardManager::getInstance();

  ledManager->setRGB256(ventRedChannel1, 0, 0, 0);
  ledManager->setRGB256(ventRedChannel2, 0, 0, 0);
}

char* ToggleActionNames = "Vents";
static std::vector<togFn> ToggleActions = {
  VentAction
};

static std::vector<togFn> ToggleDisableActions = {
  VentDisableAction
};

char* ActionNames = "Horn|Ricochet||Titanic Feet|Explosion";

static std::vector<fn> Actions = { 
  PlayHorn,
  PlayRicochet,
  FireTitanicFeet,
  PlayExplosion 
};

static std::vector<int> EyeChannels = { 0, 1 };
static std::vector<int> ExhaustChannels = { 3, 4 };

TogglePwmLedPeripheral eyeLeds("Eyes", 1, EyeChannels, 254, "6641ed28-d551-430b-9222-407194f31bee");
TogglePwmLedPeripheral exhaustLeds("Exhaust", 2, ExhaustChannels, 254, "21b20b9e-7748-4765-9b0b-fec30e702965");
SliderPeripheral volumeSlider("Volume", 3,  "", 50, 0, 100, 5, VolumeSliderAction, "b4daa255-bb66-4628-af14-ccb3600ddfd2");
ToggleActionPeripheral toggleActions("Toggles", 4, ToggleActionNames, ToggleActions, ToggleDisableActions, "a6b0ba67-ba48-481a-a288-ba40b48dc4b3");
ActionPeripheral actions("Actions", 5, ActionNames, Actions, "d47a3f8d-e29d-4964-a7f8-b870e99ca792");

static const int CerastusPeripheralsCount = 5;
static Peripheral* CerastusPeripherals[CerastusPeripheralsCount] = {
  &eyeLeds,
  &exhaustLeds,
  &volumeSlider,
  &toggleActions,
  &actions
};

#endif
