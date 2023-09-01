#ifndef Armiger_h
#define Armiger_h

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
//--- 5: AutoCannon    ---//
//------------------------//


//---   Horn   ---//
static bool PlayHorn(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(1);
  return false;
}

//---   Chain Sword   ---//
static bool PlayChainSword(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(2);
  return false;
}

//---   Thermal Spear   ---//
static bool PlayThermalSpear(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(3);
  return false;
}

//---   Bolter   ---//
static bool PlayBolter(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(4);
}

//---   Auto Cannons   ---//
static bool PlayAutoCannons(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(5);
}

//---   Volume Slider   ---//
static void VolumeSliderAction(unsigned long updateMillies, uint16_t sliderValue, uint16_t sliderMinValue, uint16_t sliderMaxValue) {
  AudioManager* audioManager = AudioManager::getInstance();

  //Volume is a scale from 0-30
  uint8_t scaledVolume = sliderValue / (sliderMaxValue - sliderMinValue) * 30;
  
  audioManager->setVolume(scaledVolume);
}

char* ActionNames = "Horn|Bolter|Auto Cannons|Chain Sword|Thermal Spear";
static std::vector<fn> Actions = { 
  PlayHorn, 
  PlayBolter, 
  PlayAutoCannons, 
  PlayChainSword, 
  PlayThermalSpear 
};

ToggleLedPeripheral eyeLeds("Eye LEDs", 0, D6, "1cf09419-9b12-4536-bf18-e7e7a5a2fead");
SliderPeripheral volumeSlider("Volume", 1, "", 50, 0, 100, 5, VolumeSliderAction, "b4daa255-bb66-4628-af14-ccb3600ddfd2");
ActionPeripheral actions("Actions", 2, ActionNames, Actions, "d47a3f8d-e29d-4964-a7f8-b870e99ca792");

static const int ArmigerPeripheralsCount = 3;
static Peripheral* ArmigerPeripherals[ArmigerPeripheralsCount] = {
  &eyeLeds,
  &volumeSlider,
  &actions
};

#endif
