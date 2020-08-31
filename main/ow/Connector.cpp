#include "Connector.h"
#include "UUID.h"
#include "Authenticator.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <BLEDevice.h>

namespace ow
{

static const char *LOG_TAG = "OWConnector";
static TaskHandle_t connector_task_hdnl = nullptr;

void connector_task(void *pvParameter)
{
    ESP_LOGI(LOG_TAG, "connector_task...");

    auto *connector = static_cast<Connector*>(pvParameter);
    ESP_ERROR_CHECK(nullptr == connector ? ESP_FAIL : ESP_OK);

    while (true) {
        connector->doWork();

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

Connector::Connector()
{
    BLEDevice::init("OWM");
}

void Connector::start()
{
    if (connector_task_hdnl) {
        ESP_LOGE(LOG_TAG, "already started");
    }

    ESP_LOGI(LOG_TAG, "starting OWConnector...");
    xTaskCreate(
            &connector_task,
            "connector_task",
            4 * 1024,
            this,
            5,
            &connector_task_hdnl
    );
}

void Connector::scanAndConnect()
{
    startScan();
}

void Connector::connect(BLEAddress address)
{
    connectTo(address);
}

void Connector::doWork()
{
    if (!m_isConnected && m_scanResult) {
        connectTo(m_scanResult->getAddress(), m_scanResult->getAddressType());
        delete m_scanResult;
        m_scanResult = nullptr;
    }
}

void Connector::startScan()
{
    ESP_LOGI(LOG_TAG, "OWConnector::startScan");

    if (m_advertisedDeviceCallbacks) {
        // assume we already started when the callback handler is already instantiated
        BLEDevice::getScan()->start(0);

        return;
    }

    m_advertisedDeviceCallbacks = new AdvertisedDeviceCallbacks(this);

    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(m_advertisedDeviceCallbacks);
    pBLEScan->setInterval(1349);
    pBLEScan->setWindow(449);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(5, false);
}

void Connector::stopScan()
{
    ESP_LOGI(LOG_TAG, "OWConnector::stopScan");

    BLEDevice::getScan()->stop();
}

bool Connector::connectTo(BLEAddress address, esp_ble_addr_type_t type)
{
    ESP_LOGI(LOG_TAG, "Connector::connectTo: %s", address.toString().c_str());

    BLEClient *client = BLEDevice::createClient();
    m_clientCallbacks = new ClientCallbacks(this);
    client->setClientCallbacks(m_clientCallbacks);
    client->connect(address, type);

    BLERemoteService *remoteService = client->getService(ow::UUID::Service);
    if (remoteService == nullptr) {
        ESP_LOGE(LOG_TAG, "OWConnector::connectServiceAndCharacteristic: Failed to find service UUID: %s", ow::UUID::Service.toString().c_str());
        client->disconnect();
        return false;
    }
    ESP_LOGI(LOG_TAG, "OWConnector::connectServiceAndCharacteristic: found service");

    m_authenticator = new Authenticator(remoteService);
    m_authenticator->startAuthentication();
    // found one wheel!
    //  - start authentication
    //  - once authenticated: m_oneWheel = new OnewWheel(remoteService)

    return true;
}

Connector::AdvertisedDeviceCallbacks::AdvertisedDeviceCallbacks(Connector *connector):
    m_connector(connector)
{}

void Connector::AdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
    ESP_LOGI(LOG_TAG, "AdvertisedDeviceCallbacks:onResult: %s", advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(ow::UUID::Service)) {
        ESP_LOGI(LOG_TAG, "AdvertisedDeviceCallbacks:onResult: device provides requested service! stopping scan...");
        m_connector->stopScan();
        m_connector->m_scanResult = new BLEAdvertisedDevice(advertisedDevice);
    }
}

Connector::ClientCallbacks::ClientCallbacks(Connector *connector):
    m_connector(connector)
{}

void Connector::ClientCallbacks::onConnect(BLEClient *client)
{
    ESP_LOGI(LOG_TAG, "ClientCallbacks:onConnect");
}

void Connector::ClientCallbacks::onDisconnect(BLEClient *client)
{
    ESP_LOGI(LOG_TAG, "ClientCallbacks:onDisconnect");

    m_connector->m_isConnected = false;
}

} // namespace ow
