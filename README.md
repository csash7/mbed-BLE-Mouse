# mbed-BLE-Mouse
### BLE Mouse library for Arduino boards with BLE support and mbed OS.
This library turns Arduino Board into a BLE Mouse which can be connected to devices such as Windows PC, android phones etc.
This library is completely written using Mbed OS's BLE API and also supports pairing with the Central device.

## Usage
This library is similar to the Arduino's Mouse library and supports all the functions which are in the mouse library.
It also has additional features such as setDeviceName, setManufacturerName and setBatteryLevel.
```
#include <HIDMouse.h>

HIDMouse bleMouse;

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  bleMouse.setDeviceName("Device");
  bleMouse.setManufacturerName("Manufacturer");
  bleMouse.setBatteryLevel(75);
  bleMouse.begin();
}

void loop() {
  while(bleMouse.isConnected()) {
  digitalWrite(LED_BUILTIN, HIGH);
  bleMouse.move(0,0,1);
  delay(1000);
  }
  
  digitalWrite(LED_BUILTIN, LOW);
 }
 ```
 
 ## Compatible Hardware
 - Arduino Nano BLE
 - Arduino Nano BLE Sense
 - Any board with Mbed OS and BLE support.
 
 ## Installation
### Using the Arduino IDE Library Manager

1. Choose `Tools` -> `Manage Libraries` -> `Mbed BLE Mouse`

### Using Git

```sh
cd ~/Documents/Arduino/libraries/
git clone https://github.com/csash7/mbed-BLE-Mouse
```

## Examples

See [examples](examples) folder.

## License

This library is [licensed](LICENSE-2.0.txt) under the [APACHE Licence](http://www.apache.org/licenses/LICENSE-2.0).
