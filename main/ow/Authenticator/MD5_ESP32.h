#ifndef ONEWHEELMONITOR_MD5_ESP32_H
#define ONEWHEELMONITOR_MD5_ESP32_H

#include "MD5.h"

class MD5_ESP32 : public MD5
{
public:
    unsigned char *getDigest(const unsigned char *buf, unsigned int len) override;
};

#endif //ONEWHEELMONITOR_MD5_ESP32_H
