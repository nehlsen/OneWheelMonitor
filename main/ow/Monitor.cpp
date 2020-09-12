#include "Monitor.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Connector.h"
#include "OneWheel.h"
#include "UpdateListenerLog.h"

#ifdef CONFIG_ENABLE_DISPLAY
#include "UpdateListenerDisplay.h"
#endif

#ifdef CONFIG_ENABLE_MQTT
#include "UpdateListenerMqtt.h"
#endif

namespace ow
{

static const char *LOG_TAG = "ow::Monitor";
static TaskHandle_t monitor_task_hdnl = nullptr;

static const int updateDelayNotConnected = 1000;
static const int updateDelayConnected = 5000;
static int updateDelay = updateDelayNotConnected;

[[noreturn]] void monitor_task(void *pvParameter)
{
    ESP_LOGI(LOG_TAG, "monitor_task...");

    auto *monitor = static_cast<Monitor*>(pvParameter);
    ESP_ERROR_CHECK(nullptr == monitor ? ESP_FAIL : ESP_OK);

    while (true) {
        monitor->doWork();

        vTaskDelay(updateDelay / portTICK_PERIOD_MS);
    }
}

Monitor::Monitor()
{
    m_connector = new Connector;
    m_ow = new OneWheel;
    initUpdateListeners();
}

void Monitor::start()
{
    if (monitor_task_hdnl) {
        ESP_LOGE(LOG_TAG, "already started");
    }

    ESP_LOGI(LOG_TAG, "starting Monitor...");
    xTaskCreate(
            &monitor_task,
            "monitor_task",
            4 * 1024,
            this,
            5,
            &monitor_task_hdnl
    );

//    m_connector->scanAndConnect();
    m_connector->connect(BLEAddress("90:e2:02:2b:21:63"));
}

void Monitor::doWork()
{
    m_connector->maintainConnection();

    if (!m_connector->isConnected()) {
        m_ow->tearDown();
    } else if(!m_ow->isReady()) {
        m_ow->setup(m_connector->getRootService());
    }

    if (m_ow->isReady()) {
        tellAllListenersConnected();
        updateDelay = updateDelayConnected;
    } else {
        tellAllListenersNotConnected();
        updateDelay = updateDelayNotConnected;
    }
}

void Monitor::initUpdateListeners()
{
    m_updateListeners.push_back(new UpdateListenerLog);

#ifdef CONFIG_ENABLE_DISPLAY
    m_updateListeners.push_back(new UpdateListenerDisplay);
#endif

#ifdef CONFIG_ENABLE_MQTT
    m_updateListeners.push_back(new UpdateListenerMqtt);
#endif
}

void Monitor::tellAllListenersNotConnected()
{
    for (UpdateListener *listener : m_updateListeners) {
        listener->setOneWheelIsConnectedAndAuthenticated(false);
    }
}

void Monitor::tellAllListenersConnected()
{
    for (UpdateListener *listener : m_updateListeners) {
        listener->setOneWheelIsConnectedAndAuthenticated(true);
        listener->setValues(m_ow);
    }
}

}
