#include "UpdateListener.h"

void ow::UpdateListener::setOneWheelIsConnectedAndAuthenticated(bool isConnectedAndAuthenticated)
{
    m_isConnectedAndAuthenticated = isConnectedAndAuthenticated;
}

bool ow::UpdateListener::isConnectedAndAuthenticated() const
{
    return m_isConnectedAndAuthenticated;
}
