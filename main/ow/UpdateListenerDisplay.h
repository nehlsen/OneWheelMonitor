#ifndef ONEWHEELMONITOR_UPDATELISTENERDISPLAY_H
#define ONEWHEELMONITOR_UPDATELISTENERDISPLAY_H

#include "UpdateListener.h"

namespace ow
{

class Display;

class UpdateListenerDisplay : public UpdateListener
{
public:
    UpdateListenerDisplay();
    ~UpdateListenerDisplay();

    void setValues(OneWheelValueReader *reader) override;

protected:
    Display *m_display;
};

}

#endif //ONEWHEELMONITOR_UPDATELISTENERDISPLAY_H
