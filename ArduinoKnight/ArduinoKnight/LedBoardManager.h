#ifndef LedBoardManager_h
#define LedBoardManager_h

#include <Arduino.h>
#include <Adafruit_TLC59711.h>

#define NUM_TLC59711  1
#define dataPin       2
#define clockPin      3

class LedBoardManager{
  private:
    Adafruit_TLC59711 _ledBoard; 
    static LedBoardManager*
         
    // static pointer which will points
    //to the instance of this class
    instancePtr;
   
    // Default constructor
    LedBoardManager() :
    _ledBoard(Adafruit_TLC59711(NUM_TLC59711, clockPin, dataPin))
    {
    }
   
  public:
   
  // deleting copy constructor
  LedBoardManager(const LedBoardManager& obj)
    = delete;
 
    static LedBoardManager* getInstance()
    {
      // If there is no instance of class
      // then we can create an instance.
      if (instancePtr == NULL)
      {
        // We can access private members
        // within the class.
        instancePtr = new LedBoardManager();
       
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
    _ledBoard.begin();
    _ledBoard.write();
  }
   
  // this function will right to led driver
  void setValue (int ledPin,int pwmVal) {
    int pwm = pwmVal * 257;
    int chan = ledPin;

    _ledBoard.setPWM(chan, pwm);
    _ledBoard.write();
  }

  // this function will write rgb values to led driver
  void setRGB(int rgbGroup, int redVal, int greenVal, int blueVal) {
    int red = redVal * 256;
    int green = greenVal * 256;
    int blue = blueVal * 256;
    const int rChannel = 3 * rgbGroup;

    _ledBoard.setLED(rgbGroup, red, green, blue);
    _ledBoard.write();
  }

  // this function will write rgb values to led driver
  void setRGB256(int rgbGroup, int redVal, int greenVal, int blueVal) {
    int red = redVal;
    int green = greenVal;
    int blue = blueVal;
    const int rChannel = 3 * rgbGroup;
  
    _ledBoard.setPWM(rChannel, red);
    _ledBoard.write();
    _ledBoard.setPWM(rChannel+1, green);
    _ledBoard.write();
    _ledBoard.setPWM(rChannel+2, blue);
    //_ledBoard.setLED(channel, red, green, blue);
    _ledBoard.write();
  }

  // this function will write a color to the led driver
  void setRGB(int channel, const char* color) {

    int number = (int) strtol( &color[0], NULL, 16);
    int r = number >> 16;
    int g = number >> 8 & 0xFF;
    int b = number & 0xFF;

    setRGB(channel, r, g, b);
  }
};

static long RGBtoHex(int red, int green, int blue) {
  return ((long)red << 16L) | ((long)green << 8L) | (long)blue;
}

#endif
