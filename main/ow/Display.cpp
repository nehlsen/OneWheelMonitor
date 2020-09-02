#include "Display.h"
#include <ssd1306_default_if.h>
#include <ssd1306_draw.h>
#include <ssd1306_font.h>

namespace ow
{

static const int I2CDisplayAddress = 0x3C;
static const int I2CDisplayWidth = 128;
static const int I2CDisplayHeight = 64;
static const int I2CResetPin = -1;

Display::Display()
{
    assert(SSD1306_I2CMasterInitDefault( ) == true);
    assert(SSD1306_I2CMasterAttachDisplayDefault(&m_I2CDisplay, I2CDisplayWidth, I2CDisplayHeight, I2CDisplayAddress, I2CResetPin) == true);
}

void Display::displayText(char *text)
{
    SSD1306_Clear(&m_I2CDisplay, SSD_COLOR_BLACK);
    SSD1306_SetFont(&m_I2CDisplay, &Font_droid_sans_mono_7x13);
    SSD1306_FontDrawAnchoredString(&m_I2CDisplay, TextAnchor_Center, text, SSD_COLOR_WHITE);
    SSD1306_Update(&m_I2CDisplay);
}

void Display::displayPercent(uint8_t percent)
{
    char t[] = {"999%"};
    sprintf(t, "%d%%", percent);

    SSD1306_Clear(&m_I2CDisplay, SSD_COLOR_BLACK);
    SSD1306_SetFont(&m_I2CDisplay, &Font_droid_sans_mono_16x31);
    SSD1306_FontDrawAnchoredString(&m_I2CDisplay, TextAnchor_Center, t, SSD_COLOR_WHITE);
    SSD1306_Update(&m_I2CDisplay);
}

}
