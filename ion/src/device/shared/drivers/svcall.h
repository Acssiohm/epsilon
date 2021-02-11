#ifndef ION_DEVICE_SHARED_DRIVERS_SVCALL_H
#define ION_DEVICE_SHARED_DRIVERS_SVCALL_H

namespace Ion {
namespace Device {
namespace SVCall {

#define SVC_BACKLIGHT_BRIGHTNESS 0
#define SVC_BACKLIGHT_INIT 1
#define SVC_BACKLIGHT_IS_INITIALIZED 2
#define SVC_BACKLIGHT_SET_BRIGHTNESS 3
#define SVC_BACKLIGHT_SHUTDOWN 4
#define SVC_BATTERY_IS_CHARGING 5
#define SVC_BATTERY_LEVEL 6
#define SVC_BATTERY_VOLTAGE 7
#define SVC_CRC32_BYTE 8
#define SVC_CRC32_WORD 9
#define SVC_DISPLAY_COLORED_TILING_SIZE_10 10
#define SVC_DISPLAY_POST_PUSH_MULTICOLOR 11
#define SVC_DISPLAY_PULL_RECT 12
#define SVC_DISPLAY_PUSH_RECT 13
#define SVC_DISPLAY_PUSH_RECT_UNIFORM 14
#define SVC_DISPLAY_UNIFORM_TILING_SIZE_10 15
#define SVC_DISPLAY_WAIT_FOR_V_BLANK 16
#define SVC_EVENTS_COPY_TEXT 17
#define SVC_EVENTS_GET_EVENT 18
#define SVC_EVENTS_IS_DEFINED 19
#define SVC_EVENTS_REPETITION_FACTOR 20
#define SVC_EVENTS_SET_SHIFT_ALPHA_STATUS 21
#define SVC_EVENTS_SHIFT_ALPHA_STATUS 22
#define SVC_FCC_ID 23
#define SVC_KEYBOARD_SCAN 24
#define SVC_LED_GET_COLOR 25
#define SVC_LED_SET_BLINKING 26
#define SVC_LED_SET_COLOR 27
#define SVC_LED_UPDATE_COLOR_WITH_PLUG_AND_CHARGE 28
#define SVC_PERSISTING_BYTES_READ 29
#define SVC_PERSISTING_BYTES_WRITE 30
#define SVC_POWER_STANDBY 31
#define SVC_POWER_SUSPEND 32
#define SVC_RANDOM 33
#define SVC_RESET_CORE 34
#define SVC_SERIAL_NUMBER 35
#define SVC_TIMING_MILLIS 36
#define SVC_TIMING_MSLEEP 37
#define SVC_TIMING_USLEEP 38
#define SVC_USB_CLEAR_ENUMERATION_INTERRUPT 39
#define SVC_USB_DFU 40
#define SVC_USB_DISABLE 41
#define SVC_USB_ENABLE 42
#define SVC_USB_IS_ENUMERATED 43
#define SVC_USB_IS_PLUGGED 44

#define SVC_CIRCUIT_BREAKER_SET_HOME_CHECKPOINT 45
#define SVC_CIRCUIT_BREAKER_UNSET_CHECKPOINT 46
#define SVC_CIRCUIT_BREAKER_HAS_CHECKPOINT 47
#define SVC_CIRCUIT_BREAKER_LOAD_HOME_CHECKPOINT 48
#define SVC_CIRCUIT_BREAKER_SET_CUSTOM_CHECKPOINT 49
#define SVC_CIRCUIT_BREAKER_LOAD_CUSTOM_CHECKPOINT 50

}
}
}

#endif
