#ifndef ONEWHEELMONITOR_UUID_H
#define ONEWHEELMONITOR_UUID_H

#include <BLEUUID.h>

namespace ow::UUID {

static char BleDescriptorEnableNotificationValue[] = {0x01, 0x00};

static BLEUUID Service(                       "e659F300-ea98-11e3-ac10-0800200c9a66");
static BLEUUID ConfigDescriptor(              "00002902-0000-1000-8000-00805f9b34fb");

static BLEUUID SerialNumberCharacteristic(    "e659f301-ea98-11e3-ac10-0800200c9a66");
// ...
static BLEUUID BatteryRemainingCharacteristic("e659f303-ea98-11e3-ac10-0800200c9a66");
// ...
static BLEUUID FirmwareRevisionCharacteristic("e659f311-ea98-11e3-ac10-0800200c9a66");
// ...
static BLEUUID HardwareRevisionCharacteristic("e659f318-ea98-11e3-ac10-0800200c9a66");
// ...
static BLEUUID UartSerialReadCharacteristic(  "e659f3fe-ea98-11e3-ac10-0800200c9a66");
static BLEUUID UartSerialWriteCharacteristic( "e659f3ff-ea98-11e3-ac10-0800200c9a66");

}

#endif //ONEWHEELMONITOR_UUID_H
