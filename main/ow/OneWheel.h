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
    OneWheel();

    bool isReady();

    void setup(BLERemoteService *oneWheelRootService);
    void tearDown();

//    xxx_t getSerialNumber() const;
    uint8_t getBatteryRemaining();
// get...
// ...
    uint16_t getFirmwareRevision() const;
    uint16_t getHardwareRevision() const;
// ...

private:
    BLERemoteService *m_oneWheelRootService;

    BLERemoteCharacteristic *getCharacteristic(BLEUUID uuid) const;
};

} // namespace ow

#endif //ONEWHEELMONITOR_ONEWHEEL_H
