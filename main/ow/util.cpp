#include "util.h"

int twoByteHexToInt(uint8_t *data)
{
    int result = (int)(data[0]<<8);
    result += (int)(data[1]);

    return result;
}

template<typename int_t>
int_t twoByteHexToIntX(uint8_t *data)
{
    int_t result = (int_t)(data[0]<<8);
    result += (int_t)(data[1]);

    return result;
}

int16_t twoByteHexToInt16(uint8_t *data)
{
    return twoByteHexToIntX<int16_t>(data);
}

uint16_t twoByteHexToUint16(uint8_t *data)
{
    return twoByteHexToIntX<uint16_t>(data);
}

uint8_t twoByteHexToUint8(uint8_t *data)
{
    return twoByteHexToIntX<uint8_t>(data);
}
