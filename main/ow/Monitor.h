#ifndef ONEWHEELMONITOR_MONITOR_H
#define ONEWHEELMONITOR_MONITOR_H

#include "UpdateListener.h"
#include <vector>

namespace ow
{

class Connector;
class OneWheel;

class Monitor
{
public:
    Monitor();

    void start();

    void doWork();

protected:
    Connector *m_connector;
    OneWheel *m_ow;

    void initUpdateListeners();
    std::vector<UpdateListener*> m_updateListeners;

    void tellAllListenersNotConnected();
    void tellAllListenersConnected();
};

}

#endif //ONEWHEELMONITOR_MONITOR_H
