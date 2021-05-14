#ifndef ION_DEVICE_KERNEL_KEYBOARD_H
#define ION_DEVICE_KERNEL_KEYBOARD_H

#include <ion/keyboard.h>
#include <drivers/config/keyboard.h>
#include <drivers/timing.h>

namespace Ion {
namespace Device {
namespace Keyboard {

using namespace Ion::Keyboard;

void init(bool activateInterruption = true);
void shutdown();
void initInterruptions();
void shutdownInterruptions();
void debounce();
void handleInterruption();

State scan();
State popState();

inline uint8_t rowForKey(Key key) {
  return (int)key/Config::numberOfColumns;
}
inline uint8_t columnForKey(Key key) {
  return (int)key%Config::numberOfColumns;
}

inline void setRowState(uint16_t rowState) {
  // TODO: Assert pin numbers are sequentials and dynamically find 9 and 0
  Config::RowGPIO.ODR()->setBitRange(8, 0, rowState);

  // TODO: 100 us seems to work, but wasn't really calculated
  Ion::Timing::usleep(100);
}

inline void activateRow(uint8_t row) {
  /* In open-drain mode, a 0 in the register drives the pin low, and a 1 lets
   * the pin floating (Hi-Z). So we want to set the current row to zero and all
   * the others to 1. */
  uint16_t rowState = ~(1 << static_cast<uint16_t>(Config::RowPins[row]));
  setRowState(rowState);
}

inline void activateAllRows() {
  setRowState(0);
}

inline bool columnIsActive(uint8_t column) {
  uint8_t pin = Config::ColumnPins[column];
  return !(Config::ColumnGPIO.IDR()->getBitRange(pin,pin));
}

}
}
}

#endif
