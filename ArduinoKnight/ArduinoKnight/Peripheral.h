#ifndef Peripheral_h
#define Peripheral_h

#include <ArduinoBLE.h>

class Peripheral {
    public:
        virtual void Initialize();
        virtual bool Update();
        virtual void Cleanup();
        virtual void DebugOut();
};

#endif
