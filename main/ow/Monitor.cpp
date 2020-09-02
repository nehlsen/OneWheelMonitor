#include "Monitor.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "Connector.h"
#include "OneWheel.h"
#include "Display.h"

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
    m_display = new Display;
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

    m_display->displayText("connecting...");

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
        ESP_LOGI(LOG_TAG, "Temperature:       %dc", m_ow->getTemperature());
        ESP_LOGI(LOG_TAG, "Current Amps:      %d", m_ow->getCurrentAmps());
        ESP_LOGI(LOG_TAG, "Battery Temp:      %dc", m_ow->getBatteryTemp());
        ESP_LOGI(LOG_TAG, "Battery Voltage:   %d", m_ow->getBatteryVoltage());
//        ESP_LOGI(LOG_TAG, "Custom Name:       '%s'", m_ow->getCustomName().c_str());

        m_display->displayPercent(m_ow->getBatteryRemaining());

        updateDelay = updateDelayConnected;
    } else {
        updateDelay = updateDelayNotConnected;
    }
}

}
