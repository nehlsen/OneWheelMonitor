#ifndef ONEWHEELMONITOR_UTIL_H
#define ONEWHEELMONITOR_UTIL_H

#include <cstdint>

[[deprecated("use twoByteHexToIntX instead")]]
int twoByteHexToInt(uint8_t *data);

int16_t twoByteHexToInt16(uint8_t *data);
uint16_t twoByteHexToUint16(uint8_t *data);
uint8_t twoByteHexToUint8(uint8_t *data);

//template<typename int_t>
//int_t twoByteHexToIntX(uint8_t *data);

#endif //ONEWHEELMONITOR_UTIL_H
