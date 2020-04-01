#include "MQTT.h"
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include <ArduinoLog.h>

const char *WiFi_SSID;
const char *WiFi_Password;

const char *MQTT_ServerAddress;
const char *MQTT_Port;

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

MQTT::MQTT()
{
}

void MQTT::init()
{
}

void connectToWifi()
{
    Log.notice("Connecting to Wi-Fi...");
    WiFi.begin(WiFi_SSID, WiFi_Password);
}

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
    Log.notice("Connected to Wi-Fi.");
    connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
    Log.warning("Disconnected from Wi-Fi.");
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt()
{
    Log.notice("Connecting to MQTT...");
    mqttClient.connect();
}

void onMqttConnect(bool sessionPresent)
{
    Log.notice("Connected to MQTT." CR);
    Log.notice("Session present: ");
    Log.notice(sessionPresent CR);
    uint16_t packetIdSub = mqttClient.subscribe("test/lol", 2);
    Log.notice("Subscribing at QoS 2, packetId: ");
    Log.notice(packetIdSub CR);
    mqttClient.publish("test/lol", 0, true, "test 1");
    Log.notice("Publishing at QoS 0" CR);
    uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
    Log.notice("Publishing at QoS 1, packetId: ");
    Log.notice(packetIdPub1 CR);
    uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
    Log.notice("Publishing at QoS 2, packetId: ");
    Log.notice(packetIdPub2 CR);
}
void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Log.warning("Disconnected from MQTT." CR);

    if (WiFi.isConnected())
    {
        mqttReconnectTimer.once(2, connectToMqtt);
    }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos)
{
    Log.notice("Subscribe acknowledged." CR);
    Log.notice("  packetId: ");
    Log.notice(packetId CR);
    Log.notice("  qos: ");
    Log.notice(qos CR);
}

void onMqttUnsubscribe(uint16_t packetId)
{
    Log.notice("Unsubscribe acknowledged." CR);
    Log.notice("  packetId: ");
    Log.notice(packetId CR);
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{
    Log.notice("Publish received." CR);
    Log.notice("  topic: ");
    Log.notice(topic CR);
    Log.notice("  qos: ");
    Log.notice(properties.qos CR);
    Log.notice("  dup: ");
    Log.notice(properties.dup CR);
    Log.notice("  retain: ");
    Log.notice(properties.retain CR);
    Log.notice("  len: ");
    Log.notice(len CR);
    Log.notice("  index: ");
    Log.notice(index CR);
    Log.notice("  total: ");
    Log.notice(total CR);
}

void onMqttPublish(uint16_t packetId)
{
    Log.notice("Publish acknowledged." CR);
    Log.notice("  packetId: ");
    Log.notice(packetId CR);
}