#include "CerastusAcheron.h"
#include "ArduinoKnight.h"

char* name = "Storm of Wrath";
bool verboseDebuggingEnabled = false;

int peripheralCount = CerastusPeripheralsCount;
Peripheral** peripheralArray = CerastusPeripherals;

void setup() {
  RunSetup(name, verboseDebuggingEnabled, peripheralCount, peripheralArray);
}

void loop() {
  RunLoop();
}
