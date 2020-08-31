#include "ChallengeResponse.h"
#include <cstring>
#include <cstdlib>
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

void ChallengeResponse::setChallenge(uint8_t *challenge)
{
    memcpy(m_challengeRequest, challenge, PackageSize);
}

bool ChallengeResponse::generateResponse()
{
    ESP_LOGD(LOG_TAG, "generateResponse()");
    ESP_LOGV(LOG_TAG, "  received challenge");
    ESP_LOG_BUFFER_HEXDUMP(LOG_TAG, m_challengeRequest, PackageSize, ESP_LOG_VERBOSE);

    ESP_LOGV(LOG_TAG, "  challenge checksum:  0x%02x", m_challengeRequest[HeaderSize + DataSize]);
    ESP_LOGV(LOG_TAG, "  calculated checksum: 0x%02x", calculateChecksum(m_challengeRequest));

    if (m_challengeRequest[HeaderSize + DataSize] != calculateChecksum(m_challengeRequest)) {
        ESP_LOGE(LOG_TAG, "generateResponse(), challenge checksum mismatch");
        return false;
    }

    uint8_t *challenge = extractChallenge();
    uint8_t *salt = getSecretSalt();

    auto *messageBuffer = (uint8_t*)calloc(2, DataSize);
    memcpy(messageBuffer, challenge, DataSize);
    memcpy(messageBuffer+DataSize, salt, DataSize);

    ESP_LOGV(LOG_TAG, "  both Parts");
    ESP_LOG_BUFFER_HEXDUMP(LOG_TAG, messageBuffer, 2*DataSize, ESP_LOG_VERBOSE);

    const unsigned int md5Size = 16;
    auto *digest = m_md5->getDigest(messageBuffer, 2*DataSize);
    ESP_LOGV(LOG_TAG, "  both Parts - MD5");
    ESP_LOG_BUFFER_HEXDUMP(LOG_TAG, digest, md5Size, ESP_LOG_VERBOSE);

    resetResponse();
    copyHeader();
    setBody(digest);
    setChecksum();

    ESP_LOGV(LOG_TAG, "  final response");
    ESP_LOG_BUFFER_HEXDUMP(LOG_TAG, m_response, PackageSize, ESP_LOG_VERBOSE);

    free(digest);
    free(messageBuffer);
    free(salt);
    free(challenge);

    return true;
}

uint8_t *ChallengeResponse::extractChallenge()
{
    auto *challenge = (uint8_t*)calloc(1, DataSize);
    memcpy(challenge, m_challengeRequest + HeaderSize, DataSize);

    ESP_LOGV(LOG_TAG, "  extractChallenge");
    ESP_LOG_BUFFER_HEXDUMP(LOG_TAG, challenge, DataSize, ESP_LOG_VERBOSE);

    return challenge;
}

uint8_t *ChallengeResponse::getSecretSalt() const
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

    ESP_LOGV(LOG_TAG, "  getSecretSalt");
    ESP_LOG_BUFFER_HEXDUMP(LOG_TAG, salt, DataSize, ESP_LOG_VERBOSE);

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
    ESP_LOGV(LOG_TAG, "  checksum: 0x%02x", checkSum);
    m_response[HeaderSize + DataSize] = checkSum;
}

uint8_t ChallengeResponse::calculateChecksum(const uint8_t *data) const
{
    uint8_t checkSum = 0x0;
    for (int i = 0; i < HeaderSize + DataSize; ++i) {
        checkSum ^= data[i];
    }

    return checkSum;
}
