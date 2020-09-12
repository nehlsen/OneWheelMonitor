#ifndef ONEWHEELMONITOR_UPDATELISTENERMQTT_H
#define ONEWHEELMONITOR_UPDATELISTENERMQTT_H

#include "UpdateListener.h"

namespace ow
{

class UpdateListenerMqtt : public UpdateListener
{
public:
    void setValues(OneWheelValueReader *reader) override;
};

}

#endif //ONEWHEELMONITOR_UPDATELISTENERMQTT_H
