#include "Connector.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <BLEDevice.h>

namespace ow
{

static const char *LOG_TAG = "OWConnector";
static TaskHandle_t connector_task_hdnl = nullptr;

static BLEUUID owServiceUUID("e659F300-ea98-11e3-ac10-0800200c9a66");
//a                           e659f300-ea98-11e3-ac10-0800200c9a66
static BLEUUID batteryCharacteristicUUID("e659f303-ea98-11e3-ac10-0800200c9a66");

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
//    xTaskCreatePinnedToCore(
//            &connector_task,
//            "connector_task",
//            4 * 1024,
//            this,
//            5,
//            &connector_task_hdnl,
//            0
//    );
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
        connectTo(m_scanResult);
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

bool Connector::connectTo(BLEAdvertisedDevice *device)
{
    ESP_LOGI(LOG_TAG, "OWConnector::connectTo(DEVICE): %s", device->getAddress().toString().c_str());

    BLEClient *client = BLEDevice::createClient(); // FIXME save client as member var
    m_clientCallbacks = new ClientCallbacks(this);
    client->setClientCallbacks(m_clientCallbacks);
    client->connect(device);

    return connectServiceAndCharacteristic(client);
}

bool Connector::connectTo(BLEAddress address)
{
    ESP_LOGI(LOG_TAG, "OWConnector::connectTo(ADDRESS): %s", address.toString().c_str());

    BLEClient *client = BLEDevice::createClient();
    m_clientCallbacks = new ClientCallbacks(this);
    client->setClientCallbacks(m_clientCallbacks);
    client->connect(address);

    return connectServiceAndCharacteristic(client);
}

bool Connector::connectServiceAndCharacteristic(BLEClient *client)
{
    BLERemoteService *remoteService = client->getService(owServiceUUID);
    if (remoteService == nullptr) {
        ESP_LOGE(LOG_TAG, "OWConnector::connectServiceAndCharacteristic: Failed to find service UUID: %s", owServiceUUID.toString().c_str());
        client->disconnect();
        return false;
    }
    ESP_LOGI(LOG_TAG, "OWConnector::connectServiceAndCharacteristic: found service");

    m_batteryCharacteristic = remoteService->getCharacteristic(batteryCharacteristicUUID);
    if (m_batteryCharacteristic == nullptr) {
        ESP_LOGE(LOG_TAG, "OWConnector::connectServiceAndCharacteristic: Failed to find characteristic UUID: %s", batteryCharacteristicUUID.toString().c_str());
        client->disconnect();
        return false;
    }
    ESP_LOGI(LOG_TAG, "OWConnector::connectServiceAndCharacteristic: found characteristic");

    // Read the value of the characteristic.
    if(m_batteryCharacteristic->canRead()) {
        std::string value = m_batteryCharacteristic->readValue();
        ESP_LOGI(LOG_TAG, "OWConnector::connectServiceAndCharacteristic: current characteristic value: \"%s\"", value.c_str());
    }

    if(m_batteryCharacteristic->canNotify()) {
//        m_batteryCharacteristic->registerForNotify(notifyCallback);
        m_batteryCharacteristic->registerForNotify(
                [](BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
                    ESP_LOGI(LOG_TAG, "OWConnector: battery characteristic value changed notifyier");
                    ESP_LOGI(LOG_TAG, "OWConnector: UUID:%s, length:%d, data:\"%s\"", pBLERemoteCharacteristic->getUUID().toString().c_str(), length, (char*)pData);
                });
    }

    m_isConnected = true;
    return true;
}

Connector::AdvertisedDeviceCallbacks::AdvertisedDeviceCallbacks(Connector *connector):
    m_connector(connector)
{}

void Connector::AdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
    ESP_LOGI(LOG_TAG, "AdvertisedDeviceCallbacks:onResult: %s", advertisedDevice.toString().c_str());

    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(owServiceUUID)) {
        ESP_LOGI(LOG_TAG, "AdvertisedDeviceCallbacks:onResult: device provides requested service! stopping scan...");
        m_connector->stopScan();
        m_connector->m_scanResult = new BLEAdvertisedDevice(advertisedDevice);
//        myDevice = new BLEAdvertisedDevice(advertisedDevice);
//        doConnect = true;
//        doScan = true;
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
