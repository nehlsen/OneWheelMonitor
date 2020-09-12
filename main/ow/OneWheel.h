#ifndef ONEWHEELMONITOR_ONEWHEEL_H
#define ONEWHEELMONITOR_ONEWHEEL_H

#include <BLERemoteCharacteristic.h>
#include "OneWheelValueReader.h"

class BLERemoteService;

namespace ow
{

// simple shell for BleRemoteService to access OW properties by name
class OneWheel : public OneWheelValueReader
{
public:
    bool isReady();

    void setup(BLERemoteService *oneWheelRootService);
    void tearDown();

    uint8_t getBatteryRemaining() override;
    uint8_t getTemperature() override;
    uint8_t getCurrentAmps() override;
    uint8_t getBatteryTemp() override;
    uint8_t getBatteryVoltage() override;
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
