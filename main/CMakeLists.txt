set(COMPONENT_SRCS
# BTLE Lib - my app
        main.cpp
        ow/Connector.cpp
        ow/Authenticator.cpp
        ow/Authenticator/InputBuffer.cpp
        ow/Authenticator/ChallengeResponse.cpp
        ow/Authenticator/MD5.cpp
        ow/Authenticator/MD5_ESP32.cpp
# BTLE Lib - sample app
#        BLE_Arduino/BLE_client.cpp
# IDF BTLE - modified sample
#        bare/ble.cpp
#        bare/ble2.cpp
#        bare/main.cpp
        )

register_component(
        SRCS ${COMPONENT_SRCS}
        INCLUDE_DIRS "."
)
