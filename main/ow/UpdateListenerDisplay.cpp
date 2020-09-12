#include "UpdateListenerDisplay.h"
#include "Display.h"

ow::UpdateListenerDisplay::UpdateListenerDisplay()
{
    m_display = new Display;
    m_display->displayText("connecting...");
}

ow::UpdateListenerDisplay::~UpdateListenerDisplay()
{
    delete m_display;
}

void ow::UpdateListenerDisplay::setValues(ow::OneWheelValueReader *reader)
{
    m_display->displayPercent(reader->getBatteryRemaining());
}
