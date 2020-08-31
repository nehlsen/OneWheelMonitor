#include "OneWheel.h"
#include "UUID.h"
#include "util.h"
#include <BLERemoteService.h>

namespace ow
{

OneWheel::OneWheel(BLERemoteService *oneWheelRootService):
    m_oneWheelRootService(oneWheelRootService)
{
}

uint8_t OneWheel::getBatteryRemaining()
{
    auto batteryRemainingCharacteristic = getCharacteristic(UUID::BatteryRemainingCharacteristic);
    batteryRemainingCharacteristic->readValue();
    auto rawValue = batteryRemainingCharacteristic->readRawData();

    return twoByteHexToInt(rawValue);
}

BLERemoteCharacteristic *OneWheel::getCharacteristic(BLEUUID uuid) const
{
    return m_oneWheelRootService->getCharacteristic(uuid);
}

} // namespace ow
