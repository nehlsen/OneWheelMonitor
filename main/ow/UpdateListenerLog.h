#ifndef ONEWHEELMONITOR_UPDATELISTENERLOG_H
#define ONEWHEELMONITOR_UPDATELISTENERLOG_H

#include "UpdateListener.h"

namespace ow
{

class UpdateListenerLog : public UpdateListener
{
public:
    void setValues(OneWheelValueReader *reader) override;
};

}

#endif //ONEWHEELMONITOR_UPDATELISTENERLOG_H
