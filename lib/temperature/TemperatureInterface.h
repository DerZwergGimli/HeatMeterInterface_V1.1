#ifndef TemperatureInterface_h
#define TemperatureInterface_h
#include <Adafruit_MAX31865.h>
#include "ShiftRegisterIO.h"

class TemperatureInterface
{
public:
    TemperatureInterface();
    void init(Adafruit_MAX31865 thermo);
    void readTemperature(Adafruit_MAX31865 thermo, SR_IO *sr_io, struct MeterData *meterData);
};
#endif