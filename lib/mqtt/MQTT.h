#ifndef MQTT_h
#define MQTT_h
#include "ConfigInterface.h"
#include "ShiftRegisterIO.h"
#include "ConfigInterface.h"
#include "AsyncMqttClient.h"
#include <ESP8266WiFi.h>
#include <Ticker.h>

class MQTT
{
    Configuration *config;

public:
    MQTT();

    void init();
    // void connectToWifi();
    // void connectToMqtt();
    // void onMqttConnect(bool sessionPresent);
    // void onMqttDisconnect(AsyncMqttClientDisconnectReason reason);
    // void onMqttSubscribe(uint16_t packetId, uint8_t qos);
    // void onMqttUnsubscribe(uint16_t packetId);
    // void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);
    // void onMqttPublish(uint16_t packetId);
    void send(struct ShiftRegisterIO *shiftRegister_io, struct Configuration *config, struct MeterData *meterData, struct HeaterData *heaterData);
};
#endif