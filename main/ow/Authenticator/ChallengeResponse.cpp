#include "ChallengeResponse.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <esp_log.h>

#define LOG_TAG "ChallengeResponse"

ChallengeResponse::ChallengeResponse(MD5 *md5):
    m_md5(md5)
{
    m_challengeRequest = (uint8_t*)calloc(1, PackageSize);
    m_response = (uint8_t*)calloc(1, PackageSize);
}

ChallengeResponse::~ChallengeResponse()
{
    free(m_challengeRequest);
    free(m_response);
}

uint8_t *ChallengeResponse::getResponse() const
{
    return m_response;
}

uint8_t ChallengeResponse::getResponseSize() const
{
    return PackageSize;
}

void ChallengeResponse::setChallenge(uint8_t *challenge)
{
    memcpy(m_challengeRequest, challenge, PackageSize);
}

void ChallengeResponse::generateResponse()
{
    ESP_LOGI(LOG_TAG, "ChallengeResponse::generateResponse()");
    ESP_LOGI(LOG_TAG, "  received challenge");
    esp_log_buffer_hex(LOG_TAG, m_challengeRequest, PackageSize);

    ESP_LOGI(LOG_TAG, "  challenge checksum:  0x%02x", m_challengeRequest[HeaderSize + DataSize]);
    ESP_LOGI(LOG_TAG, "  calculated checksum: 0x%02x", calculateChecksum(m_challengeRequest));

    uint8_t *challenge = extractChallenge();
    uint8_t *salt = getSecretSalt();

    auto *messageBuffer = (uint8_t*)calloc(2, DataSize);
    memcpy(messageBuffer, challenge, DataSize);
    memcpy(messageBuffer+DataSize, salt, DataSize);

    ESP_LOGI(LOG_TAG, "  both Parts");
    esp_log_buffer_hex(LOG_TAG, messageBuffer, 2*DataSize);

    const unsigned int md5Size = 16;
    auto *digest = m_md5->getDigest(messageBuffer, 2*DataSize);
    ESP_LOGI(LOG_TAG, "  both Parts - MD5");
    esp_log_buffer_hex(LOG_TAG, digest, md5Size);

    resetResponse();
    copyHeader();
    setBody(digest);
    setChecksum();

    ESP_LOGI(LOG_TAG, "  final response");
    esp_log_buffer_hex(LOG_TAG, m_response, PackageSize);

    free(digest);
    free(messageBuffer);
    free(salt);
    free(challenge);
    return;

    // response: <header><md5(challengePart+magicPart)><checkByte>

//    const unsigned int challengePartSize = 16;
//    char *challengePart = (char*)calloc(1, challengePartSize+1);
//    std::strncpy(challengePart, (char*)m_challengeRequest + 3, challengePartSize);
//    ESP_LOGI(LOG_TAG, "  challengePart");
//    esp_log_buffer_hex(LOG_TAG, challengePart, challengePartSize);

//    const unsigned int magicPartSize = 32;
//    char *magicPart = (char*)calloc(1, magicPartSize+1);
//    sprintf(magicPart, "D9255F0F23354E19BA739CCDC4A91765");
//    ESP_LOGI(LOG_TAG, "  magicPart");
//    esp_log_buffer_hex(LOG_TAG, magicPart, magicPartSize);

//    unsigned char *buffer = (unsigned char*)calloc(1, challengePartSize+magicPartSize+1);
//    sprintf((char*)buffer, "%s%s", challengePart, magicPart);
//    ESP_LOGI(LOG_TAG, "  both Parts");
//    esp_log_buffer_hex(LOG_TAG, buffer, challengePartSize+magicPartSize);
//
//    const unsigned int md5Size = 16;
////    struct MD5Context md5_ctx;
////    MD5Init(&md5_ctx);
////    MD5Update(&md5_ctx, buffer, challengePartSize+magicPartSize);
////    unsigned char digest[md5Size+1];
////    MD5Final(digest, &md5_ctx);
//    unsigned char *digest = m_md5->getDigest(buffer, challengePartSize+magicPartSize);
//
//    ESP_LOGI(LOG_TAG, "  both Parts - MD5");
//    esp_log_buffer_hex(LOG_TAG, digest, md5Size);

//    const unsigned int checkByteSize = 1;
//    const unsigned int responseSize = headerSize + md5Size + checkByteSize;
//    char *response = (char*)calloc(1, responseSize + 1);
//    sprintf(response, "%s%s", responseHeader, digest);
//    ESP_LOGI(LOG_TAG, "  response without checksum");
//    esp_log_buffer_hex(LOG_TAG, response, responseSize-1);

//    char checkByte = 0x0;
//    for (int i = 0; i < headerSize + md5Size; ++i) {
//        checkByte ^= response[i];
//    }
//    response[headerSize + md5Size] = checkByte;
//
//    ESP_LOGI(LOG_TAG, "  checksum: 0x%02x", checkByte);
//    ESP_LOGI(LOG_TAG, "  response with checksum");
//    esp_log_buffer_hex(LOG_TAG, response, responseSize);
//
//    m_response = (uint8_t*)response;
////    m_responseSize = responseSize;
//
//    ESP_LOGI(LOG_TAG, "CHALLENGE RESPONSE READY !");
}

uint8_t *ChallengeResponse::extractChallenge()
{
    auto *challenge = (uint8_t*)calloc(1, DataSize);
    memcpy(challenge, m_challengeRequest + HeaderSize, DataSize);

    ESP_LOGI(LOG_TAG, "  extractChallenge");
    esp_log_buffer_hex(LOG_TAG, challenge, DataSize);

    return challenge;
}

uint8_t *ChallengeResponse::getSecretSalt()
{
    // "D9255F0F23354E19BA739CCDC4A91765"
    uint8_t s[] = {
        0xd9, 0x25, 0x5f, 0x0f,
        0x23, 0x35, 0x4e, 0x19,
        0xba, 0x73, 0x9c, 0xcd,
        0xc4, 0xa9, 0x17, 0x65
    };

    auto *salt = (uint8_t*)calloc(1, DataSize);
    memcpy(salt, s, DataSize);

    ESP_LOGI(LOG_TAG, "  getSecretSalt");
    esp_log_buffer_hex(LOG_TAG, salt, DataSize);

    return salt;
}

void ChallengeResponse::resetResponse()
{
    memset(m_response, 0x0, PackageSize);
}

void ChallengeResponse::copyHeader()
{
    memcpy(m_response, m_challengeRequest, HeaderSize);
}

void ChallengeResponse::setBody(uint8_t *data)
{
    memcpy(m_response+HeaderSize, data, DataSize);
}

void ChallengeResponse::setChecksum()
{
    uint8_t checkSum = calculateChecksum(m_response);
    ESP_LOGI(LOG_TAG, "  checksum: 0x%02x", checkSum);
    m_response[HeaderSize + DataSize] = checkSum;
}

uint8_t ChallengeResponse::calculateChecksum(uint8_t *data)
{
    uint8_t checkSum = 0x0;
    for (int i = 0; i < HeaderSize + DataSize; ++i) {
        checkSum ^= data[i];
    }

    return checkSum;
}
