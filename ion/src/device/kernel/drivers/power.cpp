#include <kernel/drivers/power.h>
#include <drivers/reset.h>
#include <ion/usb.h>
#include <kernel/drivers/authentication.h>
#include <kernel/drivers/battery.h>
#include <kernel/drivers/board.h>
#include <kernel/drivers/events.h>
#include <kernel/drivers/keyboard.h>
#include <kernel/drivers/keyboard_queue.h>
#include <kernel/drivers/led.h>
#include <regs/regs.h>
#include <regs/config/pwr.h>
#include <regs/config/rcc.h>
#include <shared/drivers/usb_privileged.h>

namespace Ion {
namespace Device {
namespace Power {

using namespace Regs;

void suspend(bool checkIfOnOffKeyReleased) {
  bool isLEDActive = LED::getColor() != KDColorBlack;
  bool plugged = Ion::USB::isPlugged();
  bool numworksAuthentication = Authentication::trustedUserland();

  if (checkIfOnOffKeyReleased) {
    waitUntilOnOffKeyReleased();
  }

  /* First, shutdown all peripherals except LED. Indeed, the charging pin state
   * might change when we shutdown peripherals that draw current. This also
   * shutdown all interruptions. This has to be done before shuting down the
   * external flash because the interruptions implementation might be located
   * in the external flash. */
  Board::shutdownPeripherals(true);

  while (1) {
    // Update LED color according to plug and charge state
    Battery::initGPIO();
    USB::initGPIO();
    LED::init(numworksAuthentication);
    isLEDActive = LED::updateColorWithPlugAndCharge() != KDColorBlack;

    // Configure low-power mode
    if (isLEDActive) {
      Power::sleepConfiguration();
    } else {
      Power::stopConfiguration();
    }

    // Shutdown all peripherals (except LED if active)
    Board::shutdownPeripherals(isLEDActive);

    /* Wake up on:
     * - Power key
     * - Plug/Unplug USB
     * - Stop charging */
    Power::configWakeUp();

    Power::internalFlashSuspend(isLEDActive);

    // Check power key
    Keyboard::init(false); // Don't activate interruptions which would override wake-up configurations
    Keyboard::State scan = Keyboard::scan();
    Keyboard::State OnlyOnOffKeyDown = Keyboard::State(Keyboard::Key::OnOff);

    // Check plugging state
    USB::initGPIO();
    if (scan == OnlyOnOffKeyDown || (!plugged && Ion::USB::isPlugged())) {
      // Wake up
      waitUntilOnOffKeyReleased();
      break;
    } else {
      /* The wake up event can be an unplug event or a battery charging event.
       * In both cases, we want to update static observed states like
       * sLastUSBPlugged or sLastBatteryCharging. */
      Events::getPlatformEvent();
    }
    plugged = Ion::USB::isPlugged();
  }

  // Reset normal frequency
  Board::setStandardFrequency(Board::Frequency::High);
  Board::initPeripherals(numworksAuthentication, false);
  // Update LED according to plug and charge state
  LED::updateColorWithPlugAndCharge();
  /* If the USB has been unplugged while sleeping, the USB should have been
   * soft disabled but as part of the USB peripheral was asleep, this could
   * not be done before. */
  if (Ion::USB::isPlugged()) {
    Ion::USB::disable();
  }
}

void waitUntilOnOffKeyReleased() {
  /* Wait until power is released to avoid restarting just after suspending */
  bool isPowerDown = true;
  while (isPowerDown) {
    Keyboard::State scan = Keyboard::scan();
    isPowerDown = scan.keyDown(Keyboard::Key::OnOff);
  }
  Ion::Timing::msleep(100);
  // Flush the keyboard queue to avoid handling artifacts state at wake-up
  Keyboard::Queue::sharedQueue()->flush();
}

void inline shutdownExternalFlashAndEnterLowPowerMode(bool keepLEDActive) {
  // Shutdown the external flash
  ExternalFlash::shutdown();
  // Shutdown all clocks (except the ones used by LED if active)
  Board::shutdownPeripheralsClocks(keepLEDActive);
  enterLowPowerMode();
}

void __attribute__((noinline)) internalFlashSuspend(bool isLEDActive) {
  shutdownExternalFlashAndEnterLowPowerMode(isLEDActive);
  /* A hardware event triggered a wake up, we determine if the device should
   * wake up. We wake up when:
   * - only the power key was down
   * - the unplugged device was plugged
   * - the battery stopped charging */
  CORTEX.SCR()->setSLEEPDEEP(false);
  if (!isLEDActive) {
    // When LED are off, the system STOPs which switchs off the PLL
    Board::initSystemClocks();
  }
  Board::initPeripheralsClocks();
  // Init external flash
  ExternalFlash::init();
}

void __attribute__((noinline)) internalFlashStandby() {
  shutdownExternalFlashAndEnterLowPowerMode(false);
  Reset::coreWhilePlugged();
}

void sleepConfiguration() {
  // Decrease HCLK frequency
  Board::setStandardFrequency(Device::Board::Frequency::Low);

#if REGS_PWR_CONFIG_ADDITIONAL_FIELDS
  // Disable over-drive
  PWR.CR()->setODSWEN(false);
  while(!PWR.CSR()->getODSWRDY()) {
  }
  PWR.CR()->setODEN(true);
#endif

  CORTEX.SCR()->setSLEEPDEEP(false);
}

void stopConfiguration() {
  PWR.CR()->setMRUDS(true); // Main regulator in Low Voltage and Flash memory in Deep Sleep mode when the device is in Stop mode
  PWR.CR()->setLPUDS(true); // Low-power regulator in under-drive mode if LPDS bit is set and Flash memory in power-down when the device is in Stop under-drive mode
  PWR.CR()->setLPDS(true); // Low-power Voltage regulator on. Takes longer to wake up.
  PWR.CR()->setFPDS(true); // Put the flash to sleep. Takes longer to wake up.

  CORTEX.SCR()->setSLEEPDEEP(true);
}

void enterLowPowerMode() {
  /* To enter sleep, we need to issue a WFE instruction, which waits for the
   * event flag to be set and then clears it. However, the event flag might
   * already be on. So the safest way to make sure we actually wait for a new
   * event is to force the event flag to on (SEV instruction), use a first WFE
   * to clear it, and then a second WFE to wait for a _new_ event. */
  asm("sev");
  asm("wfe");
  asm("nop");
  asm("wfe");
}

}
}
}
