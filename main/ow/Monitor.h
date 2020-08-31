#ifndef ONEWHEELMONITOR_MONITOR_H
#define ONEWHEELMONITOR_MONITOR_H

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
};

}

#endif //ONEWHEELMONITOR_MONITOR_H
