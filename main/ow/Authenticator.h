#ifndef ONEWHEELMONITOR_AUTHENTICATOR_H
#define ONEWHEELMONITOR_AUTHENTICATOR_H

#include <BLERemoteService.h>

namespace ow
{

/**
 * OneWheel Authentication basically works like this
 * 1) register for notify on UartSerialReadCharacteristic
 * 2) read firmware version, write back to start key exchange
 * 3) generate key and write to UartSerialWriteCharacteristic
 */
class Authenticator
{
public:
    explicit Authenticator(BLERemoteService *oneWheelService);

    bool isAuthenticated() const;
    bool startAuthentication();

protected:
    BLERemoteService *m_oneWheelService;

    enum {
        NOT_STARTED = 0,
        FIRMWARE_CHECK_SUCCEED = 1<<1,
        FIRMWARE_CHECK_FAILED = 1<<2,
        NOTIFICATIONS_ENABLED = 1<<3,
        FIRMWARE_WRITE_BACK = 1<<4,
        CHALLENGE_RESPONSE_WRITTEN = 1<<5,
        CHALLENGE_RESPONSE_FAILED_TO_GENERATE = 1<<6,
        TRY_AUTH_FAILED = 1<<7,
        AUTHENTICATION_COMPLETE = 1<<8,
    } m_authenticationState;

    void enableSerialReadNotify();
    void disableSerialReadNotify();

    bool requestChallenge();
    void sendChallengeResponse();

    bool tryAuthenticated();
};

} // namespace ow

#endif //ONEWHEELMONITOR_AUTHENTICATOR_H
