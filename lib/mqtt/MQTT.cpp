#include "MQTT.h"
#include <ArduinoLog.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include "ConfigInterface.h"

#define WiFi_SSID "IoT_Network"
#define WiFi_Password "I2mN39996"

//String WIFI_SSID;
//String WIFI_PASSWORD;
#define MQTT_HOST IPAddress(192, 168, 10, 1)
#define MQTT_PORT 1883

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

Configuration *configurationLocal;
MeterData *p_meterData;
HeaterData *p_heaterData;

MeterData meterDataOld[4];
HeaterData heaterDataOld;

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
    WiFi.setAutoReconnect(false);
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
    WiFi.mode(WIFI_STA);
    WiFi.setOutputPower(20);
    WiFi.hostname("HeatMeterInterface");
    WiFi.disconnect();
    WiFi.begin(WiFi_SSID, WiFi_Password);
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
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason)
{
    Log.warning("Disconnected from MQTT (Reason %d)." CR, reason);

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

    bool retain = false;
    int qos = 1;
    int packageID = -1;
    uint32_t myfreeRam = system_get_free_heap_size();

    //Backup data to send  only on change;

    if (configurationLocal && p_heaterData && p_meterData && initSending)
    {
        String topic;
        String message;
        bool sendingLoopDone = false;

        while (!sendingLoopDone)
        {
            switch (messageCounter)
            {
            case 0:
                topic = configurationLocal->device_name + "/info/freeRam";
                message = "System freeRam=" + String(myfreeRam);
                packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                sendingLoopDone = true;
                messageCounter = 10;
                break;

            case 10:
                if (meterDataOld[interfaceCounter].counterValue_MWh != p_meterData[interfaceCounter].counterValue_MWh)
                {
                    topic = configurationLocal->device_name + "/" + String(p_meterData[interfaceCounter].meter_ID) + "/counterValue_MWh";
                    message = String("Interface") + String(p_meterData[interfaceCounter].meter_ID) + String(" counterValue_MWh=") + String(p_meterData[interfaceCounter].counterValue_MWh, 15);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                if (interfaceCounter >= 3)
                    messageCounter = 11;
                interfaceCounter++;
                break;
            case 11:
                if (meterDataOld[interfaceCounter].counterValue_m3 != p_meterData[interfaceCounter].counterValue_m3)
                {
                    topic = configurationLocal->device_name + "/" + String(p_meterData[interfaceCounter].meter_ID) + "/counterValue_m3";
                    message = String("Interface") + String(p_meterData[interfaceCounter].meter_ID) + String(" counterValue_m3=") + String(p_meterData[interfaceCounter].counterValue_m3, 15);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                if (interfaceCounter >= 3)
                    messageCounter = 12;
                interfaceCounter++;
                break;
            case 12:
                if (meterDataOld[interfaceCounter].temperature_up_Celcius != p_meterData[interfaceCounter].temperature_up_Celcius)
                {
                    topic = configurationLocal->device_name + "/" + String(p_meterData[interfaceCounter].meter_ID) + "/temperature_up_Celcius";
                    message = String("Interface") + String(p_meterData[interfaceCounter].meter_ID) + String(" temperature_up_Celcius=") + String(p_meterData[interfaceCounter].temperature_up_Celcius);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                if (interfaceCounter >= 3)
                    messageCounter = 13;
                interfaceCounter++;
                break;
            case 13:
                if (meterDataOld[interfaceCounter].temperature_down_Celcius != p_meterData[interfaceCounter].temperature_down_Celcius)
                {
                    topic = configurationLocal->device_name + "/" + String(p_meterData[interfaceCounter].meter_ID) + "/temperature_down_Celcius";
                    message = String("Interface") + String(p_meterData[interfaceCounter].meter_ID) + String(" temperature_down_Celcius=") + String(p_meterData[interfaceCounter].temperature_down_Celcius);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                if (interfaceCounter >= 3)
                    messageCounter = 14;
                interfaceCounter++;
                break;
            case 14:
                if (meterDataOld[interfaceCounter].waterMeterState != p_meterData[interfaceCounter].waterMeterState)
                {
                    topic = configurationLocal->device_name + "/" + String(p_meterData[interfaceCounter].meter_ID) + "/waterMeterState";
                    message = String("Interface") + String(p_meterData[interfaceCounter].meter_ID) + String(" waterMeterState=") + String(p_meterData[interfaceCounter].waterMeterState);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                if (interfaceCounter >= 3)
                    messageCounter = 15;
                interfaceCounter++;
                break;
            case 15:
                if (meterDataOld[interfaceCounter].deltaTemperature_K != p_meterData[interfaceCounter].deltaTemperature_K)
                {
                    topic = configurationLocal->device_name + "/" + String(p_meterData[interfaceCounter].meter_ID) + "/deltaTemperature_K";
                    message = String("Interface") + String(p_meterData[interfaceCounter].meter_ID) + String(" deltaTemperature_K=") + String(p_meterData[interfaceCounter].deltaTemperature_K);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                if (interfaceCounter >= 3)
                    messageCounter = 16;
                interfaceCounter++;
                break;
            case 16:
                if (meterDataOld[interfaceCounter].counterValue_m3_delta != p_meterData[interfaceCounter].counterValue_m3_delta)
                {
                    topic = configurationLocal->device_name + "/" + String(p_meterData[interfaceCounter].meter_ID) + "/counterValue_m3_delta";
                    message = String("Interface") + String(p_meterData[interfaceCounter].meter_ID) + String(" counterValue_m3_delta=") + String(p_meterData[interfaceCounter].counterValue_m3_delta, 15);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                    p_meterData[interfaceCounter].counterValue_m3_delta = 0;
                }
                if (interfaceCounter >= 3)
                    messageCounter = 17;
                interfaceCounter++;
                break;
            case 17:
                if (meterDataOld[interfaceCounter].counterValue_J_delta != p_meterData[interfaceCounter].counterValue_J_delta)
                {
                    topic = configurationLocal->device_name + "/" + String(p_meterData[interfaceCounter].meter_ID) + "/counterValue_J_delta";
                    message = String("Interface") + String(p_meterData[interfaceCounter].meter_ID) + String(" counterValue_J_delta=") + String(p_meterData[interfaceCounter].counterValue_J_delta, 15);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    p_meterData[interfaceCounter].counterValue_J_delta = 0;
                    sendingLoopDone = true;
                }
                if (interfaceCounter >= 3)
                    messageCounter = 18;
                interfaceCounter++;
                break;
            case 18:
                if (meterDataOld[interfaceCounter].counterValue_MWh_delta != p_meterData[interfaceCounter].counterValue_MWh_delta)
                {
                    topic = configurationLocal->device_name + "/" + String(p_meterData[interfaceCounter].meter_ID) + "/counterValue_MWh_delta";
                    message = String("Interface") + String(p_meterData[interfaceCounter].meter_ID) + String(" counterValue_MWh_delta=") + String(p_meterData[interfaceCounter].counterValue_MWh_delta, 15);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    p_meterData[interfaceCounter].counterValue_MWh_delta = 0;
                    sendingLoopDone = true;
                }
                if (interfaceCounter >= 3)
                    messageCounter = 20;
                interfaceCounter++;
                break;
            case 20:
                if (heaterDataOld.state != p_heaterData->state)
                {
                    topic = configurationLocal->device_name + "/" + String(p_heaterData->heater_name) + "/state";
                    message = String("Heater") + String(p_heaterData->heater_ID) + String(" state=") + String(p_heaterData->state);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                messageCounter = 21;
                break;
            case 21:
                if (heaterDataOld.counter_times_on != p_heaterData->counter_times_on)
                {
                    topic = configurationLocal->device_name + "/" + String(p_heaterData->heater_name) + "/counter_times_on";
                    message = String("Heater") + String(p_heaterData->heater_ID) + String(" counter_times_on=") + String(p_heaterData->counter_times_on);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                messageCounter = 22;
                break;
            case 22:
                if (heaterDataOld.counter_times_off != p_heaterData->counter_times_off)
                {
                    topic = configurationLocal->device_name + "/" + String(p_heaterData->heater_name) + "/counter_times_off";
                    message = String("Heater") + String(p_heaterData->heater_ID) + String(" counter_times_off=") + String(p_heaterData->counter_times_off);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                messageCounter = 23;
                break;
            case 23:
                if (heaterDataOld.runtime_on_current != p_heaterData->runtime_on_current)
                {
                    topic = configurationLocal->device_name + "/" + String(p_heaterData->heater_name) + "/runtime_on_current_ms";
                    message = String("Heater") + String(p_heaterData->heater_ID) + String(" runtime_on_current_ms=") + String(p_heaterData->runtime_on_current);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                messageCounter = 24;
                break;
            case 24:
                if (heaterDataOld.runtime_off_current != p_heaterData->runtime_off_current)
                {
                    topic = configurationLocal->device_name + "/" + String(p_heaterData->heater_name) + "/runtime_off_current_ms";
                    message = String("Heater") + String(p_heaterData->heater_ID) + String(" runtime_off_current_ms=") + String(p_heaterData->runtime_off_current);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                messageCounter = 25;
                break;
            case 25:
                if (heaterDataOld.runtime_on_previous != p_heaterData->runtime_on_previous)
                {
                    topic = configurationLocal->device_name + "/" + String(p_heaterData->heater_name) + "/runtime_on_previous_ms";
                    message = String("Heater") + String(p_heaterData->heater_ID) + String(" runtime_on_previous_ms=") + String(p_heaterData->runtime_on_previous);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                messageCounter = 26;
                break;
            case 26:
                if (heaterDataOld.runtime_off_previous != p_heaterData->runtime_off_previous)
                {
                    topic = configurationLocal->device_name + "/" + String(p_heaterData->heater_name) + "/runtime_off_previous_ms";
                    message = String("Heater") + String(p_heaterData->heater_ID) + String(" runtime_off_previous_ms=") + String(p_heaterData->runtime_off_previous);
                    packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
                    sendingLoopDone = true;
                }
                messageCounter = 100;
                break;
            case 100:
                sendingLoopDone = true;
                for (size_t i = 0; i < 4; i++)
                {
                    meterDataOld[i] = p_meterData[i];
                }
                heaterDataOld = *p_heaterData;
                messageCounter = -1;
                initSending = false;
                messageCounter = 0;
                break;
            default:
                sendingLoopDone = true;
                initSending = false;
                messageCounter = 0;
                break;
            }
            if (interfaceCounter >= 4)
                interfaceCounter = 0;
        }
        if (packageID > 0)
        {
            Log.notice("MQTT message send:%i" CR, packageID);
            Log.notice("messageCounter=%i" CR, messageCounter);
            Log.notice("interfaceCounter=%i" CR, interfaceCounter);
        }
    }
    else
    {
        Log.error("Data for MQTT is not init" CR);
        initSending = false;
        messageCounter = 0;
    }
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
    mqttClient.setServer(MQTT_HOST, MQTT_PORT);

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
            Log.notice("Send Data via MQTT" CR);

            configurationLocal = config;
            p_meterData = meterData;
            p_heaterData = heaterData;

            char topic[50];
            char message[20];
            bool reatin = false;
            int qos = 1;
            //TODO: IF init sensing is still true do some kind of retry sending
            initSending = true;
            sprintf(topic, "%s/init", configurationLocal->device_Name);
            sprintf(message, "System Alive=%i", 1);
            mqttClient.publish(topic, qos, reatin, message);

            //onMqttConnect(true);

            shiftRegister_io->led_WIFI(false);
        }
    }
    else
    {
        shiftRegister_io->led_ERROR(true);
    }
}
