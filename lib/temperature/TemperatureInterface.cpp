#include "TemperatureInterface.h"
#include <Adafruit_MAX31865.h>
#include "ShiftRegisterIO.h"
#include "ConfigInterface.h"
#include "MAX31865.h"
#include <SPI.h>

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
    shiftRegisterIO->t_MuxSelect(meterData[0].mux_up);
    shiftRegisterIO->write();

    uint8_t fault = thermo.readFault();
    if (fault)
    {
        Serial.print("Fault 0x");
        Serial.println(fault, HEX);
        if (fault & MAX31865_FAULT_HIGHTHRESH)
        {
            Serial.println("RTD High Threshold");
        }
        if (fault & MAX31865_FAULT_LOWTHRESH)
        {
            Serial.println("RTD Low Threshold");
        }
        if (fault & MAX31865_FAULT_REFINLOW)
        {
            Serial.println("REFIN- > 0.85 x Bias");
        }
        if (fault & MAX31865_FAULT_REFINHIGH)
        {
            Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
        }
        if (fault & MAX31865_FAULT_RTDINLOW)
        {
            Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
        }
        if (fault & MAX31865_FAULT_OVUV)
        {
            Serial.println("Under/Over voltage");
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
        Serial.print("Fault 0x");
        Serial.println(fault, HEX);
        if (fault & MAX31865_FAULT_HIGHTHRESH)
        {
            Serial.println("RTD High Threshold");
        }
        if (fault & MAX31865_FAULT_LOWTHRESH)
        {
            Serial.println("RTD Low Threshold");
        }
        if (fault & MAX31865_FAULT_REFINLOW)
        {
            Serial.println("REFIN- > 0.85 x Bias");
        }
        if (fault & MAX31865_FAULT_REFINHIGH)
        {
            Serial.println("REFIN- < 0.85 x Bias - FORCE- open");
        }
        if (fault & MAX31865_FAULT_RTDINLOW)
        {
            Serial.println("RTDIN- < 0.85 x Bias - FORCE- open");
        }
        if (fault & MAX31865_FAULT_OVUV)
        {
            Serial.println("Under/Over voltage");
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