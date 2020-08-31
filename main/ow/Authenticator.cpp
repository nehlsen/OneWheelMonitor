#include "Authenticator.h"
#include "UUID.h"
#include <esp_log.h>
#include <BLERemoteService.h>
#include <BLERemoteCharacteristic.h>
#include <BLERemoteDescriptor.h>
#include <cstring>
#include "esp32/rom/md5_hash.h"

#include "Authenticator/InputBuffer.h"
#include "Authenticator/ChallengeResponse.h"
#include "Authenticator/MD5_ESP32.h"

namespace ow
{

static int authentication_steps = 4;
#define LOG_TAG "ow::Authenticator"

static InputBuffer *MyInputBuffer = new InputBuffer;
static ChallengeResponse *MyChallengeResponse = new ChallengeResponse(new MD5_ESP32);

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    ESP_LOGI(__FILE__, "Notify callback, received %d bytes", length);
//    ESP_LOGI(__FILE__, "Notify callback for characteristic %s", pBLERemoteCharacteristic->getUUID().toString().c_str());
//    ESP_LOGI(__FILE__, " of data length %d", length);
//    ESP_LOGI(__FILE__, "data: %s", (char*)pData);

    MyInputBuffer->append(pData, length);
    if (MyInputBuffer->isSaturated()) {
        MyChallengeResponse->setChallenge(MyInputBuffer->getBuffer());
        MyChallengeResponse->generateResponse();
    }
}

Authenticator::Authenticator(BLERemoteService *oneWheelService):
    m_oneWheelService(oneWheelService), m_authenticationState(NOT_STARTED)
{
    if (!oneWheelService->getUUID().equals(ow::UUID::Service)) {
        ESP_LOGE(LOG_TAG, "Provided Service UUID does not match OneWheel Service UUID");
    }
}

bool Authenticator::startAuthentication()
{
    if (m_authenticationState != NOT_STARTED) {
        return false;
    }


//    (1) readCharacteristic:FW REV
    ESP_LOGI(LOG_TAG, "1/%d: read Characteristic: Firmware Revision", authentication_steps);
    auto firmwareRevisionCharacteristic = m_oneWheelService->getCharacteristic(ow::UUID::FirmwareRevisionCharacteristic);
    std::string firmwareRevision = firmwareRevisionCharacteristic->readValue();
    ESP_LOGI(LOG_TAG, "  Firmware Revision: %s", firmwareRevision.c_str());
    esp_log_buffer_hex(LOG_TAG, firmwareRevision.c_str(), firmwareRevision.length());

//       (2) if Characteristic == FW REV
//           check version >= 4034
//       (2a)serial read Characteristic : enable notify
    ESP_LOGI(LOG_TAG, "2/%d: enable notify Characteristic: UART Serial Read", authentication_steps);
    auto serialReadCharacteristic = m_oneWheelService->getCharacteristic(ow::UUID::UartSerialReadCharacteristic);
    serialReadCharacteristic->registerForNotify(notifyCallback);
//       (2b)serial read config descriptor : enable notify (via write)
//    auto serialReadConfigDescriptor = serialReadCharacteristic->getDescriptor(ow::UUID::ConfigDescriptor);
//    serialReadConfigDescriptor->writeValue((uint8_t*)&ow::UUID::BleDescriptorEnableNotificationValue, 2);

// onNotify Characteristic
    // (3a->2a) if serial read characteristic
    //              <SEND SECRET> via write to SerialWriteCharacteristic

// onWrite Descriptor
    // (3b->2b) if serial read config descriptor
    //             write firmware rev characteristic
    ESP_LOGI(LOG_TAG, "3/%d: write Characteristic: Firmware Revision", authentication_steps);
    firmwareRevisionCharacteristic->writeValue(firmwareRevision);

    if (MyChallengeResponse->getResponseSize() > 0) {
        ESP_LOGI(LOG_TAG, "4/%d: challenge-response ready, stopping notifications", authentication_steps);
        serialReadCharacteristic->registerForNotify(nullptr);

        auto serialWriteCharacteristic = m_oneWheelService->getCharacteristic(ow::UUID::UartSerialWriteCharacteristic);

        ESP_LOGI(LOG_TAG, "4/%d: send challenge-response", authentication_steps);
        serialWriteCharacteristic->writeValue(MyChallengeResponse->getResponse(), MyChallengeResponse->getResponseSize());
        ESP_LOGI(LOG_TAG, "4/%d: write complete...", authentication_steps);
    } else {
        ESP_LOGE(LOG_TAG, "sendChallengeResponse, FAIL, no response avail");
    }

    tryAuthenticated();

// onWrite Characteristic
    // (4) if serial write characteristic
    //       DONE!

    return false;
}

void Authenticator::tryAuthenticated()
{
    ESP_LOGI(LOG_TAG, "tryAuthenticated");

    auto batteryRemainingCharacteristic = m_oneWheelService->getCharacteristic(ow::UUID::BatteryRemainingCharacteristic);
    std::string batteryRemainingValue = batteryRemainingCharacteristic->readValue();
    ESP_LOGI(LOG_TAG, "BatteryRemaining: %s", batteryRemainingValue.c_str());
    esp_log_buffer_hex(LOG_TAG, batteryRemainingValue.c_str(), batteryRemainingValue.length());
}

bool Authenticator::isAuthenticated() const
{
    return false;
}

} // namespace ow
