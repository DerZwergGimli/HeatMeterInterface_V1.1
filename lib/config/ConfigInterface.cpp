#include "ConfigInterface.h"
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include "FS.h"

ConfigInterface ::ConfigInterface() {}

bool ConfigInterface::init()
{
    Log.notice("Mounting FileSystem" CR);

    if (!SPIFFS.begin())
    {
        Log.fatal("Failed to mount file system");
        return false;
    }
    return true;
}

bool ConfigInterface::loadConfig(struct Configuration *config, struct MeterData (&meterData)[4], struct HeaterData *heaterData)
{
    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile)
    {
        Log.fatal("Failed to open config file");
        return false;
    }

    int size = configFile.size();
    if (size > (JSON_OBJECT_SIZE(66) + 1460))
    {
        Log.error("Config file size is too large");
        return false;
    }

    // Allocate a buffer to store contents of the file.
    std::unique_ptr<char[]> buf(new char[size]);

    // We don't use String here because ArduinoJson library requires the input
    // buffer to be mutable. If you don't use ArduinoJson, you may as well
    // use configFile.readString instead.
    configFile.readBytes(buf.get(), size);

    //StaticJsonDocument<2366> doc;
    const size_t capacity = JSON_OBJECT_SIZE(66) + 1460;
    DynamicJsonDocument doc(capacity);
    auto error = deserializeJson(doc, buf.get());
    if (error)
    {
        Log.fatal("Failed to parse config file");
        return config;
    }

    config->device_name = doc["Device_Name"].as<String>();
    config->device_ID = doc["Device_ID"];
    config->wifi_SSID = doc["WIFI_SSID"];
    config->wifi_Password = doc["WIFI_PASSWORD"];
    config->mqtt_Server = doc["MQTT_Server"];
    config->mqtt_Port = doc["MQTT_ServerPort"];
    config->task_Display_Interval = doc["Task_Display_Interval"];
    config->task_Button_Interval = doc["Task_Button_Interval"];
    config->task_Temperature_Interval = doc["Task_Temperature_Interval"];
    config->task_Resistance_Interval = doc["Task_Resistance_Interval"];
    config->task_Voltage_Interval = doc["Task_Voltage_Interval"];
    config->task_MQTT_Interval = doc["Task_MQTT_Interval"];

    meterData[0].meter_ID = doc["Meter_1_ID"];
    meterData[0].meter_name = doc["Meter_1_Name"];
    meterData[0].RREF_up = doc["Meter_1_RREF_T1"];
    meterData[0].RREF_down = doc["Meter_1_RREF_T2"];
    meterData[0].mux_up = doc["Meter_1_MUX_T1"];
    meterData[0].mux_down = doc["Meter_1_MUX_T2"];
    meterData[0].mux_resistance = doc["Meter_1_MUX_R"];
    meterData[0].mux_resistance_threshold = doc["Meter_1_MUX_R_Threshold"];
    meterData[0].counterValue_m3 = doc["Meter_1_CounterValue_m3"];
    meterData[0].counterValue_J = doc["Meter_1_CounterValue_J"];
    meterData[0].counterValue_MWh = doc["Meter_1_CounterValue_MWh"];

    meterData[1].meter_ID = doc["Meter_2_ID"];
    meterData[1].meter_name = doc["Meter_2_Name"];
    meterData[1].RREF_up = doc["Meter_2_RREF_T1"];
    meterData[1].RREF_down = doc["Meter_2_RREF_T2"];
    meterData[1].mux_up = doc["Meter_2_MUX_T1"];
    meterData[1].mux_down = doc["Meter_2_MUX_T2"];
    meterData[1].mux_resistance = doc["Meter_2_MUX_R"];
    meterData[1].mux_resistance_threshold = doc["Meter_2_MUX_R_Threshold"];
    meterData[1].counterValue_m3 = doc["Meter_2_CounterValue_m3"];
    meterData[1].counterValue_J = doc["Meter_2_CounterValue_J"];
    meterData[1].counterValue_MWh = doc["Meter_2_CounterValue_MWh"];

    meterData[2].meter_ID = doc["Meter_3_ID"];
    meterData[2].meter_name = doc["Meter_3_Name"];
    meterData[2].RREF_up = doc["Meter_3_RREF_T1"];
    meterData[2].RREF_down = doc["Meter_3_RREF_T2"];
    meterData[2].mux_up = doc["Meter_3_MUX_T1"];
    meterData[2].mux_down = doc["Meter_3_MUX_T2"];
    meterData[2].mux_resistance = doc["Meter_3_MUX_R"];
    meterData[2].mux_resistance_threshold = doc["Meter_3_MUX_R_Threshold"];
    meterData[2].counterValue_m3 = doc["Meter_3_CounterValue_m3"];
    meterData[2].counterValue_J = doc["Meter_3_CounterValue_J"];
    meterData[2].counterValue_MWh = doc["Meter_3_CounterValue_MWh"];

    meterData[3].meter_ID = doc["Meter_4_ID"];
    meterData[3].meter_name = doc["Meter_4_Name"];
    meterData[3].RREF_up = doc["Meter_4_RREF_T1"];
    meterData[3].RREF_down = doc["Meter_4_RREF_T2"];
    meterData[3].mux_up = doc["Meter_4_MUX_T1"];
    meterData[3].mux_down = doc["Meter_4_MUX_T2"];
    meterData[3].mux_resistance = doc["Meter_4_MUX_R"];
    meterData[3].mux_resistance_threshold = doc["Meter_4_MUX_R_Threshold"];
    meterData[3].counterValue_m3 = doc["Meter_4_CounterValue_m3"];
    meterData[3].counterValue_J = doc["Meter_4_CounterValue_J"];
    meterData[3].counterValue_MWh = doc["Meter_4_CounterValue_MWh"];

    heaterData->heater_ID = doc["Heater_ID"];
    heaterData->heater_Name = doc["Heater_Name"];
    heaterData->mux_select = doc["Heater_MUX_R"];
    heaterData->threshold_Analaog = doc["Heater_MUX_R_Threshold"];
    heaterData->runtime_on_current = doc["Heater_Runtime_On_Current"];
    heaterData->runtime_off_current = doc["Heater_Runtime_Off_Current"];
    heaterData->runtime_on_previous = doc["Heater_Runtime_On_Previous"];
    heaterData->runtime_off_previous = doc["Heater_Runtime_Off_Previous"];
    heaterData->counter_times_on = doc["Heater_Counter_Times_On"];
    heaterData->counter_times_off = doc["Heater_Counter_Times_Off"];

    Log.notice("Loaded Config File");
    return true;
}

