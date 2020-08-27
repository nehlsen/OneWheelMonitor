#include "ow/Connector.h"

#ifdef __cplusplus
extern "C" {
#endif

void app_main()
{
    auto *connector = new ow::Connector;
    connector->start();
//    connector->scanAndConnect();
    connector->connect(BLEAddress("90:e2:02:2b:21:63"));
}

#ifdef __cplusplus
} // extern "C"
#endif
