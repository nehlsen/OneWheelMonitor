#ifndef ONEWHEELMONITOR_ONEWHEEL_H
#define ONEWHEELMONITOR_ONEWHEEL_H

#include <cstdint>
#include <BLERemoteCharacteristic.h>

class BLERemoteService;

namespace ow
{

// simple shell for BleRemoteService to access OW properties by name
class OneWheel
{
public:
    bool isReady();

    void setup(BLERemoteService *oneWheelRootService);
    void tearDown();

    uint8_t getBatteryRemaining();
    uint8_t getTemperature();
    uint8_t getCurrentAmps();
    uint8_t getBatteryTemp();
    uint8_t getBatteryVoltage();
    std::string getCustomName();

private:
    BLERemoteService *m_oneWheelRootService = nullptr;

    uint8_t getCharacteristicIntValue(BLEUUID &uuid);
    std::string getCharacteristicStringValue(BLEUUID &uuid);
    BLERemoteCharacteristic *getCharacteristic(BLEUUID uuid) const;

    static std::vector<BLEUUID> getCharacteristicsForNotifications();
    void setupNotifications();
    void deleteNotifications();
};

} // namespace ow

#endif //ONEWHEELMONITOR_ONEWHEEL_H
