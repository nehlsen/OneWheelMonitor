#include "ConnectorCallbacks.h"
#include "Connector.h"
#include <esp_log.h>
#include "UUID.h"

#define LOG_TAG "ConnectorCallbacks"

namespace ow
{

ConnectorAdvertisedDeviceCallbacks::ConnectorAdvertisedDeviceCallbacks(Connector *connector):
    m_connector(connector)
{}

void ConnectorAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
    ESP_LOGI(LOG_TAG, "AdvertisedDeviceCallbacks:onResult: %s", advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(ow::UUID::Service)) {
        ESP_LOGI(LOG_TAG, "AdvertisedDeviceCallbacks:onResult: device provides requested service! stopping scan...");
        m_connector->stopScan();
        m_connector->m_scanResult = new BLEAdvertisedDevice(advertisedDevice);
    }
}


ConnectorClientCallbacks::ConnectorClientCallbacks(Connector *connector) :
        m_connector(connector)
{}

void ConnectorClientCallbacks::onConnect(BLEClient *client)
{
    ESP_LOGI(LOG_TAG, "ClientCallbacks:onConnect");
}

void ConnectorClientCallbacks::onDisconnect(BLEClient *client)
{
    ESP_LOGI(LOG_TAG, "ClientCallbacks:onDisconnect");

    m_connector->m_isConnected = false;
}

}
