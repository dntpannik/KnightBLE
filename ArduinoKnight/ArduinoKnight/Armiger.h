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
//--- 0: Horn          ---//
//--- 1: Chain Sword   ---//
//--- 2: Thermal Spear ---//
//--- 3: Bolter        ---//
//--- 4: AutoCannon    ---//
//------------------------//


//---   Horn   ---//
static bool PlayHorn(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager->playTrack(0);
  return false;
}

//---   Chain Sword   ---//
static bool PlayChainSword(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager.play(1);
  return false;
}

//---   Thermal Spear   ---//
static bool PlayThermalSpear(unsigned long updateMillies) {
  AudioManager* audioManager = AudioManager::getInstance();
  audioManager.play(2);
  return false;
}

//---   Bolter   ---//
const long bolterChannel = 0;
const long bolterFireInterval = 60;
const long bolterLastFireMillis = 0;
static bool FireBolter(unsigned long updateMillies) {
   LedBoardManager* ledManager = LedBoardManager::getInstance();
   
  if (updateMillies - bolterLastFireMillis >= bolterFireInterval) {
    bolterLastFireMillis = updateMillies
    manager->setValue(bolterChannel, 254);
    
  } else {
    manager->setValue(bolterChannel, 0);
    return false;
  }

  return true; //Still active
}

//---   Auto Cannons   ---//
const long autoCannonLeftChannel = 1;
const long autoCannonRightChannel = 2;
const long autoCannonHalfFireInterval = 30;
const long autoCannonLastFireMillis = 0;
const long autoCannonLastFireSide = 1; //1 for right, 0 for left
static bool FireAutoCannon(unsigned long updateMillies) {
   LedBoardManager* ledManager = LedBoardManager::getInstance();
   
  if (updateMillies - autoCannonHalfFireInterval >= bolterFireInterval) {
    autoCannonLastFireMillis = updateMillies

    if (autoCannonLastFireSide == 1) {
      manager->setValue(autoCannonLeftChannel, 254);
      autoCannonLastFireSide = 0
    } else {
      manager->setValue(autoCannonRightChannel, 254);
      autoCannonLastFireSide = 1
    }
    
  } else {
    manager->setValue(autoCannonLeftChannel, 0);
    manager->setValue(autoCannonRightChannel, 0);
    return false;
  }

  return true; //Still active
}

//---   Volume Slider   ---//
static void VolumeSliderAction(unsigned long updateMillies, uint16_t sliderValue, uint16_t sliderMinValue, uint16_t sliderMaxValue) {
  AudioManager* audioManager = AudioManager::getInstance();

  //Volume is a scale from 0-30
  uint8_t scaledVolume = sliderValue / (sliderMaxValue - sliderMinValue) * 30;
  
  audioManager->setVolume(scaledVolume);
}

char* ActionNames = "Horn|Bolter|Auto Cannons|Chain Sword|Thermal Spear";

static fn Actions[] = { PlayHorn, FireBolter, FireAutoCannons, PlayChainSword, PlayThermalSpear };

ToggleLedPeripheral led("Test LED", D6, "1cf09419-9b12-4536-bf18-e7e7a5a2fead");
TogglePwmLedPeripheral pwmLed("Pwm Led", 0, 254, "1cf09419-9b12-4536-bf18-e7e7a5a2fead");
TogglePwmLedPeripheral pwmLed2("LED2", 6, 254, "6641ed28-d551-430b-9222-407194f31bee");
RgbLedPeripheral rgbLed("RGB", 1, "6f32a8", "aa284a56-af19-4a4a-9930-c6ff40817e1e");
RgbLedPeripheral rgbLed2("RGB2", 3, "4fcdff", "ba38fe7a-463f-4336-96f2-36b814856d04");
SliderPeripheral volumeSlider("Volume", "", 50, 0, 100, 5, VolumeSliderAction, "b4daa255-bb66-4628-af14-ccb3600ddfd2");
ActionPeripheral actions("Actions", ActionNames, Actions, "d47a3f8d-e29d-4964-a7f8-b870e99ca792");

static const int ArmigerPeripheralsCount = 7;
static Peripheral* ArmigerPeripherals[QuestorisPeripheralsCount] = {
  &led,
  &pwmLed,
  &pwmLed2,
  &rgbLed,
  &rgbLed2,
  &volumeSlider,
  &actions
};

#endif
