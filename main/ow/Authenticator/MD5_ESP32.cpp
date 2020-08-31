#include <cstdlib>
#include "MD5_ESP32.h"
#include "esp32/rom/md5_hash.h"

unsigned char *MD5_ESP32::getDigest(const unsigned char *buf, unsigned int len)
{
    auto *digest = (unsigned char*)calloc(1, 16);


//    const unsigned int md5Size = 16; // FIXME or is this 32?
    struct MD5Context md5_ctx;
    MD5Init(&md5_ctx);
    MD5Update(&md5_ctx, buf, len);
//    unsigned char digest[md5Size+1];
    MD5Final(digest, &md5_ctx);


    return digest;
}
