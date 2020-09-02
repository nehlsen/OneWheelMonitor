#ifndef ONEWHEELMONITOR_DISPLAY_H
#define ONEWHEELMONITOR_DISPLAY_H

#include <cstdint>
#include <ssd1306.h>

namespace ow
{

class Display
{
public:
    Display();

    void displayText(char *text);
    void displayPercent(uint8_t percent);

protected:
    SSD1306_Device m_I2CDisplay;
};

}

#endif //ONEWHEELMONITOR_DISPLAY_H
