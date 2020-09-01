#ifndef ONEWHEELMONITOR_UUID_H
#define ONEWHEELMONITOR_UUID_H

#include <BLEUUID.h>

namespace ow::UUID {

#define PREFIX "e659f3"
#define POSTFIX "-ea98-11e3-ac10-0800200c9a66"

static BLEUUID Service(                       PREFIX "00" POSTFIX);

static BLEUUID SerialNumberCharacteristic(    PREFIX "01" POSTFIX );
static BLEUUID BatteryRemainingCharacteristic(PREFIX "03" POSTFIX);
static BLEUUID TemperatureCharacteristic(     PREFIX "10" POSTFIX);
static BLEUUID FirmwareRevisionCharacteristic(PREFIX "11" POSTFIX);
static BLEUUID CurrentAmpsCharacteristic(     PREFIX "12" POSTFIX);
static BLEUUID BatteryTempCharacteristic(     PREFIX "15" POSTFIX);
static BLEUUID BatteryVoltageCharacteristic(  PREFIX "16" POSTFIX);
static BLEUUID HardwareRevisionCharacteristic(PREFIX "18" POSTFIX);
static BLEUUID CustomNameCharacteristic(      PREFIX "fd" POSTFIX);
static BLEUUID UartSerialReadCharacteristic(  PREFIX "fe" POSTFIX);
static BLEUUID UartSerialWriteCharacteristic( PREFIX "ff" POSTFIX);

}

#endif //ONEWHEELMONITOR_UUID_H
