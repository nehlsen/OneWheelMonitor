#ifndef ONEWHEELMONITOR_UPDATELISTENER_H
#define ONEWHEELMONITOR_UPDATELISTENER_H

#include "OneWheelValueReader.h"

namespace ow
{

class UpdateListener
{
public:
    void setOneWheelIsConnectedAndAuthenticated(bool isConnectedAndAuthenticated);

    virtual void setValues(OneWheelValueReader *reader) = 0;

protected:
    bool isConnectedAndAuthenticated() const;

private:
    bool m_isConnectedAndAuthenticated = false;
};

}

#endif //ONEWHEELMONITOR_UPDATELISTENER_H
