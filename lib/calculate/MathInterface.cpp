#include "MathInterface.h"

MathInterface::MathInterface() {}

void calculateHeatEnergy()
{
    /*
    **
    **      Q = c * m * dT
    **      
    **      Q = Wärmeenergie                [Q]=J
    **      c = pezifische Wärmekapazität   [c]=J/(kg·K)
    **      m = Masse                       [m] = kg
    **      dT = Temperaturdifferenz        [T] = K
    */

    float c = 4200;
    float m = 5;
    float dT = 10;

    float Q = c * m * dT;
}

void MathInterface::avarage(struct MeterData *MeterData)
{
}
