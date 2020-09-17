#ifndef ONEWHEELMONITOR_UPDATELISTENERMQTT_H
#define ONEWHEELMONITOR_UPDATELISTENERMQTT_H

#include "UpdateListener.h"
#include <MqttPublisher.h>

namespace ow
{

class UpdateListenerMqtt : public UpdateListener
{
public:
    UpdateListenerMqtt();

    void setOneWheelIsConnectedAndAuthenticated(bool isConnectedAndAuthenticated) override;

    void setValues(OneWheelValueReader *reader) override;

protected:
    EBLi::MqttPublisher *m_connectedStatePublisher;
    EBLi::MqttPublisher *m_batteryRemainingPublisher;
    EBLi::MqttPublisher *m_temperaturePublisher;
    EBLi::MqttPublisher *m_currentAmpsPublisher;
    EBLi::MqttPublisher *m_batteryTempPublisher;
    EBLi::MqttPublisher *m_batteryVoltagePublisher;
};

}

#endif //ONEWHEELMONITOR_UPDATELISTENERMQTT_H
