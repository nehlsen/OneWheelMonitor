#include "InputBuffer.h"
#include <cstring>
#include <esp_log.h>

#define LOG_TAG "InputBuffer"

InputBuffer::InputBuffer():
    InputBuffer(20)
{}

InputBuffer::InputBuffer(uint8_t size): m_bufferSize(size)
{
    m_buffer = (uint8_t*)calloc(1, m_bufferSize);
}

InputBuffer::~InputBuffer()
{
    free(m_buffer);
}

bool InputBuffer::isSaturated() const
{
    return m_writeIndex == m_bufferSize;
}

bool InputBuffer::append(uint8_t* pData, size_t length)
{
    if (m_writeIndex + length > m_bufferSize) {
        ESP_LOGE(LOG_TAG, "Buffer overflow!");
        return false;
    }

    memcpy(m_buffer + m_writeIndex, pData, length);
    m_writeIndex += length;

    if (isSaturated()) {
        ESP_LOGD(LOG_TAG, "Buffer saturated!");
    }

    return true;
}

void InputBuffer::reset()
{
    ESP_LOGD(LOG_TAG, "reset");

    memset(m_buffer, 0x0, m_bufferSize);
    m_writeIndex = 0;
}

uint8_t InputBuffer::getBufferSize() const
{
    return m_bufferSize;
}

uint8_t *InputBuffer::getBuffer() const
{
    return m_buffer;
}
