#ifndef ONEWHEELMONITOR_AUTHENTICATOR_H
#define ONEWHEELMONITOR_AUTHENTICATOR_H

#include <BLERemoteService.h>

namespace ow
{

class Authenticator
{
public:
    explicit Authenticator(BLERemoteService *oneWheelService);

    bool startAuthentication();

    void tryAuthenticated();

    bool isAuthenticated() const;

protected:
    BLERemoteService *m_oneWheelService;

    enum {
        NOT_STARTED                       = 0,
        FIRMWARE_REVISION_READ_REQUESTED  = 0x01,
        FIRMWARE_REVISION_READ_RECEIVED   = 0x02,
        ENABLE_NOTIFICATION_REQUESTED     = 0x04,
        ENABLE_NOTIFICATION_RECEIVED      = 0x08,
        FIRMWARE_REVISION_WRITE_REQUESTED = 0x10,
        FIRMWARE_REVISION_WRITE_RECEIVED  = 0x20,
    } m_authenticationState;

    void sendSecret() {}
};

} // namespace ow

#endif //ONEWHEELMONITOR_AUTHENTICATOR_H
