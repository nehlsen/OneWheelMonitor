#include "UpdateListenerLog.h"
#include <esp_log.h>

static const char *LOG_TAG = "ow::Monitor";

void ow::UpdateListenerLog::setValues(OneWheelValueReader *reader)
{
    ESP_LOGI(LOG_TAG, "Battery Remaining: %d%%", reader->getBatteryRemaining());
    ESP_LOGI(LOG_TAG, "Temperature:       %dc", reader->getTemperature());
    ESP_LOGI(LOG_TAG, "Current Amps:      %d", reader->getCurrentAmps());
    ESP_LOGI(LOG_TAG, "Battery Temp:      %dc", reader->getBatteryTemp());
    ESP_LOGI(LOG_TAG, "Battery Voltage:   %d", reader->getBatteryVoltage());
//    ESP_LOGI(LOG_TAG, "Custom Name:       '%s'", m_ow->getCustomName().c_str());
}
