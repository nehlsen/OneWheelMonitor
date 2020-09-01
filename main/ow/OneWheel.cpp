#include "OneWheel.h"
#include "UUID.h"
#include "util.h"
#include <BLERemoteService.h>
#include <esp_log.h>

#define LOG_TAG "OneWheel"

namespace ow
{

static std::map<uint16_t, std::string> characteristicStringValueCache;
static std::map<uint16_t, uint8_t> characteristicNumericValueCache;

static void characteristicChangedNotify(BLERemoteCharacteristic* characteristic, uint8_t* pData, size_t length, bool isNotify)
{
    ESP_LOGD(LOG_TAG, "Change notification, handle: %d", characteristic->getHandle());
    characteristicStringValueCache.erase(characteristic->getHandle());
    characteristicNumericValueCache.erase(characteristic->getHandle());
}

bool OneWheel::isReady()
{
    if (m_oneWheelRootService == nullptr) {
        return false;
    }

    return true;
}

void OneWheel::setup(BLERemoteService *oneWheelRootService)
{
    m_oneWheelRootService = oneWheelRootService;
    setupNotifications();
}

void OneWheel::tearDown()
{
    if(m_oneWheelRootService) deleteNotifications();
    characteristicStringValueCache.clear();
    characteristicNumericValueCache.clear();
    m_oneWheelRootService = nullptr;
}

uint8_t OneWheel::getBatteryRemaining()
{
    return getCharacteristicIntValue(UUID::BatteryRemainingCharacteristic);
}

uint8_t OneWheel::getTemperature()
{
    return getCharacteristicIntValue(UUID::TemperatureCharacteristic);
}

uint8_t OneWheel::getCurrentAmps()
{
    return getCharacteristicIntValue(UUID::CurrentAmpsCharacteristic);
}

uint8_t OneWheel::getBatteryTemp()
{
    return getCharacteristicIntValue(UUID::BatteryTempCharacteristic);
}

uint8_t OneWheel::getBatteryVoltage()
{
    return getCharacteristicIntValue(UUID::BatteryVoltageCharacteristic);
}

std::string OneWheel::getCustomName()
{
    return getCharacteristicStringValue(UUID::CustomNameCharacteristic);
}

uint8_t OneWheel::getCharacteristicIntValue(BLEUUID &uuid)
{
    auto characteristic = getCharacteristic(uuid);
    if (!characteristic) {
        ESP_LOGE(LOG_TAG, "Error: tried to access non-existent Characteristic with UUID '%s'", uuid.toString().c_str());
        return 0;
    }
    const uint16_t handle = characteristic->getHandle();

    uint8_t currentValue;
    if (characteristicNumericValueCache.count(handle) > 0) {
        currentValue = characteristicNumericValueCache.at(handle);
    } else {
        characteristic->readValue();
        currentValue = twoByteHexToUint8(characteristic->readRawData());
        characteristicNumericValueCache[handle] = currentValue;
    }

    return currentValue;
}

std::string OneWheel::getCharacteristicStringValue(BLEUUID &uuid)
{
    auto characteristic = getCharacteristic(uuid);
    if (!characteristic) {
        ESP_LOGE(LOG_TAG, "Error: tried to access non-existent Characteristic with UUID '%s'", uuid.toString().c_str());
        return "";
    }
    const uint16_t handle = characteristic->getHandle();

    std::string currentValue;
    if (characteristicStringValueCache.count(handle) > 0) {
        currentValue = characteristicStringValueCache.at(handle);
    } else {
        currentValue = characteristic->readValue();
        characteristicStringValueCache[handle] = currentValue;
    }

    return currentValue;
}

BLERemoteCharacteristic *OneWheel::getCharacteristic(BLEUUID uuid) const
{
    return m_oneWheelRootService->getCharacteristic(uuid);
}

void OneWheel::setupNotifications()
{
    for (BLEUUID uuid : getCharacteristicsForNotifications()) {
        auto characteristic = getCharacteristic(uuid);
        characteristic->registerForNotify(characteristicChangedNotify);
        ESP_LOGD(LOG_TAG, "registerForNotify, handle: %d, uuid: %s", characteristic->getHandle(), characteristic->getUUID().toString().c_str());
    }
}

void OneWheel::deleteNotifications()
{
    for (BLEUUID uuid : getCharacteristicsForNotifications()) {
        auto characteristic = getCharacteristic(uuid);
        characteristic->registerForNotify(nullptr);
    }
}

std::vector<BLEUUID> OneWheel::getCharacteristicsForNotifications()
{
    return {
        UUID::BatteryRemainingCharacteristic,
        UUID::TemperatureCharacteristic,
        UUID::CurrentAmpsCharacteristic,
        UUID::BatteryTempCharacteristic,
        UUID::BatteryVoltageCharacteristic,
    };
}

} // namespace ow
