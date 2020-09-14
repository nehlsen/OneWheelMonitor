#include "UpdateListenerMqtt.h"
#include <Mqtt.h>

#define MQTT_BROKER "mqtt://black-pearl"
#define MQTT_DEVICE_TOPIC "OWM"

namespace ow
{

UpdateListenerMqtt::UpdateListenerMqtt()
{
    m_connectedStatePublisher = EBLi::Mqtt::instance()->createPublisher("connected");
    m_batteryRemainingPublisher = EBLi::Mqtt::instance()->createPublisher("batteryRemaining");
    m_temperaturePublisher = EBLi::Mqtt::instance()->createPublisher("temperature");
}

void UpdateListenerMqtt::setValues(ow::OneWheelValueReader *reader)
{
    m_batteryRemainingPublisher->publishValue(reader->getBatteryRemaining());
    m_temperaturePublisher->publishValue(reader->getTemperature());

//    publishState("batteryRemaining", std::to_string(reader->getBatteryRemaining()));
//    publishState("temperature", std::to_string(reader->getTemperature()));
//    publishState("currentAmps", std::to_string(reader->getCurrentAmps()));
//    publishState("batteryTemp", std::to_string(reader->getBatteryTemp()));
//    publishState("batteryVoltage", std::to_string(reader->getBatteryVoltage()));
}

void UpdateListenerMqtt::setOneWheelIsConnectedAndAuthenticated(bool isConnectedAndAuthenticated)
{
    UpdateListener::setOneWheelIsConnectedAndAuthenticated(isConnectedAndAuthenticated);
    m_connectedStatePublisher->publishValue(isConnectedAndAuthenticated);
}

}
