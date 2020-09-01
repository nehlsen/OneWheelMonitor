#include "Connector.h"
#include "UUID.h"
#include "Authenticator.h"
#include <esp_log.h>
#include <BLEDevice.h>
#include "ConnectorCallbacks.h"

namespace ow
{

static const char *LOG_TAG = "ow::Connector";

Connector::Connector()
{
    BLEDevice::init("OWM");
}

void Connector::scanAndConnect()
{
    startScan();
}

void Connector::connect(BLEAddress address)
{
    connectTo(address);
}

void Connector::maintainConnection()
{
    if (m_isConnected && m_authenticator) {
        m_authenticator->maintainAuthentication();
    }
    if (isConnected()) {
        return;
    }

    if (!m_isConnected && m_scanResult) {
        connectTo(m_scanResult->getAddress(), m_scanResult->getAddressType());
        delete m_scanResult;
        m_scanResult = nullptr;
    }

    // TODO retry...
    //   if not connected, retry periodically
    //   if not auth'd, retry periodically
    //   re-scan ?!
    //   more ?!
}

bool Connector::isConnected() const
{
    return m_isConnected && m_authenticator && m_authenticator->isAuthenticated();
}

void Connector::startScan()
{
    ESP_LOGI(LOG_TAG, "startScan");

    if (m_advertisedDeviceCallbacks) {
        // assume we already started when the callback handler is already instantiated
        BLEDevice::getScan()->start(0);

        return;
    }

    m_advertisedDeviceCallbacks = new ConnectorAdvertisedDeviceCallbacks(this);

    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(m_advertisedDeviceCallbacks);
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, false);
}

void Connector::stopScan()
{
    ESP_LOGI(LOG_TAG, "stopScan");

    BLEDevice::getScan()->stop();
}

bool Connector::connectTo(BLEAddress address, esp_ble_addr_type_t type)
{
    ESP_LOGI(LOG_TAG, "connectTo: %s", address.toString().c_str());

    if (m_client == nullptr) {
        m_client = BLEDevice::createClient();
        auto *clientCallbacks = new ConnectorClientCallbacks(this);
        m_client->setClientCallbacks(clientCallbacks);
    }

    if (m_client->connect(address, type)) {
        ESP_LOGI(LOG_TAG, "Connection established");
        m_isConnected = true;
    } else {
        ESP_LOGE(LOG_TAG, "Failed to connect");
        return false;
    }

    BLERemoteService *owRootService = getRootService();
    if (owRootService == nullptr) {
        ESP_LOGE(LOG_TAG, "Failed to find service UUID: %s", ow::UUID::Service.toString().c_str());
        m_client->disconnect();
        return false;
    }
    ESP_LOGI(LOG_TAG, "found desired service");

    if (m_authenticator == nullptr) {
        m_authenticator = new Authenticator(owRootService);
    }

    return m_authenticator->authenticate();
}

BLERemoteService *Connector::getRootService()
{
    return m_client->getService(ow::UUID::Service);
}

} // namespace ow