bool ConfigInterface::saveConfig(struct Configuration *config, struct MeterData (&meterData)[4], struct HeaterData *heaterData)
{
    StaticJsonDocument<1072> doc;

    doc["Device_Name"] = config->device_Name;
    doc["Device_ID"] = config->device_ID;
    doc["WIFI_SSID"] = config->wifi_SSID;
    doc["WIFI_PASSWORD"] = config->wifi_Password;
    doc["MQTT_Server"] = config->mqtt_Server;
    doc["MQTT_ServerPort"] = config->mqtt_Port;
    doc["Task_Display_Interval"] = config->task_Display_Interval;
    doc["Task_Button_Interval"] = config->task_Button_Interval;
    doc["Task_Temperature_Interval"] = config->task_Temperature_Interval;
    doc["Task_Resistance_Interval"] = config->task_Resistance_Interval;
    doc["Task_Voltage_Interval"] = config->task_Voltage_Interval;
    doc["Task_MQTT_Interval"] = config->task_MQTT_Interval;

    doc["Meter_1_ID"] = meterData[0].meter_ID;
    doc["Meter_1_Name"] = meterData[0].meter_name;
    doc["Meter_1_RREF_T1"] = meterData[0].RREF_up;
    doc["Meter_1_RREF_T2"] = meterData[0].RREF_down;
    doc["Meter_1_MUX_T1"] = meterData[0].mux_up;
    doc["Meter_1_MUX_T2"] = meterData[0].mux_down;
    doc["Meter_1_MUX_R"] = meterData[0].mux_resistance;
    doc["Meter_1_MUX_R_Threshold"] = meterData[0].mux_resistance_threshold;
    doc["Meter_1_CounterValue_m3"] = meterData[0].counterValue_m3;
    doc["Meter_1_CounterValue_J"] = meterData[0].counterValue_J;
    doc["Meter_1_CounterValue_MWh"] = meterData[0].counterValue_MWh;

    doc["Meter_2_ID"] = meterData[1].meter_ID;
    doc["Meter_2_Name"] = meterData[1].meter_name;
    doc["Meter_2_RREF_T1"] = meterData[1].RREF_up;
    doc["Meter_2_RREF_T2"] = meterData[1].RREF_down;
    doc["Meter_2_MUX_T1"] = meterData[1].mux_up;
    doc["Meter_2_MUX_T2"] = meterData[1].mux_down;
    doc["Meter_2_MUX_R"] = meterData[1].mux_resistance;
    doc["Meter_2_MUX_R_Threshold"] = meterData[1].mux_resistance_threshold;
    doc["Meter_2_CounterValue_m3"] = meterData[1].counterValue_m3;
    doc["Meter_2_CounterValue_J"] = meterData[1].counterValue_J;
    doc["Meter_2_CounterValue_MWh"] = meterData[1].counterValue_MWh;

    doc["Meter_3_ID"] = meterData[2].meter_ID;
    doc["Meter_3_Name"] = meterData[2].meter_name;
    doc["Meter_3_RREF_T1"] = meterData[2].RREF_up;
    doc["Meter_3_RREF_T2"] = meterData[2].RREF_down;
    doc["Meter_3_MUX_T1"] = meterData[2].mux_up;
    doc["Meter_3_MUX_T2"] = meterData[2].mux_down;
    doc["Meter_3_MUX_R"] = meterData[2].mux_resistance;
    doc["Meter_3_MUX_R_Threshold"] = meterData[2].mux_resistance_threshold;
    doc["Meter_3_CounterValue_m3"] = meterData[2].counterValue_m3;
    doc["Meter_3_CounterValue_J"] = meterData[2].counterValue_J;
    doc["Meter_3_CounterValue_MWh"] = meterData[2].counterValue_MWh;

    doc["Meter_4_ID"] = meterData[3].meter_ID;
    doc["Meter_4_Name"] = meterData[3].meter_name;
    doc["Meter_4_RREF_T1"] = meterData[3].RREF_up;
    doc["Meter_4_RREF_T2"] = meterData[3].RREF_down;
    doc["Meter_4_MUX_T1"] = meterData[3].mux_up;
    doc["Meter_4_MUX_T2"] = meterData[3].mux_down;
    doc["Meter_4_MUX_R"] = meterData[3].mux_resistance;
    doc["Meter_4_MUX_R_Threshold"] = meterData[3].mux_resistance_threshold;
    doc["Meter_4_CounterValue_m3"] = meterData[3].counterValue_m3;
    doc["Meter_4_CounterValue_J"] = meterData[3].counterValue_J;
    doc["Meter_4_CounterValue_MWh"] = meterData[3].counterValue_MWh;

    doc["Heater_ID"] = heaterData->heater_ID;
    doc["Heater_Name"] = heaterData->heater_Name;
    doc["Heater_MUX_R"] = heaterData->mux_select;
    doc["Heater_MUX_R_Threshold"] = heaterData->threshold_Analaog;
    doc["Heater_Runtime_On_Current"] = heaterData->runtime_on_current;
    doc["Heater_Runtime_Off_Current"] = heaterData->runtime_off_current;
    doc["Heater_Runtime_On_Previous"] = heaterData->runtime_on_previous;
    doc["Heater_Runtime_Off_Previous"] = heaterData->runtime_off_previous;
    doc["Heater_Counter_Times_On"] = heaterData->counter_times_on;
    doc["Heater_Counter_Times_Off"] = heaterData->counter_times_off;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile)
    {
        Log.fatal("Failed to open config file for writing");
        return false;
    }

    serializeJson(doc, configFile);
    return true;
}

void ConfigInterface::serialPrintConfig(Configuration conf)
{
    Serial.print("Loaded ID: ");
    Serial.println(conf.device_ID);
    Serial.print("Loaded RREF_RJ1_T1: ");
    //Serial.println(conf.RREF_RJ1_T1);
    Serial.print("Loaded RREF_RJ1_T2: ");
    //Serial.println(conf.RREF_RJ1_T2);
    Serial.print("Loaded RREF_RJ2_T1: ");
    //Serial.println(conf.RREF_RJ2_T1);
    Serial.print("Loaded RREF_RJ2_T2: ");
    //Serial.println(conf.RREF_RJ2_T2);
    Serial.print("Loaded RREF_RJ3_T1: ");
    //Serial.println(conf.RREF_RJ3_T1);
    Serial.print("Loaded RREF_RJ3_T2: ");
    //Serial.println(conf.RREF_RJ3_T2);
    Serial.print("Loaded RREF_RJ4_T1: ");
    //Serial.println(conf.RREF_RJ4_T1);
    Serial.print("Loaded RREF_RJ4_T2: ");
    //Serial.println(conf.RREF_RJ4_T2);
}