#include "util.h"

int twoByteHexToInt(uint8_t *data)
{
    int result = (int)(data[0]<<8);
    result += (int)(data[1]);

    return result;
}
