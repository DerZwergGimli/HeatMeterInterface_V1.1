#include "TemperatureInterface.h"
#include <Adafruit_MAX31865.h>
#include "ShiftRegisterIO.h"
#include "ConfigInterface.h"
#include "MAX31865.h"
#include <SPI.h>
#include <ArduinoLog.h>

TemperatureInterface::TemperatureInterface()
{
}

void TemperatureInterface::init(Adafruit_MAX31865 thermo)
{
    thermo.begin(MAX31865_2WIRE); // set to 2WIRE or 4WIRE as necessary
}

void TemperatureInterface::readTemperature(ShiftRegisterIO *shiftRegisterIO, Adafruit_MAX31865 thermo, struct MeterData *meterData)
{
    float RNOMINAL = 100.0;

    // UP
    shiftRegisterIO->t_MuxSelect(meterData->mux_up);
    shiftRegisterIO->write();

    uint8_t fault = thermo.readFault();
    if (fault)
    {
        Log.warning("Fault 0x");
        Log.warning("%h" CR, HEX);
        if (fault & MAX31865_FAULT_HIGHTHRESH)
        {
            Log.warning("RTD High Threshold" CR);
        }
        if (fault & MAX31865_FAULT_LOWTHRESH)
        {
            Log.warning("RTD Low Threshold" CR);
        }
        if (fault & MAX31865_FAULT_REFINLOW)
        {
            Log.warning("REFIN- > 0.85 x Bias" CR);
        }
        if (fault & MAX31865_FAULT_REFINHIGH)
        {
            Log.warning("REFIN- < 0.85 x Bias - FORCE- open" CR);
        }
        if (fault & MAX31865_FAULT_RTDINLOW)
        {
            Log.warning("RTDIN- < 0.85 x Bias - FORCE- open" CR);
        }
        if (fault & MAX31865_FAULT_OVUV)
        {
            Log.warning("Under/Over voltage" CR);
        }
        thermo.clearFault();
    }
    else
    {
        float temp_up = thermo.temperature(RNOMINAL, meterData->RREF_up);
        if (temp_up < 300)
        {
            if (meterData->temperature_up_Celcius_mean == 0)
            {
                meterData->temperature_up_Celcius_mean = temp_up;
                meterData->temperature_up_Celcius_sum = 0;
                meterData->temperature_up_Celcius_numberOfPoints = 0;
            }
            meterData->temperature_up_Celcius = temp_up;
            meterData->temperature_up_Celcius_sum += temp_up;
            meterData->temperature_up_Celcius_numberOfPoints++;
            meterData->temperature_up_Celcius_mean = meterData->temperature_up_Celcius_sum / meterData->temperature_up_Celcius_numberOfPoints;
        }
    }

    // DOWN
    shiftRegisterIO->t_MuxSelect(meterData->mux_down);
    shiftRegisterIO->write();

    fault = thermo.readFault();
    if (fault)
    {

        Log.warning("Fault 0x");
        Log.warning("%h" CR, HEX);
        if (fault & MAX31865_FAULT_HIGHTHRESH)
        {
            Log.warning("RTD High Threshold" CR);
        }
        if (fault & MAX31865_FAULT_LOWTHRESH)
        {
            Log.warning("RTD Low Threshold" CR);
        }
        if (fault & MAX31865_FAULT_REFINLOW)
        {
            Log.warning("REFIN- > 0.85 x Bias" CR);
        }
        if (fault & MAX31865_FAULT_REFINHIGH)
        {
            Log.warning("REFIN- < 0.85 x Bias - FORCE- open" CR);
        }
        if (fault & MAX31865_FAULT_RTDINLOW)
        {
            Log.warning("RTDIN- < 0.85 x Bias - FORCE- open" CR);
        }
        if (fault & MAX31865_FAULT_OVUV)
        {
            Log.warning("Under/Over voltage" CR);
        }
        thermo.clearFault();
    }
    else
    {
        float temp_down = thermo.temperature(RNOMINAL, meterData->RREF_down);
        if (temp_down < 300)
        {

            if (meterData->temperature_down_Celcius_mean == 0)
            {
                meterData->temperature_down_Celcius_mean = temp_down;
                meterData->temperature_down_Celcius_sum = 0;
                meterData->temperature_down_Celcius_numberOfPoints = 0;
            }

            meterData->temperature_down_Celcius = temp_down;
            meterData->temperature_down_Celcius_sum += temp_down;
            meterData->temperature_down_Celcius_numberOfPoints++;
            meterData->temperature_down_Celcius_mean = meterData->temperature_down_Celcius_sum / meterData->temperature_down_Celcius_numberOfPoints;
        }
    }
}