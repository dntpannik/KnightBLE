#include "CerastusCastigator.h"
#include "ArduinoKnight.h"

char* name = "Hammer of Terra";
bool verboseDebuggingEnabled = false;

int peripheralCount = CerastusPeripheralsCount;
Peripheral** peripheralArray = CerastusPeripherals;

void setup() {
  RunSetup(name, verboseDebuggingEnabled, peripheralCount, peripheralArray);
}

void loop() {
  RunLoop();
}
