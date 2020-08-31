#ifndef ONEWHEELMONITOR_CHALLENGERESPONSE_H
#define ONEWHEELMONITOR_CHALLENGERESPONSE_H

#include "MD5.h"
#include <cstdint>

class ChallengeResponse
{
public:
    // we expect 20 characters as input
    const int ChallengeSize = 20;

    explicit ChallengeResponse(MD5 *md5);
    virtual ~ChallengeResponse();

    uint8_t *getResponse() const;
    uint8_t getResponseSize() const;

    void setChallenge(uint8_t *challenge);
    void generateResponse();

protected:
    MD5 *m_md5;

    uint8_t *m_challenge = nullptr;
    uint8_t *m_response = nullptr;

    void resetResponse();
    void copyHeader();
};

#endif //ONEWHEELMONITOR_CHALLENGERESPONSE_H
