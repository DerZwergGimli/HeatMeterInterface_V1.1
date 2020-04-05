#ifndef ConfigInterface_h
#define ConfigInterface_h

#include "ExpSmooth.h"

enum ButtonState
{
    none,
    select,
    up,
    down
};

struct Configuration
{
    int device_ID = 0;
    const char *device_Name = "HeatMeterInterface";
    const char *wifi_SSID = "IoT_Network";
    const char *wifi_Password = "I2mN39996";
    const char *mqtt_Server = "192.168.10.1";
    int mqtt_Port = 1883;
    int task_Display_Interval = 0;
    int task_Button_Interval = 0;
    int task_Temperature_Interval = 0;
    int task_Resistance_Interval = 0;
    int task_Voltage_Interval = 0;
    int task_MQTT_Interval = 0;
};

struct HeaterData
{
    int heater_ID = 0;
    const char *heater_Name = "HeaterData";
    int mux_select = 3;
    int threshold_Analaog = 900;
    long runtime_on_current = 0;
    long runtime_off_current = 0;
    long runtime_on_previous = 0;
    long runtime_off_previous = 0;
    int counter_times_on;
    int counter_times_off;

    bool state = 0;
    bool pos_edge;
    bool neg_edge;

    int value_Analog;
    long runtime_on_start = 0;
    long runtime_off_start = 0;
};

struct MeterData
{
    int meter_ID;
    const char *meter_name;
    float RREF_up;
    float RREF_down;
    int mux_up;
    int mux_down;
    int mux_resistance;
    int mux_resistance_threshold;
    double counterValue_m3;
    double counterValue_J;
    double counterValue_MWh;

    bool waterMeterState;
    long hardware_CounterValue;
    float temperature_up_Celcius;
    ExpSmooth temperature_up_Celcius_smooth;
    float temperature_up_Celcius_mean;
    float temperature_up_Celcius_sum;
    int temperature_up_Celcius_numberOfPoints;
    float temperature_down_Celcius;
    ExpSmooth temperature_down_Celcius_smooth;
    float temperature_down_Celcius_mean;
    float temperature_down_Celcius_sum;
    int temperature_down_Celcius_numberOfPoints;

    //float mass_kg;
    //float specific_HeatCapacity_JpKGtK;
    float deltaTemperature_K;
    int mux_resistance_value;
    bool mux_resistance_edgeDetect;
};

class ConfigInterface
{
public:
    ConfigInterface();

    bool init();
    bool loadConfig(struct Configuration *config, MeterData (&meterData)[4], struct HeaterData *heaterData);
    bool saveConfig(Configuration *config, MeterData (&meterData)[4], struct HeaterData *heaterData);
    void serialPrintConfig(Configuration config);
};

#endif