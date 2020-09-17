#include "UpdateListenerMqtt.h"
#include <Mqtt.h>

#define MQTT_BROKER "mqtt://black-pearl"
#define MQTT_DEVICE_TOPIC "OWM"

namespace ow
{

UpdateListenerMqtt::UpdateListenerMqtt()
{
    auto *mqtt = EBLi::Mqtt::instance();
    m_connectedStatePublisher = mqtt->createPublisher("connected");
    m_batteryRemainingPublisher = mqtt->createPublisher("batteryRemaining");
    m_temperaturePublisher = mqtt->createPublisher("temperature");
    m_currentAmpsPublisher = mqtt->createPublisher("currentAmps");
    m_batteryTempPublisher = mqtt->createPublisher("batteryTemp");
    m_batteryVoltagePublisher = mqtt->createPublisher("batteryVoltage");
}

void UpdateListenerMqtt::setValues(ow::OneWheelValueReader *reader)
{
    m_batteryRemainingPublisher->publishValue(reader->getBatteryRemaining());
    m_temperaturePublisher->publishValue(reader->getTemperature());
    m_currentAmpsPublisher->publishValue(reader->getCurrentAmps());
    m_batteryTempPublisher->publishValue(reader->getBatteryTemp());
    m_batteryVoltagePublisher->publishValue(reader->getBatteryVoltage());
}

void UpdateListenerMqtt::setOneWheelIsConnectedAndAuthenticated(bool isConnectedAndAuthenticated)
{
    UpdateListener::setOneWheelIsConnectedAndAuthenticated(isConnectedAndAuthenticated);
    m_connectedStatePublisher->publishValue(isConnectedAndAuthenticated);
}

}
