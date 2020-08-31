#include "ChallengeResponse.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <esp_log.h>

#define LOG_TAG "ChallengeResponse"

ChallengeResponse::ChallengeResponse(MD5 *md5):
    m_md5(md5)
{
    m_challenge = (uint8_t*)calloc(1, ChallengeSize);
    m_response = (uint8_t*)calloc(1, ChallengeSize);
}

ChallengeResponse::~ChallengeResponse()
{
    free(m_challenge);
    free(m_response);
}

uint8_t *ChallengeResponse::getResponse() const
{
    return m_response;
}

uint8_t ChallengeResponse::getResponseSize() const
{
    return ChallengeSize;
}

void ChallengeResponse::setChallenge(uint8_t *challenge)
{
    memcpy(m_challenge, challenge, ChallengeSize);
}

void ChallengeResponse::generateResponse()
{
    ESP_LOGI(LOG_TAG, "ChallengeResponse::generateResponse()");
    ESP_LOGI(LOG_TAG, "  received challenge");
    esp_log_buffer_hex(LOG_TAG, m_challenge, ChallengeSize);

    resetResponse();
    copyHeader();

    return;

    // response: <header><md5(challengePart+magicPart)><checkByte>

    const unsigned int headerSize = 3;
    char *responseHeader = (char*)calloc(1, headerSize+1);
    sprintf(responseHeader, "%c%c%c", 0x43, 0x52, 0x58);
    ESP_LOGI(LOG_TAG, "  header");
    esp_log_buffer_hex(LOG_TAG, responseHeader, headerSize);
//        char header[] = {0x43, 0x52, 0x58};

    const unsigned int challengePartSize = 16;
    char *challengePart = (char*)calloc(1, challengePartSize+1);
    std::strncpy(challengePart, (char*)m_challenge + 3, challengePartSize);
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

    const unsigned int md5Size = 16;
//    struct MD5Context md5_ctx;
//    MD5Init(&md5_ctx);
//    MD5Update(&md5_ctx, buffer, challengePartSize+magicPartSize);
//    unsigned char digest[md5Size+1];
//    MD5Final(digest, &md5_ctx);
    unsigned char *digest = m_md5->getDigest(buffer, challengePartSize+magicPartSize);

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
//    m_responseSize = responseSize;

    ESP_LOGI(LOG_TAG, "CHALLENGE RESPONSE READY !");
}

void ChallengeResponse::resetResponse()
{
    memset(m_response, 0x0, ChallengeSize);
}

void ChallengeResponse::copyHeader()
{
    const size_t headerSize = 3;
    memcpy(m_response, m_challenge, headerSize);
}
