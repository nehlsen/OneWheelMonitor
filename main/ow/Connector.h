#ifndef ONEWHEELMONITOR_CONNECTOR_H
#define ONEWHEELMONITOR_CONNECTOR_H

#include <BLEAdvertisedDevice.h>

namespace ow
{

class Authenticator;

// a) scan and connect to first discovered OW
// b) connect to statically configured OW
class Connector
{
public:
    Connector();

    void start();

    void scanAndConnect();
    void connect(BLEAddress address);

    // handler for task loop
    void doWork();

protected:
    friend class AdvertisedDeviceCallbacks;

    class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
    {
    public:
        explicit AdvertisedDeviceCallbacks(Connector *connector);

        void onResult(BLEAdvertisedDevice advertisedDevice) override;

    private:
        Connector *m_connector;
    };
    AdvertisedDeviceCallbacks *m_advertisedDeviceCallbacks = nullptr;

    void startScan();
    void stopScan();

    BLEAdvertisedDevice *m_scanResult = nullptr;

    class ClientCallbacks : public BLEClientCallbacks
    {
    public:
        explicit ClientCallbacks(Connector *connector);

        void onConnect(BLEClient *client) override;

        void onDisconnect(BLEClient *client) override;

    private:
        Connector *m_connector;
    };
    ClientCallbacks *m_clientCallbacks = nullptr;

    bool connectTo(BLEAddress address, esp_ble_addr_type_t type = BLE_ADDR_TYPE_PUBLIC);
    bool m_isConnected = false;

    Authenticator *m_authenticator = nullptr;
};

} // namespace ow

#endif //ONEWHEELMONITOR_CONNECTOR_H
