#include "UpdateListenerMqtt.h"
#include <esp_log.h>

#define MQTT_BROKER "mqtt://black-pearl"
#define MQTT_DEVICE_TOPIC "OWM"

namespace ow
{

static const char *LOG_TAG = "ow::UpdateListenerMqtt";

static void on_got_ip(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    auto listener = static_cast<UpdateListenerMqtt*>(event_handler_arg);
    ESP_ERROR_CHECK(nullptr == listener ? ESP_FAIL : ESP_OK);

    listener->start();
}

void mqtt_event_connected(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    auto listener = static_cast<UpdateListenerMqtt*>(event_handler_arg);
    ESP_ERROR_CHECK(nullptr == listener ? ESP_FAIL : ESP_OK);

    listener->onMqttConnected();
}

void mqtt_event_disconnected(void* event_handler_arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    auto listener = static_cast<UpdateListenerMqtt*>(event_handler_arg);
    ESP_ERROR_CHECK(nullptr == listener ? ESP_FAIL : ESP_OK);

    listener->onMqttDisconnected();
}

UpdateListenerMqtt::UpdateListenerMqtt()
{
    esp_mqtt_client_config_t mqtt_cfg = {
            .uri = MQTT_BROKER,
    };
    m_client = esp_mqtt_client_init(&mqtt_cfg);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_got_ip, this);
    esp_mqtt_client_register_event(m_client, MQTT_EVENT_CONNECTED, mqtt_event_connected, this);
    esp_mqtt_client_register_event(m_client, MQTT_EVENT_DISCONNECTED, mqtt_event_disconnected, this);
}

void UpdateListenerMqtt::setValues(ow::OneWheelValueReader *reader)
{
    publishState("batteryRemaining", std::to_string(reader->getBatteryRemaining()));
    publishState("temperature", std::to_string(reader->getTemperature()));
    publishState("currentAmps", std::to_string(reader->getCurrentAmps()));
    publishState("batteryTemp", std::to_string(reader->getBatteryTemp()));
    publishState("batteryVoltage", std::to_string(reader->getBatteryVoltage()));
}

void UpdateListenerMqtt::start()
{
    esp_err_t err = esp_mqtt_client_start(m_client);
    if (err != ESP_OK) {
        ESP_LOGW(LOG_TAG, "MQTT Start Error: \"%s\"", esp_err_to_name(err));
        return;
    }
    ESP_LOGI(LOG_TAG, "MQTT Started.");
}

void UpdateListenerMqtt::stop()
{
    esp_mqtt_client_stop(m_client);
    ESP_LOGI(LOG_TAG, "MQTT Stopped.");
}

bool UpdateListenerMqtt::isConnected() const
{
    return m_isConnected;
}

void UpdateListenerMqtt::onMqttConnected()
{
    m_isConnected = true;

    ESP_LOGI(LOG_TAG, "MQTT Connection established...");
}

void UpdateListenerMqtt::onMqttDisconnected()
{
    m_isConnected = false;

    ESP_LOGI(LOG_TAG, "MQTT Connection lost...");
}

void UpdateListenerMqtt::publishState(const std::string &state, const std::string &value)
{
    if (!isConnected()) {
        return;
    }

    std::string topic = "/";
    topic += MQTT_DEVICE_TOPIC;
    topic += "/state/" + state;

    esp_mqtt_client_publish(m_client, topic.c_str(), value.c_str(), 0, 0, 0);
}

}
