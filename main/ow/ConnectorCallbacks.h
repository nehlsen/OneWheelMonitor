#ifndef ONEWHEELMONITOR_CONNECTORCALLBACKS_H
#define ONEWHEELMONITOR_CONNECTORCALLBACKS_H

#include <BLEDevice.h>

namespace ow
{

class Connector;

class ConnectorAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
public:
    explicit ConnectorAdvertisedDeviceCallbacks(Connector *connector);

    void onResult(BLEAdvertisedDevice advertisedDevice) override;

private:
    Connector *m_connector;
};

class ConnectorClientCallbacks : public BLEClientCallbacks
{
public:
    explicit ConnectorClientCallbacks(Connector *connector);

    void onConnect(BLEClient *client) override;

    void onDisconnect(BLEClient *client) override;

private:
    Connector *m_connector;
};

}

#endif //ONEWHEELMONITOR_CONNECTORCALLBACKS_H
