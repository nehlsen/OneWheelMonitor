#include "ow/Monitor.h"

#ifdef __cplusplus
extern "C" {
#endif

void app_main()
{
    auto *monitor = new ow::Monitor;
    monitor->start();
}

#ifdef __cplusplus
} // extern "C"
#endif
