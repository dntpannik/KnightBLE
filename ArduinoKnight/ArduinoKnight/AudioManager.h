#ifndef AudioManager_h
#define AudioManager_h

#include <DFRobotDFPlayerMini.h>

class AudioManager{
  private:
    DFRobotDFPlayerMini _dfPlayer;
    static AudioManager*
         
    // static pointer which will points
    //to the instance of this class
    instancePtr;
   
    // Default constructor
    AudioManager() :
    _dfPlayer()
    {
    }
   
  public:
   
  // deleting copy constructor
  AudioManager(const AudioManager& obj)
    = delete;
 
    static AudioManager* getInstance()
    {
      // If there is no instance of class
      // then we can create an instance.
      if (instancePtr == NULL)
      {
        // We can access private members
        // within the class.
        instancePtr = new AudioManager();
       
        // returning the instance pointer
        return instancePtr;
      }
      else
      {
        // if instancePtr != NULL that means
        // the class already have an instance.
        // So, we are returning that instance
        // and not creating new one.
        return instancePtr;
      }
  }
 
  void Initialize() {
    if (!_dfPlayer.begin(Serial1)) {
      Serial.println(F("Unable to begin:"));
      Serial.println(F("1.Please recheck the connection!"));
      Serial.println(F("2.Please insert the SD card!"));
      //while(true);
    }
    Serial.println(F("DFPlayer Mini online."));
  }

  //Sets the volume of the player. Max value is 30
  void setVolume(uint8_t vol) {
    if (vol > 30) {
      vol = 30;
    }

    _dfPlayer.volume(vol);
  }

  //Plays a track number
  void playTrack(uint8_t trackNum) {
    _dfPlayer.play(trackNum);
  }
};

#endif
