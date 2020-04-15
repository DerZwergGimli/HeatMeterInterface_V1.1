#ifndef ShiftRegisterIO_h
#define ShiftRegisterIO_h
#include "ConfigInterface.h"

struct SR_IO
{
    bool PT100Reader_Enable = false;
    bool TMuxS0 = false;
    bool TMuxS1 = false;
    bool TMuxS2 = false;
    bool RMuxS2 = false;
    bool RMuxS1 = false;
    bool RMuxS0 = false;
    bool Distance_Enable = false;

    bool LED_Wifi = false;
    bool LED_Error = false;
    bool LED_Ready = false;
    bool LED_RJ4 = false;
    bool LED_RJ3 = false;
    bool LED_RJ2 = false;
    bool LED_RJ1 = false;
    bool OLED_Enbable = false;

    bool RJ4_Counter = false; // Keep this mostly true using pnp-transostor
    bool RJ4_Status = false;
    bool RJ3_Counter = false; // Keep this mostly true using pnp-transostor
    bool RJ3_Status = false;
    bool RJ2_Counter = false; // Keep this mostly true using pnp-transostor
    bool RJ2_Status = false;
    bool RJ1_Counter = false; // Keep this mostly true using pnp-transostor
    bool RJ1_Status = false;
};

class ShiftRegisterIO
{
public:
    //Constructor
    ShiftRegisterIO();

    //Methodes
    void init();
    void allOne();
    void allZero();

    void ledBlink(int time);
    void r_MuxSelect(int channel);
    void t_MuxSelect(int channel);
    void write();

    // Measuremnets
    void checkMeterResistance(struct MeterData *meterData);
    void checkForVoltage(struct HeaterData *heaterData);
    String checkButton(String name);

    // Enable/Disbale Tasks
    void toggleDisplay(bool toggle);

    // HW Counter
    void counter_RJX(int x, bool toggle);

    // LED IO_Config
    void led_ERROR(bool toggle);
    void led_READY(bool toggle);
    void led_WIFI(bool toggle);
    void led_RJ1(bool toggle);
    void led_RJ2(bool toggle);
    void led_RJ3(bool toggle);
    void led_RJ4(bool toggle);
    void led_statusRJ1(bool toggle);
    void led_statusRJ2(bool toggle);
    void led_statusRJ3(bool toggle);
    void led_statusRJ4(bool toggle);
};
#endif