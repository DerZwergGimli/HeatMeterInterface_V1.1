#include "MQTT.h"
#include <ArduinoLog.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include "ConfigInterface.h"

//const char *WiFi_SSID = "IoT_Network ";
//const char *WiFi_Password = "I2mN39996";

#define WIFI_SSID "IoT_Network"
#define WIFI_PASSWORD "I2mN39996"

const char *MQTT_ServerAddress = "192.168.10.1";
const int MQTT_Port = 1883;

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

Configuration *configurationLocal;
MeterData *meterDataLocal;
HeaterData *heaterDataLocal;

int i = 0;
int messageCounter = 0;
int interfaceCounter = 0;
bool initSending = false;

bool DEBUG = false;

MQTT::MQTT()
{
}

void connectToWifi();
void onWifiConnect(const WiFiEventStationModeGotIP &event);
void onWifiDisconnect(const WiFiEventStationModeDisconnected &event);
void connectToMqtt();

void connectToWifi()
{
    Log.notice("Connecting to Wi-Fi..." CR);
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP &event)
{
    Log.notice("Connected to Wi-Fi." CR);
    connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected &event)
{
    Log.warning("Disconnected from Wi-Fi." CR);
    mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
    wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt()
{
    Log.notice("Connecting to MQTT..." CR);
    mqttClient.connect();
}

void onMqttConnect(bool sessionPresent)
{
    Log.notice("Connected to MQTT." CR);
    // Log.notice("Session present: ");
    // Log.notice("%i", sessionPresent);
    // Log.notice("" CR);
    // uint16_t packetIdSub = mqttClient.subscribe("test/lol", 2);
    // Log.notice("Subscribing at QoS 2, packetId: ");
    // Log.notice("%i", packetIdSub);
    // Log.notice("" CR);
    // mqttClient.publish("test/lol", 0, true, "test 1");
    // Log.notice("Publishing at QoS 0");
    // Log.notice("" CR);
    // uint16_t packetIdPub1 = mqttClient.publish("test/lol", 1, true, "test 2");
    // Log.notice("Publishing at QoS 1, packetId: ");
    // Log.notice("%i", packetIdPub1);
    // Log.notice("" CR);
    // uint16_t packetIdPub2 = mqttClient.publish("test/lol", 2, true, "test 3");
    // Log.notice("Publishing at QoS 2, packetId: ");
    // Log.notice("%i", packetIdPub2);
    // Log.notice("" CR);

    // bool reatin = false;
    // int qos = 1;
    // mqttClient.publish("test/lol", 2, true, "test 3");

    // char topic[50];
    // char message[20];
    // int message_counter = 0;
    // if (configurationLocal && meterDataLocal)
    // {
    //     for (size_t i = 0; i < 4; i++)
    //     {

    //         sprintf(topic, "%s/%d/water_CounterValue_m3", configurationLocal->name, meterDataLocal[i].meterID);
    //         sprintf(message, "%f", meterDataLocal[i].water_CounterValue_m3);
    //         message_counter = mqttClient.publish(topic, qos, reatin, message);

    //         sprintf(topic, "%s/%d/absolute_HeatEnergy_MWh", configurationLocal->name, meterDataLocal[i].meterID);
    //         sprintf(message, "%f", meterDataLocal[i].absolute_HeatEnergy_MWh);
    //         message_counter += mqttClient.publish(topic, qos, reatin, message);

    //         sprintf(topic, "%s/%d/temperature_up_Celcius", configurationLocal->name, meterDataLocal[i].meterID);
    //         sprintf(message, "%0.2f", meterDataLocal[i].temperature_up_Celcius);
    //         message_counter += mqttClient.publish(topic, qos, reatin, message);

    //         sprintf(topic, "%s/%d/temperature_down_Celcius", configurationLocal->name, meterDataLocal[i].meterID);
    //         sprintf(message, "%0.2f", meterDataLocal[i].temperature_down_Celcius);
    //         message_counter += mqttClient.publish(topic, qos, reatin, message);

    //         sprintf(topic, "%s/%d/state", configurationLocal->name, meterDataLocal[i].meterID);
    //         sprintf(message, "%i", meterDataLocal[i].ledState);
    //         message_counter += mqttClient.publish(topic, qos, reatin, message);
    //     }
    // }

    // if (configurationLocal)
    // {
    //     sprintf(topic, "%s/info/freeRam", configurationLocal->name);
    //     uint32_t freeRam = system_get_free_heap_size();
    //     sprintf(message, "%i", freeRam);
    //     message_counter += mqttClient.publish(topic, qos, reatin, message);
    // }

    // if (configurationLocal && heaterDataLocal)
    // {

    //     sprintf(topic, "%s/%s/state", configurationLocal->name, heaterDataLocal->name);
    //     sprintf(message, "%i", heaterDataLocal->state);
    //     message_counter += mqttClient.publish(topic, qos, reatin, message);

    //     sprintf(topic, "%s/%s/runtime_on_current", configurationLocal->name, heaterDataLocal->name);
    //     sprintf(message, "%li", heaterDataLocal->runtime_on_current);
    //     message_counter += mqttClient.publish(topic, qos, reatin, message);

    //     sprintf(topic, "%s/%s/runtime_off_current", configurationLocal->name, heaterDataLocal->name);
    //     sprintf(message, "%li", heaterDataLocal->runtime_off_current);
    //     message_counter += mqttClient.publish(topic, qos, reatin, message);

    //     sprintf(topic, "%s/%s/runtime_on_previous", configurationLocal->name, heaterDataLocal->name);
    //     sprintf(message, "%li", heaterDataLocal->runtime_on_previous);
    //     message_counter += mqttClient.publish(topic, qos, reatin, message);

    //     sprintf(topic, "%s/%s/runtime_off_previous", configurationLocal->name, heaterDataLocal->name);
    //     sprintf(message, "%li", heaterDataLocal->runtime_off_previous);
    //     message_counter += mqttClient.publish(topic, qos, reatin, message);
    // }
    // if (message_counter == 26)
    // {
    //     Log.notice("Published Messages" CR);
    // }
    // {
    //     Log.notice("Not all Messages SEND" CR);
    // }
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
    Log.notice("%i", packetId);
    Log.notice("" CR);
    Log.notice("  qos: ");
    Log.notice("%i", qos);
    Log.notice("" CR);
}

void onMqttUnsubscribe(uint16_t packetId)
{
    Log.notice("Unsubscribe acknowledged." CR);
    Log.notice("  packetId: ");
    Log.notice("%i", packetId);
    Log.notice("" CR);
}

void onMqttMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{

    Log.notice("Publish received." CR);
    Log.notice("  topic: ");
    Log.notice("%S", topic);
    Log.notice("" CR);
    Log.notice("  qos: ");
    Log.notice("%i", properties.qos);
    Log.notice("" CR);
    Log.notice("  dup: ");
    Log.notice("%i", properties.dup);
    Log.notice("" CR);
    Log.notice("  retain: ");
    Log.notice("%i", properties.retain);
    Log.notice("" CR);
    Log.notice("  len: ");
    Log.notice("%i", len);
    Log.notice("" CR);
    Log.notice("  index: ");
    Log.notice("%i", index);
    Log.notice("" CR);
    Log.notice("  total: ");
    Log.notice("%i", total);
    Log.notice("" CR);
}

void send()
{
    char topic[50];
    char message[30];
    bool reatin = false;
    int qos = 1;
    int messageSate;
    int message_returnValue;
    uint32_t myfreeRam = system_get_free_heap_size();
    if (configurationLocal && heaterDataLocal && meterDataLocal && initSending)
    {

        switch (messageCounter)
        {
        case 0:
            if (interfaceCounter <= 2)
            {
                sprintf(topic, "%s/%d/water_CounterValue_m3", configurationLocal->name, meterDataLocal[interfaceCounter].meterID);
                sprintf(message, "%s%i water_CounterValue_m3=%f", "Interface", meterDataLocal[interfaceCounter].meterID, meterDataLocal[interfaceCounter].water_CounterValue_m3);
                message_returnValue += mqttClient.publish(topic, qos, reatin, message);
                interfaceCounter++;
            }
            else
            {
                sprintf(topic, "%s/%d/water_CounterValue_m3", configurationLocal->name, meterDataLocal[interfaceCounter].meterID);
                sprintf(message, "%s%i water_CounterValue_m3=%f", "Interface", meterDataLocal[interfaceCounter].meterID, meterDataLocal[interfaceCounter].water_CounterValue_m3);
                message_returnValue += mqttClient.publish(topic, qos, reatin, message);
                messageCounter++;
                interfaceCounter = 0;
            }
            break;
        case 1:
            if (interfaceCounter <= 2)
            {
                sprintf(topic, "%s/%d/absolute_HeatEnergy_MWh", configurationLocal->name, meterDataLocal[interfaceCounter].meterID);
                sprintf(message, "%s%i absolute_HeatEnergy_MWh=%f", "Interface", meterDataLocal[interfaceCounter].meterID, meterDataLocal[interfaceCounter].absolute_HeatEnergy_MWh);
                message_returnValue += mqttClient.publish(topic, qos, reatin, message);
                interfaceCounter++;
            }
            else
            {
                sprintf(topic, "%s/%d/absolute_HeatEnergy_MWh", configurationLocal->name, meterDataLocal[interfaceCounter].meterID);
                sprintf(message, "%s%i absolute_HeatEnergy_MWh=%f", "Interface", meterDataLocal[interfaceCounter].meterID, meterDataLocal[interfaceCounter].absolute_HeatEnergy_MWh);
                message_returnValue += mqttClient.publish(topic, qos, reatin, message);
                messageCounter++;
                interfaceCounter = 0;
            }
            break;
        case 2:
            if (interfaceCounter <= 2)
            {
                sprintf(topic, "%s/%d/temperature_up_Celcius", configurationLocal->name, meterDataLocal[interfaceCounter].meterID);
                sprintf(message, "%s%i temperature_up_Celcius=%f", "Interface", meterDataLocal[interfaceCounter].meterID, meterDataLocal[interfaceCounter].temperature_up_Celcius);
                message_returnValue += mqttClient.publish(topic, qos, reatin, message);
                interfaceCounter++;
            }
            else
            {
                sprintf(topic, "%s/%d/temperature_up_Celcius", configurationLocal->name, meterDataLocal[interfaceCounter].meterID);
                sprintf(message, "%s%i temperature_up_Celcius=%f", "Interface", meterDataLocal[interfaceCounter].meterID, meterDataLocal[interfaceCounter].temperature_up_Celcius);
                message_returnValue += mqttClient.publish(topic, qos, reatin, message);
                messageCounter++;
                interfaceCounter = 0;
            }
            break;
        case 3:
            if (interfaceCounter <= 2)
            {
                sprintf(topic, "%s/%d/temperature_down_Celcius", configurationLocal->name, meterDataLocal[interfaceCounter].meterID);
                sprintf(message, "%s%i temperature_down_Celcius=%f", "Interface", meterDataLocal[interfaceCounter].meterID, meterDataLocal[interfaceCounter].temperature_down_Celcius);
                message_returnValue += mqttClient.publish(topic, qos, reatin, message);
                interfaceCounter++;
            }
            else
            {
                sprintf(topic, "%s/%d/temperature_down_Celcius", configurationLocal->name, meterDataLocal[interfaceCounter].meterID);
                sprintf(message, "%s%i temperature_down_Celcius=%f", "Interface", meterDataLocal[interfaceCounter].meterID, meterDataLocal[interfaceCounter].temperature_down_Celcius);
                message_returnValue += mqttClient.publish(topic, qos, reatin, message);
                messageCounter++;
                interfaceCounter = 0;
            }
            break;
        case 4:
            if (interfaceCounter <= 2)
            {
                sprintf(topic, "%s/%d/state", configurationLocal->name, meterDataLocal[interfaceCounter].meterID);
                sprintf(message, "%s%i state=%i", "Interface", meterDataLocal[interfaceCounter].meterID, meterDataLocal[interfaceCounter].waterMeterState);
                message_returnValue += mqttClient.publish(topic, qos, reatin, message);
                interfaceCounter++;
            }
            else
            {
                sprintf(topic, "%s/%d/state", configurationLocal->name, meterDataLocal[interfaceCounter].meterID);
                sprintf(message, "%s%i state=%i", "Interface", meterDataLocal[interfaceCounter].meterID, meterDataLocal[interfaceCounter].waterMeterState);
                message_returnValue += mqttClient.publish(topic, qos, reatin, message);
                messageCounter = 10;
                interfaceCounter = 0;
            }
            break;
        case 10:
            sprintf(topic, "%s/%s/state", configurationLocal->name, heaterDataLocal->name);
            sprintf(message, "%s state=%i", heaterDataLocal->name, heaterDataLocal->state);
            message_returnValue += mqttClient.publish(topic, qos, reatin, message);
            messageCounter++;
            break;
        case 11:
            sprintf(topic, "%s/%s/runtime_on_current", configurationLocal->name, heaterDataLocal->name);
            sprintf(message, "%s runtime_on_current=%li", "Heater", heaterDataLocal->runtime_on_current);
            message_returnValue += mqttClient.publish(topic, qos, reatin, message);
            messageCounter++;
            break;
        case 12:
            sprintf(topic, "%s/%s/runtime_off_current", configurationLocal->name, heaterDataLocal->name);
            sprintf(message, "%s runtime_off_current=%li", "Heater", heaterDataLocal->runtime_off_current);
            message_returnValue += mqttClient.publish(topic, qos, reatin, message);
            messageCounter++;
            break;
        case 13:
            sprintf(topic, "%s/%s/runtime_on_previous", configurationLocal->name, heaterDataLocal->name);
            sprintf(message, "%s runtime_on_previous=%li", "Heater", heaterDataLocal->runtime_on_previous);
            message_returnValue += mqttClient.publish(topic, qos, reatin, message);
            messageCounter++;
            break;
        case 14:
            printf(topic, "%s/%s/runtime_of_previous", configurationLocal->name, heaterDataLocal->name);
            sprintf(message, "%s runtime_of_previous=%li", "Heater", heaterDataLocal->runtime_off_previous);
            message_returnValue += mqttClient.publish(topic, qos, reatin, message);
            messageCounter = 20;
            break;
        case 20:
            sprintf(topic, "%s/info/freeRam", configurationLocal->name);
            sprintf(message, "%s freeRam=%i", "System", myfreeRam);
            message_returnValue += mqttClient.publish(topic, qos, reatin, message);
            messageCounter = 0;
            initSending = false;
            break;
        default:
            messageCounter = 0;
            initSending = false;
            break;
        }
    }
    else
    {
        Log.error("Data for MQTT is not init" CR);
        initSending = false;
        messageCounter = 0;
    }
    Log.notice("messageCounter=%i" CR, messageCounter);
    Log.notice("interfaceCounter=%i" CR, messageCounter);
}
void onMqttPublish(uint16_t packetId)
{

    if (DEBUG)
    {
        Log.notice("Publish acknowledged.");
        Log.notice("  packetId: ");
        Log.notice("%i", packetId);
        Log.notice("" CR);
    }

    //Call again other function
    if (initSending)
    {
        send();
    }
}
void MQTT::init()
{

    wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
    wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);
    //mqttClient.setMaxTopicLength(128);
    mqttClient.onConnect(onMqttConnect);
    mqttClient.onDisconnect(onMqttDisconnect);
    //mqttClient.onSubscribe(onMqttSubscribe);
    //mqttClient.onUnsubscribe(onMqttUnsubscribe);
    //mqttClient.onMessage(onMqttMessage);
    mqttClient.onPublish(onMqttPublish);
    mqttClient.setServer(MQTT_ServerAddress, MQTT_Port);

    connectToWifi();
}

void MQTT::send(struct ShiftRegisterIO *shiftRegister_io, struct Configuration *config, struct MeterData *meterData, struct HeaterData *heaterData)
{

    if (WiFi.isConnected())
    {
        shiftRegister_io->led_ERROR(false);
        if (mqttClient.connected())
        {
            shiftRegister_io->led_WIFI(true);

            this->config = config;
            configurationLocal = config;
            meterDataLocal = meterData;
            heaterDataLocal = heaterData;

            char topic[50];
            char message[20];
            bool reatin = false;
            int qos = 1;
            int messageSate;

            sprintf(topic, "%s/init", configurationLocal->name);
            sprintf(message, "%i", 1);
            mqttClient.publish(topic, qos, reatin, message);
            initSending = true;

            //onMqttConnect(true);

            shiftRegister_io->led_WIFI(false);
        }
    }
    else
    {
        shiftRegister_io->led_ERROR(true);
    }
}
