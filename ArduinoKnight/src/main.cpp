#include <Arduino.h>
#include "LedPeripheral.h"

// put function declarations here:
int myFunction(int, int);
 
LedPeripheral peripheral(4); 

void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(peripheral.ServiceId());
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}