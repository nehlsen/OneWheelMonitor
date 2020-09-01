#include "Authenticator.h"
#include "UUID.h"
#include <esp_log.h>
#include <BLERemoteService.h>
#include <BLERemoteCharacteristic.h>
#include "Authenticator/InputBuffer.h"
#include "Authenticator/ChallengeResponse.h"
#include "Authenticator/MD5_ESP32.h"
#include "util.h"

namespace ow
{

#define LOG_TAG "ow::Authenticator"

static const int authentication_steps = 5;

static InputBuffer *MyInputBuffer = new InputBuffer;
static ChallengeResponse *MyChallengeResponse = new ChallengeResponse(new MD5_ESP32);

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    ESP_LOGV(LOG_TAG, "Notify callback, received %d bytes", length);

    MyInputBuffer->append(pData, length);
    if (MyInputBuffer->isSaturated()) {
        MyChallengeResponse->setChallenge(MyInputBuffer->getBuffer());
        MyInputBuffer->reset();
    }
}

Authenticator::Authenticator(BLERemoteService *oneWheelService):
    m_oneWheelService(oneWheelService), m_authenticationState(NOT_STARTED)
{
    if (!oneWheelService->getUUID().equals(ow::UUID::Service)) {
        ESP_LOGE(LOG_TAG, "Provided Service UUID does not match OneWheel Service UUID");
    }
}

bool Authenticator::maintainAuthentication()
{
    if (isAuthenticated() && tryAuthenticated()) {
        ESP_LOGD(LOG_TAG, "maintainAuthentication: still authenticated");
        return true;
    }
    if (!isAuthenticated() && m_authenticationState != NOT_STARTED) {
        ESP_LOGD(LOG_TAG, "maintainAuthentication: authentication in progress");
        return true;
    }

    ESP_LOGW(LOG_TAG, "maintainAuthentication: authentication lost, re-authenticating");
    m_authenticationState = NOT_STARTED;
    return authenticate();
}

bool Authenticator::isAuthenticated() const
{
    return m_authenticationState == AUTHENTICATION_COMPLETE;
}

bool Authenticator::authenticate()
{
    if (m_authenticationState != NOT_STARTED) {
        return false;
    }

    enableSerialReadNotify();
    if (!requestChallenge()) {
        return false;
    }

    if (MyChallengeResponse->generateResponse()) {
        disableSerialReadNotify();
        sendChallengeResponse();
    } else {
        m_authenticationState = CHALLENGE_RESPONSE_FAILED_TO_GENERATE;
        ESP_LOGE(LOG_TAG, "sendChallengeResponse, FAIL, no response avail");
    }

    if (tryAuthenticated()) {
        ESP_LOGI(LOG_TAG, "5/%d: tryAuthenticated SUCCEED", authentication_steps);
        m_authenticationState = AUTHENTICATION_COMPLETE;
        return true;
    } else {
        ESP_LOGI(LOG_TAG, "5/%d: tryAuthenticated FAILED", authentication_steps);
        m_authenticationState = TRY_AUTH_FAILED;
        return false;
    }
}

void Authenticator::enableSerialReadNotify()
{
    ESP_LOGI(LOG_TAG, "1/%d: enable notify Characteristic: UART Serial Read", authentication_steps);
    auto serialReadCharacteristic = m_oneWheelService->getCharacteristic(UUID::UartSerialReadCharacteristic);
    serialReadCharacteristic->registerForNotify(notifyCallback);
    m_authenticationState = NOTIFICATIONS_ENABLED;
}

void Authenticator::disableSerialReadNotify()
{
    auto serialReadCharacteristic = m_oneWheelService->getCharacteristic(UUID::UartSerialReadCharacteristic);
    serialReadCharacteristic->registerForNotify(nullptr);
}

bool Authenticator::requestChallenge()
{
    ESP_LOGI(LOG_TAG, "2/%d: read Characteristic: Firmware Revision", authentication_steps);
    auto firmwareRevisionCharacteristic = m_oneWheelService->getCharacteristic(UUID::FirmwareRevisionCharacteristic);
    std::string firmwareRevision = firmwareRevisionCharacteristic->readValue();
    if (!isFirmwareRevisionCompatible(twoByteHexToInt(firmwareRevisionCharacteristic->readRawData()))) {
        return false;
    }

    ESP_LOGI(LOG_TAG, "3/%d: write Characteristic: Firmware Revision", authentication_steps);
    firmwareRevisionCharacteristic->writeValue(firmwareRevision);
    m_authenticationState = FIRMWARE_WRITE_BACK;
    waitAfterWrite();

    return true;
}

void Authenticator::waitAfterWrite() const
{
    static const int writeWait = 250;
    vTaskDelay(writeWait / portTICK_PERIOD_MS);
}

bool Authenticator::isFirmwareRevisionCompatible(int firmwareRevision)
{
    if (firmwareRevision < 4134 || firmwareRevision >= 5000) {
        ESP_LOGE(LOG_TAG, "isFirmwareRevisionCompatible, FAIL, version %d not compatible", firmwareRevision);

        m_authenticationState = FIRMWARE_CHECK_FAILED;
        return false;
    }

    m_authenticationState = FIRMWARE_CHECK_SUCCEED;
    return true;
}

void Authenticator::sendChallengeResponse()
{
    auto serialWriteCharacteristic = m_oneWheelService->getCharacteristic(UUID::UartSerialWriteCharacteristic);
    ESP_LOGI(LOG_TAG, "4/%d: send challenge-response", authentication_steps);
    serialWriteCharacteristic->writeValue(MyChallengeResponse->getResponse(), ChallengeResponse::PackageSize);
    m_authenticationState = CHALLENGE_RESPONSE_WRITTEN;
    waitAfterWrite();
}

bool Authenticator::tryAuthenticated()
{
    ESP_LOGD(LOG_TAG, "tryAuthenticated");

    auto batteryRemainingCharacteristic = m_oneWheelService->getCharacteristic(ow::UUID::BatteryRemainingCharacteristic);
    batteryRemainingCharacteristic->readValue();
    const uint8_t batteryPercent = twoByteHexToUint8(batteryRemainingCharacteristic->readRawData());

    ESP_LOGD(LOG_TAG, "tryAuthenticated, battery remaining %d%%", batteryPercent);
    return batteryPercent > 0;
}

} // namespace ow
