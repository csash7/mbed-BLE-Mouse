#include "mouse.h"

uint8_t report[] = { 0, 0, 0, 0 };

BLEMouse::BLEMouse(BLE &ble) : HIDServiceBase(ble,
                       MOUSE_REPORT_MAP, 
                       sizeof(MOUSE_REPORT_MAP),
                       report,
                       NULL,
                       NULL,
                       sizeof(inputReport)),
_button(0)
{
}

void BLEMouse::click(uint8_t b) {
  this->press(b);
  this->release(b);
}

void BLEMouse::move(signed char x, signed char y, signed char wheel) {
  unsigned char mouseMove[4]= { 0x00, 0x00, 0x00, 0x00 };

  // send key code
  mouseMove[0] = this->_button;
  mouseMove[1] = x;
  mouseMove[2] = y;
  mouseMove[3] = wheel;

  this->send(mouseMove);
}

void BLEMouse::press(uint8_t b) {
  this->_button |= b;

  this->move(0, 0, 0);
}

void BLEMouse::release(uint8_t b) {
  this->_button &= ~b;

  this->move(0, 0, 0);
}

bool BLEMouse::isPressed(uint8_t b) {
 return ((this->_button & b) != 0);
}

