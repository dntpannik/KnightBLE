#ifndef Peripheral_h
#define Peripheral_h

#include <ArduinoBLE.h>

class Peripheral {
    public:
        virtual void Initialize();
        virtual void Update();
};

#endif
