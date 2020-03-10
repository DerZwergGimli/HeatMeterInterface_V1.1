#ifndef ConfigInterface_h
#define ConfigInterface_h

#include "ExpSmooth.h"

struct Configuratrion
{
    int ID;
    const char *name = "HeatMeterInterface";
    const char *wifi_SSID = "IoT_Network";
    const char *wifi_Password = "I2mN39996";
    const char *mqtt_ServerAddress = "192.168.10.1";
    const int mqtt_Port = 1883;
    const char *mqtt_Username;
    const char *mqtt_Password;
};

struct MeterData
{
    short meterID;
    bool waterMeterState;
    bool ledState;
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
    float water_CounterValue_m3;

    float delta_HeatEnergy_J;
    double absolute_HeatEnergy_MWh;
    //float mass_kg;
    //float specific_HeatCapacity_JpKGtK;
    float deltaTemperature_K;

    float RREF_up;
    float RREF_down;

    int mux_up;
    int mux_down;
    int mux_resistance;
    int mux_resistance_threshold;
    bool mux_resistance_edgeDetect;
};

class ConfigInterface
{
public:
    ConfigInterface();

    bool init();
    bool loadConfig(struct Configuratrion *config, MeterData (&meterData)[4]);
    bool saveConfig(Configuratrion config);
    void serialPrintConfig(Configuratrion config);
};

#endif