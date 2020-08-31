#include "Monitor.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Connector.h"
#include "OneWheel.h"

namespace ow
{

static const char *LOG_TAG = "ow::Monitor";
static TaskHandle_t monitor_task_hdnl = nullptr;

[[noreturn]] void monitor_task(void *pvParameter)
{
    ESP_LOGI(LOG_TAG, "monitor_task...");

    auto *monitor = static_cast<Monitor*>(pvParameter);
    ESP_ERROR_CHECK(nullptr == monitor ? ESP_FAIL : ESP_OK);

    while (true) {
        monitor->doWork();

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

Monitor::Monitor()
{
    m_connector = new Connector;
    m_ow = new OneWheel;
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
        ESP_LOGI(LOG_TAG, "Battery Remaining: %d%%", m_ow->getBatteryRemaining());
    }
}

}
