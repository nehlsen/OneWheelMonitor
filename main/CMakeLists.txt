set(COMPONENT_SRCS
        main.cpp
        ow/Monitor.cpp
        ow/OneWheel.cpp
        ow/OneWheelValueReader.cpp
        ow/Display.cpp
        ow/Connector.cpp
        ow/ConnectorCallbacks.cpp
        ow/Authenticator.cpp
        ow/Authenticator/InputBuffer.cpp
        ow/Authenticator/ChallengeResponse.cpp
        ow/Authenticator/MD5.cpp
        ow/Authenticator/MD5_ESP32.cpp
        ow/util.cpp
        ow/UpdateListener.cpp
        ow/UpdateListenerLog.cpp
        ow/UpdateListenerDisplay.cpp
        ow/UpdateListenerMqtt.cpp
        )

register_component(
        SRCS ${COMPONENT_SRCS}
        INCLUDE_DIRS "."
)
