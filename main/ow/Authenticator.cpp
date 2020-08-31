#include "Authenticator.h"
#include "UUID.h"
#include <esp_log.h>
#include <BLERemoteService.h>
#include <BLERemoteCharacteristic.h>
#include <BLERemoteDescriptor.h>
#include <cstring>
#include "esp32/rom/md5_hash.h"

namespace ow
{

static int authentication_steps = 4;
#define LOG_TAG "ow::Authenticator"


class ChallengeResponse
{
public:
    ChallengeResponse()
    {
        // we expect 20 characters as input
        m_inputBuffer = (uint8_t*)calloc(1, InputBufferSize);
    }
    virtual ~ChallengeResponse()
    {
        free(m_inputBuffer);
    }

    void setAuthenticator(Authenticator *auth)
    {
        m_auth = auth;
    }
    void setResponseWriteTo(BLERemoteCharacteristic *serialWriteCharacteristic)
    {
        m_writeResponseTo = serialWriteCharacteristic;
    }

    void readData(uint8_t* pData, size_t length)
    {
        if (m_inputWriteIndex + length > InputBufferSize) {
            ESP_LOGE(LOG_TAG, "ChallengeResponse: Input Buffer overflow!");
            return;
        }

        memcpy(m_inputBuffer + m_inputWriteIndex, pData, length);
        m_inputWriteIndex += length;

        if (m_inputWriteIndex == InputBufferSize) {
            ESP_LOGI(LOG_TAG, "ChallengeResponse: Input Buffer saturated!");
            doTheDeed();
        }
    }

    void doTheDeed()
    {
        ESP_LOGI(LOG_TAG, "received challenge");
        esp_log_buffer_hex(LOG_TAG, m_inputBuffer, InputBufferSize);

        // response: <header><md5(challengePart+magicPart)><checkByte>

        const unsigned int headerSize = 3;
        char *responseHeader = (char*)calloc(1, headerSize+1);
        sprintf(responseHeader, "%c%c%c", 0x43, 0x52, 0x58);
        ESP_LOGI(LOG_TAG, "  header");
        esp_log_buffer_hex(LOG_TAG, responseHeader, headerSize);

        const unsigned int challengePartSize = 16;
        char *challengePart = (char*)calloc(1, challengePartSize+1);
        std::strncpy(challengePart, (char*)m_inputBuffer + 3, challengePartSize);
        ESP_LOGI(LOG_TAG, "  challengePart");
        esp_log_buffer_hex(LOG_TAG, challengePart, challengePartSize);

        const unsigned int magicPartSize = 32;
        char *magicPart = (char*)calloc(1, magicPartSize+1);
        sprintf(magicPart, "D9255F0F23354E19BA739CCDC4A91765");
        ESP_LOGI(LOG_TAG, "  magicPart");
        esp_log_buffer_hex(LOG_TAG, magicPart, magicPartSize);

        unsigned char *buffer = (unsigned char*)calloc(1, challengePartSize+magicPartSize+1);
        sprintf((char*)buffer, "%s%s", challengePart, magicPart);
        ESP_LOGI(LOG_TAG, "  both Parts");
        esp_log_buffer_hex(LOG_TAG, buffer, challengePartSize+magicPartSize);

        const unsigned int md5Size = 16; // FIXME or is this 32?
        struct MD5Context md5_ctx;
        MD5Init(&md5_ctx);
        MD5Update(&md5_ctx, buffer, challengePartSize+magicPartSize);
        unsigned char digest[md5Size+1];
        MD5Final(digest, &md5_ctx);

        ESP_LOGI(LOG_TAG, "  both Parts - MD5");
        esp_log_buffer_hex(LOG_TAG, digest, md5Size);

        const unsigned int checkByteSize = 1;
        const unsigned int responseSize = headerSize + md5Size + checkByteSize;
        char *response = (char*)calloc(1, responseSize + 1);
        sprintf(response, "%s%s", responseHeader, digest);
        ESP_LOGI(LOG_TAG, "  response without checksum");
        esp_log_buffer_hex(LOG_TAG, response, responseSize-1);

        char checkByte = 0x0;
        for (int i = 0; i < headerSize + md5Size; ++i) {
            checkByte ^= response[i];
        }
        response[headerSize + md5Size] = checkByte;

        ESP_LOGI(LOG_TAG, "  checksum: 0x%02x", checkByte);
        ESP_LOGI(LOG_TAG, "  response with checksum");
        esp_log_buffer_hex(LOG_TAG, response, responseSize);

        m_response = (uint8_t*)response;
        m_responseSize = responseSize;

        ESP_LOGI(LOG_TAG, "CHALLENGE RESPONSE READY !");


//        if (m_writeResponseTo) {
//            ESP_LOGI(LOG_TAG, "4/%d: send challenge-response", authentication_steps);
//            m_writeResponseTo->writeValue((uint8_t*)response, responseSize);
//            ESP_LOGI(LOG_TAG, "4/%d: write complete...", authentication_steps);
//
////            m_auth->tryAuthenticated();
//        }
    }

    void sendChallengeResponse()
    {
        if (m_responseSize == 0) {
            ESP_LOGE(LOG_TAG, "sendChallengeResponse, FAIL, no response avail");
            return;
        }
        if (m_writeResponseTo == nullptr) {
            ESP_LOGE(LOG_TAG, "sendChallengeResponse, FAIL, not able to write");
            return;
        }

        ESP_LOGI(LOG_TAG, "4/%d: send challenge-response", authentication_steps);
        m_writeResponseTo->writeValue(m_response, m_responseSize);
        ESP_LOGI(LOG_TAG, "4/%d: write complete...", authentication_steps);

//        m_auth->tryAuthenticated();
    }

protected:
    Authenticator *m_auth;
    BLERemoteCharacteristic *m_writeResponseTo = nullptr;

    const uint8_t InputBufferSize = 20;
    uint8_t m_inputWriteIndex = 0;
    uint8_t *m_inputBuffer;

    uint8_t *m_response = nullptr;
    uint8_t m_responseSize = 0;
};

static ChallengeResponse *MyChallengeResponse = new ChallengeResponse;


static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
    ESP_LOGI(__FILE__, "Notify callback, received %d bytes", length);
//    ESP_LOGI(__FILE__, "Notify callback for characteristic %s", pBLERemoteCharacteristic->getUUID().toString().c_str());
//    ESP_LOGI(__FILE__, " of data length %d", length);
//    ESP_LOGI(__FILE__, "data: %s", (char*)pData);

    MyChallengeResponse->readData(pData, length);
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

    MyChallengeResponse->setAuthenticator(this);


//    (1) readCharacteristic:FW REV
    ESP_LOGI(LOG_TAG, "1/%d: read Characteristic: Firmware Revision", authentication_steps);
    auto firmwareRevisionCharacteristic = m_oneWheelService->getCharacteristic(ow::UUID::FirmwareRevisionCharacteristic);
    std::string firmwareRevision = firmwareRevisionCharacteristic->readValue();
    ESP_LOGI(LOG_TAG, "  Firmware Revision: %s", firmwareRevision.c_str());
    esp_log_buffer_hex(LOG_TAG, firmwareRevision.c_str(), firmwareRevision.length());

    auto serialWriteCharacteristic = m_oneWheelService->getCharacteristic(ow::UUID::UartSerialWriteCharacteristic);
    MyChallengeResponse->setResponseWriteTo(serialWriteCharacteristic);

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

    MyChallengeResponse->sendChallengeResponse();
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
