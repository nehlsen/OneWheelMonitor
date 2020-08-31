#ifndef ONEWHEELMONITOR_INPUTBUFFER_H
#define ONEWHEELMONITOR_INPUTBUFFER_H

#include <cstdint>
#include <cstdlib>

class InputBuffer
{
public:
    InputBuffer();
    explicit InputBuffer(uint8_t size);
    virtual ~InputBuffer();

    bool isSaturated() const;

    bool append(uint8_t* pData, size_t length);
    void reset();

    uint8_t getBufferSize() const;
    uint8_t *getBuffer() const;

protected:
    const uint8_t m_bufferSize;
    uint8_t m_writeIndex = 0;
    uint8_t *m_buffer;
};

#endif //ONEWHEELMONITOR_INPUTBUFFER_H
