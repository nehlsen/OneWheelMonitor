#ifndef ONEWHEELMONITOR_CHALLENGERESPONSE_H
#define ONEWHEELMONITOR_CHALLENGERESPONSE_H

#include "MD5.h"
#include <cstdint>

class ChallengeResponse
{
public:
    // we expect 20 characters as input
    // size of request/response Package
    const uint8_t PackageSize = 20;

    const uint8_t HeaderSize = 3;
    const uint8_t DataSize = 16;
    const uint8_t ChecksumSize = 1;

    explicit ChallengeResponse(MD5 *md5);
    virtual ~ChallengeResponse();

    uint8_t *getResponse() const;
    uint8_t getResponseSize() const;

    void setChallenge(uint8_t *challenge);
    void generateResponse();

protected:
    MD5 *m_md5;

    uint8_t *m_challengeRequest = nullptr;
    uint8_t *m_response = nullptr;

    uint8_t *extractChallenge();
    uint8_t *getSecretSalt();

    void resetResponse();
    void copyHeader();
    void setBody(uint8_t *data);
    void setChecksum();
    uint8_t calculateChecksum(uint8_t *data);
};

#endif //ONEWHEELMONITOR_CHALLENGERESPONSE_H
