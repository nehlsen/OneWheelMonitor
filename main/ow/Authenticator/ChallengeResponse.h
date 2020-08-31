#ifndef ONEWHEELMONITOR_CHALLENGERESPONSE_H
#define ONEWHEELMONITOR_CHALLENGERESPONSE_H

#include "MD5.h"
#include <cstdint>

class ChallengeResponse
{
public:
    static const uint8_t HeaderSize = 3;
    static const uint8_t DataSize = 16;
    static const uint8_t ChecksumSize = 1;
    static const uint8_t PackageSize = HeaderSize + DataSize + ChecksumSize;

    explicit ChallengeResponse(MD5 *md5);
    virtual ~ChallengeResponse();

    uint8_t *getResponse() const;

    void setChallenge(uint8_t *challenge);
    bool generateResponse();

    void reset();

protected:
    MD5 *m_md5;

    uint8_t *m_challengeRequest = nullptr;
    uint8_t *m_response = nullptr;

    uint8_t *extractChallenge();
    uint8_t *getSecretSalt() const;

    void resetResponse();
    void copyHeader();
    void setBody(uint8_t *data);
    void setChecksum();
    uint8_t calculateChecksum(const uint8_t *data) const;
};

#endif //ONEWHEELMONITOR_CHALLENGERESPONSE_H
