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
MeterData *meterDataLocal;
HeaterData *heaterDataLocal;

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

    if (configurationLocal && heaterDataLocal && meterDataLocal && initSending)
    {
        String topic;
        String message;
        switch (messageCounter)
        {
        case 0:
            topic = configurationLocal->device_name + "/info/freeRam";
            message = "System freeRam=" + String(myfreeRam);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            messageCounter = 10;
            break;

        case 10:
            topic = configurationLocal->device_name + "/" + String(meterDataLocal[interfaceCounter].meter_ID) + "/counterValue_MWh";
            message = String("Interface") + String(meterDataLocal[interfaceCounter].meter_ID) + String(" counterValue_MWh=") + String(meterDataLocal[interfaceCounter].counterValue_MWh, 5);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            if (interfaceCounter >= 3)
                messageCounter = 11;
            interfaceCounter++;
            break;
        case 11:
            topic = configurationLocal->device_name + "/" + String(meterDataLocal[interfaceCounter].meter_ID) + "/counterValue_m3";
            message = String("Interface") + String(meterDataLocal[interfaceCounter].meter_ID) + String(" counterValue_m3=") + String(meterDataLocal[interfaceCounter].counterValue_m3, 5);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            if (interfaceCounter >= 3)
                messageCounter = 12;
            interfaceCounter++;
            break;

        case 12:
            topic = configurationLocal->device_name + "/" + String(meterDataLocal[interfaceCounter].meter_ID) + "/temperature_up_Celcius";
            message = String("Interface") + String(meterDataLocal[interfaceCounter].meter_ID) + String(" temperature_up_Celcius=") + String(meterDataLocal[interfaceCounter].temperature_up_Celcius);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            if (interfaceCounter >= 3)
                messageCounter = 13;
            interfaceCounter++;
            break;

        case 13:
            topic = configurationLocal->device_name + "/" + String(meterDataLocal[interfaceCounter].meter_ID) + "/temperature_down_Celcius";
            message = String("Interface") + String(meterDataLocal[interfaceCounter].meter_ID) + String(" temperature_down_Celcius=") + String(meterDataLocal[interfaceCounter].temperature_down_Celcius);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            if (interfaceCounter >= 3)
                messageCounter = 14;
            interfaceCounter++;
            break;

        case 14:
            topic = configurationLocal->device_name + "/" + String(meterDataLocal[interfaceCounter].meter_ID) + "/waterMeterState";
            message = String("Interface") + String(meterDataLocal[interfaceCounter].meter_ID) + String(" waterMeterState=") + String(meterDataLocal[interfaceCounter].waterMeterState);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            if (interfaceCounter >= 3)
                messageCounter = 15;
            interfaceCounter++;
            break;
        case 15:
            topic = configurationLocal->device_name + "/" + String(meterDataLocal[interfaceCounter].meter_ID) + "/deltaTemperature_K";
            message = String("Interface") + String(meterDataLocal[interfaceCounter].meter_ID) + String(" deltaTemperature_K=") + String(meterDataLocal[interfaceCounter].deltaTemperature_K);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            if (interfaceCounter >= 3)
                messageCounter = 20;
            interfaceCounter++;
            break;

        case 20:
            topic = configurationLocal->device_name + "/" + String(heaterDataLocal->heater_name) + "/state";
            message = String("Heater") + String(heaterDataLocal->heater_ID) + String(" state=") + String(heaterDataLocal->state);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            messageCounter = 21;
            break;
        case 21:
            topic = configurationLocal->device_name + "/" + String(heaterDataLocal->heater_name) + "/counter_times_on";
            message = String("Heater") + String(heaterDataLocal->heater_ID) + String(" counter_times_on=") + String(heaterDataLocal->counter_times_on);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            messageCounter = 22;
            break;
        case 22:
            topic = configurationLocal->device_name + "/" + String(heaterDataLocal->heater_name) + "/counter_times_off";
            message = String("Heater") + String(heaterDataLocal->heater_ID) + String(" counter_times_off=") + String(heaterDataLocal->counter_times_off);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            messageCounter = 23;
            break;
        case 23:
            topic = configurationLocal->device_name + "/" + String(heaterDataLocal->heater_name) + "/runtime_on_current";
            message = String("Heater") + String(heaterDataLocal->heater_ID) + String(" runtime_on_current=") + String(heaterDataLocal->runtime_on_current);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            messageCounter = 24;
            break;
        case 24:
            topic = configurationLocal->device_name + "/" + String(heaterDataLocal->heater_name) + "/runtime_off_current";
            message = String("Heater") + String(heaterDataLocal->heater_ID) + String(" runtime_off_current=") + String(heaterDataLocal->runtime_off_current);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            messageCounter = 25;
            break;
        case 25:
            topic = configurationLocal->device_name + "/" + String(heaterDataLocal->heater_name) + "/runtime_on_previous";
            message = String("Heater") + String(heaterDataLocal->heater_ID) + String(" runtime_on_previous=") + String(heaterDataLocal->runtime_on_previous);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            messageCounter = 26;
            break;
        case 26:
            topic = configurationLocal->device_name + "/" + String(heaterDataLocal->heater_name) + "/runtime_off_previous";
            message = String("Heater") + String(heaterDataLocal->heater_ID) + String(" runtime_off_previous=") + String(heaterDataLocal->runtime_off_previous);
            packageID = mqttClient.publish(topic.c_str(), qos, retain, message.c_str());
            messageCounter = -1;
            break;

        default:
            initSending = false;
            messageCounter = 0;
            break;
        }

        if (interfaceCounter >= 4)
            interfaceCounter = 0;

        if (packageID > 0)
        {
            Log.notice("MQTT message send:%i" CR, packageID);
            Log.notice("messageCounter=%i" CR, messageCounter);
            Log.notice("interfaceCounter=%i" CR, interfaceCounter);
        }

        //sprintf(topic, "%s/info/freeRam", configurationLocal->device_Name);
        //sprintf(message, "%s freeRam=%i", "System", myfreeRam);

        // message_returnValue = mqttClient.publish(topic, qos, reatin, message);

        // switch (messageCounter)
        // {
        // case 0:
        //     if (interfaceCounter < 3)
        //     {
        //         sprintf(topic, "%s/%d/water_CounterValue_m3", configurationLocal->device_Name, meterDataLocal[interfaceCounter].meter_ID);
        //         sprintf(message, "%s%i water_CounterValue_m3=%f", "Interface", meterDataLocal[interfaceCounter].meter_ID, meterDataLocal[interfaceCounter].counterValue_m3);
        //         interfaceCounter += mqttClient.publish(topic, qos, reatin, message);
        //     }
        //     else
        //     {
        //         sprintf(topic, "%s/%d/water_CounterValue_m3", configurationLocal->device_Name, meterDataLocal[interfaceCounter].meter_ID);
        //         sprintf(message, "%s%i water_CounterValue_m3=%f", "Interface", meterDataLocal[interfaceCounter].meter_ID, meterDataLocal[interfaceCounter].counterValue_m3);
        //         interfaceCounter = 0;
        //         messageCounter += mqttClient.publish(topic, qos, reatin, message);
        //     }
        //     break;
        // case 1:
        //     if (interfaceCounter < 3)
        //     {
        //         sprintf(topic, "%s/%d/absolute_HeatEnergy_MWh", configurationLocal->device_Name, meterDataLocal[interfaceCounter].meter_ID);
        //         sprintf(message, "%s%i absolute_HeatEnergy_MWh=%f", "Interface", meterDataLocal[interfaceCounter].meter_ID, meterDataLocal[interfaceCounter].counterValue_MWh);
        //         interfaceCounter += mqttClient.publish(topic, qos, reatin, message);
        //     }
        //     else
        //     {
        //         sprintf(topic, "%s/%d/absolute_HeatEnergy_MWh", configurationLocal->device_Name, meterDataLocal[interfaceCounter].meter_ID);
        //         sprintf(message, "%s%i absolute_HeatEnergy_MWh=%f", "Interface", meterDataLocal[interfaceCounter].meter_ID, meterDataLocal[interfaceCounter].counterValue_MWh);
        //         interfaceCounter = 0;
        //         messageCounter += mqttClient.publish(topic, qos, reatin, message);
        //     }
        //     break;
        // case 2:
        //     if (interfaceCounter < 3)
        //     {
        //         sprintf(topic, "%s/%d/temperature_up_Celcius", configurationLocal->device_Name, meterDataLocal[interfaceCounter].meter_ID);
        //         sprintf(message, "%s%i temperature_up_Celcius=%f", "Interface", meterDataLocal[interfaceCounter].meter_ID, meterDataLocal[interfaceCounter].temperature_up_Celcius);
        //         interfaceCounter += mqttClient.publish(topic, qos, reatin, message);
        //     }
        //     else
        //     {
        //         sprintf(topic, "%s/%d/temperature_up_Celcius", configurationLocal->device_Name, meterDataLocal[interfaceCounter].meter_ID);
        //         sprintf(message, "%s%i temperature_up_Celcius=%f", "Interface", meterDataLocal[interfaceCounter].meter_ID, meterDataLocal[interfaceCounter].temperature_up_Celcius);
        //         interfaceCounter = 0;
        //         messageCounter += mqttClient.publish(topic, qos, reatin, message);
        //     }
        //     break;
        // case 3:
        //     if (interfaceCounter < 3)
        //     {
        //         sprintf(topic, "%s/%d/temperature_down_Celcius", configurationLocal->device_Name, meterDataLocal[interfaceCounter].meter_ID);
        //         sprintf(message, "%s%i temperature_down_Celcius=%f", "Interface", meterDataLocal[interfaceCounter].meter_ID, meterDataLocal[interfaceCounter].temperature_down_Celcius);

        //         interfaceCounter += mqttClient.publish(topic, qos, reatin, message);
        //     }
        //     else
        //     {
        //         sprintf(topic, "%s/%d/temperature_down_Celcius", configurationLocal->device_Name, meterDataLocal[interfaceCounter].meter_ID);
        //         sprintf(message, "%s%i temperature_down_Celcius=%f", "Interface", meterDataLocal[interfaceCounter].meter_ID, meterDataLocal[interfaceCounter].temperature_down_Celcius);
        //         interfaceCounter = 0;
        //         messageCounter += mqttClient.publish(topic, qos, reatin, message);
        //     }
        //     break;
        // case 4:
        //     if (interfaceCounter < 3)
        //     {
        //         sprintf(topic, "%s/%d/state", configurationLocal->device_Name, meterDataLocal[interfaceCounter].meter_ID);
        //         sprintf(message, "%s%i state=%i", "Interface", meterDataLocal[interfaceCounter].meter_ID, meterDataLocal[interfaceCounter].waterMeterState);
        //         messageCounter += mqttClient.publish(topic, qos, reatin, message);
        //     }
        //     else
        //     {
        //         sprintf(topic, "%s/%d/state", configurationLocal->device_Name, meterDataLocal[interfaceCounter].meter_ID);
        //         sprintf(message, "%s%i state=%i", "Interface", meterDataLocal[interfaceCounter].meter_ID, meterDataLocal[interfaceCounter].waterMeterState);
        //         messageCounter = 10;
        //         interfaceCounter = 0;
        //         message_returnValue += mqttClient.publish(topic, qos, reatin, message);
        //     }
        //     break;
        // case 10:
        //     sprintf(topic, "%s/%s/state", configurationLocal->device_Name, heaterDataLocal->heater_Name);
        //     sprintf(message, "%s state=%i", heaterDataLocal->heater_Name, heaterDataLocal->state);
        //     messageCounter += mqttClient.publish(topic, qos, reatin, message);
        //     break;
        // case 11:
        //     sprintf(topic, "%s/%s/runtime_on_current", configurationLocal->device_Name, heaterDataLocal->heater_Name);
        //     sprintf(message, "%s runtime_on_current=%li", "Heater", heaterDataLocal->runtime_on_current);
        //     messageCounter += mqttClient.publish(topic, qos, reatin, message);
        //     break;
        // case 12:
        //     sprintf(topic, "%s/%s/runtime_off_current", configurationLocal->device_Name, heaterDataLocal->heater_Name);
        //     sprintf(message, "%s runtime_off_current=%li", "Heater", heaterDataLocal->runtime_off_current);
        //     messageCounter += mqttClient.publish(topic, qos, reatin, message);
        //     break;
        // case 13:
        //     sprintf(topic, "%s/%s/runtime_on_previous", configurationLocal->device_Name, heaterDataLocal->heater_Name);
        //     sprintf(message, "%s runtime_on_previous=%li", "Heater", heaterDataLocal->runtime_on_previous);
        //     messageCounter += mqttClient.publish(topic, qos, reatin, message);
        //     break;
        // case 14:
        //     printf(topic, "%s/%s/runtime_of_previous", configurationLocal->device_Name, heaterDataLocal->heater_Name);
        //     sprintf(message, "%s runtime_of_previous=%li", "Heater", heaterDataLocal->runtime_off_previous);
        //     messageCounter = 20;
        //     message_returnValue += mqttClient.publish(topic, qos, reatin, message);
        //     break;
        // case 20:
        //     sprintf(topic, "%s/info/freeRam", configurationLocal->device_Name);
        //     sprintf(message, "%s freeRam=%i", "System", myfreeRam);
        //     messageCounter = 0;
        //     initSending = false;
        //     message_returnValue += mqttClient.publish(topic, qos, reatin, message);
        //     break;
        // default:
        //     messageCounter = 0;
        //     initSending = false;
        //     break;
        // }
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
            meterDataLocal = meterData;
            heaterDataLocal = heaterData;

            char topic[50];
            char message[20];
            bool reatin = false;
            int qos = 1;
            int messageSate;

            sprintf(topic, "%s/init", configurationLocal->device_Name);
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
