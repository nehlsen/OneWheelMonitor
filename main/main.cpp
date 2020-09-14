#include "ow/Monitor.h"
#include <ebli.h>

#ifdef __cplusplus
extern "C" {
#endif

void app_main()
{
    EBLi::init_all();

    auto *monitor = new ow::Monitor;
    monitor->start();
}

#ifdef __cplusplus
} // extern "C"
#endif
