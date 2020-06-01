/*
Boards: Arduino Nano BLE, Arduino Nano BLE Sense (Any boards with Mbed OS and BLE support)
This example turns the board into a BLE mouse
whose mouse movements is controlled using IMU sensor onbaord.
*/
#include <HIDMouse.h>
#include <Arduino_LSM9DS1.h>

HIDMouse bleMouse;

void setup() {

  if (!IMU.begin()) {
    while (1);
  }

  pinMode(LED_BUILTIN, OUTPUT);
  bleMouse.setDeviceName("Device");
  bleMouse.setManufacturerName("Manufacturer");
  bleMouse.setBatteryLevel(75);
  bleMouse.begin();

}

void loop() {
  float ax, ay, az;
  float prevxValue = 0, prevyValue = 0;
  int xAcc = 0, yAcc = 0, maxPosAcc = 5, maxNegAcc = -5;

  while(bleMouse.isConnected()) {

    digitalWrite(LED_BUILTIN, HIGH);

  if (IMU.accelerationAvailable  ()) {
    IMU.readAcceleration(ax, ay, az);

    if(ax < -0.3){
      if(prevxValue < -0.3){
        if(yAcc < maxPosAcc){
        yAcc += 1;
        }
      }
      else {
        prevxValue = ax;
        yAcc = 1;
        }
      bleMouse.move(0,yAcc); // y-Axis down
      }
    else if(ax > 0.3){
      if(prevxValue > 0.3){
        if(yAcc > maxNegAcc){
        yAcc -= 1;
        }
      }
      else {
        prevxValue = ax;
        yAcc = -1;
        }
      bleMouse.move(0,yAcc); // y-axis up
      }
    if(ay < -0.3){
      if(prevyValue < -0.3){
        if(xAcc > maxNegAcc){
        xAcc -= 1;
        }
      }
      else {
        prevyValue = ay;
        xAcc = -1;
        }
      bleMouse.move(xAcc,0); // x-axis right
      }
    else if(ay > 0.3){
      if(prevyValue > 0.3){
        if(xAcc < maxPosAcc){
        xAcc += 1;
        }
      }
      else {
        prevyValue = ay;
        xAcc = 1;
        }
      bleMouse.move(xAcc,0); // x-axis left
      }  
  }
  //delay(100);
  
  }
  digitalWrite(LED_BUILTIN, LOW);
}
