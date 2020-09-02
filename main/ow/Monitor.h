#ifndef ONEWHEELMONITOR_MONITOR_H
#define ONEWHEELMONITOR_MONITOR_H

namespace ow
{

class Connector;
class OneWheel;
class Display;

class Monitor
{
public:
    Monitor();

    void start();

    void doWork();

protected:
    Connector *m_connector;
    OneWheel *m_ow;
    Display *m_display;
};

}

#endif //ONEWHEELMONITOR_MONITOR_H
