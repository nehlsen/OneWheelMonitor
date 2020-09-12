#ifndef ONEWHEELMONITOR_UPDATELISTENERMQTT_H
#define ONEWHEELMONITOR_UPDATELISTENERMQTT_H

#include "UpdateListener.h"
#include <mqtt_client.h>
#include <string>

namespace ow
{

class UpdateListenerMqtt : public UpdateListener
{
public:
    UpdateListenerMqtt();
    void setValues(OneWheelValueReader *reader) override;

    void start();
    void stop();
    bool isConnected() const;
    void onMqttConnected();
    void onMqttDisconnected();

protected:
    esp_mqtt_client_handle_t m_client;
    bool m_isConnected = false;

    void publishState(const std::string &state, const std::string &value);
};

}

#endif //ONEWHEELMONITOR_UPDATELISTENERMQTT_H
