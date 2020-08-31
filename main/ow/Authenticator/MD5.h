#ifndef ONEWHEELMONITOR_MD5_H
#define ONEWHEELMONITOR_MD5_H

class MD5
{
public:
    // unsigned char digest[16]
    virtual unsigned char* getDigest(unsigned char const *buf, unsigned len) = 0;
};

#endif //ONEWHEELMONITOR_MD5_H
