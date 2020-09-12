#ifndef ONEWHEELMONITOR_ONEWHEELVALUEREADER_H
#define ONEWHEELMONITOR_ONEWHEELVALUEREADER_H

#include <cstdint>

namespace ow
{

class OneWheelValueReader
{
public:
    virtual uint8_t getBatteryRemaining() = 0;
    virtual uint8_t getTemperature() = 0;
    virtual uint8_t getCurrentAmps() = 0;
    virtual uint8_t getBatteryTemp() = 0;
    virtual uint8_t getBatteryVoltage() = 0;
};

} // namespace ow

#endif //ONEWHEELMONITOR_ONEWHEELVALUEREADER_H
