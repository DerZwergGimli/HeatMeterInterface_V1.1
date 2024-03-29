#include "ShiftRegisterIO.h"
#include <Arduino.h>
#include "ConfigInterface.h"
#include "Arduino.h"

//### SHIFT REGISTER PINS

int shiftRegister_IN = D0; //D4;
int shiftRegister_Clock = D4;
int shiftRegister_Latch = D3;

SR_IO sr_io;

// Constructor
ShiftRegisterIO::ShiftRegisterIO()
{
}

void ShiftRegisterIO::init()
{
    pinMode(shiftRegister_IN, OUTPUT);
    pinMode(shiftRegister_Clock, OUTPUT);
    pinMode(shiftRegister_Latch, OUTPUT);
}

void ShiftRegisterIO::allOne()
{
    digitalWrite(shiftRegister_Latch, LOW);
    for (size_t i = 0; i <= 50; i++)
    {
        digitalWrite(shiftRegister_Clock, LOW);
        digitalWrite(shiftRegister_IN, HIGH);
        digitalWrite(shiftRegister_Clock, HIGH);
    }
    digitalWrite(shiftRegister_Latch, HIGH);
}

void ShiftRegisterIO::allZero()
{

    digitalWrite(shiftRegister_Latch, LOW);
    for (size_t i = 0; i <= 50; i++)
    {
        digitalWrite(shiftRegister_Clock, LOW);
        digitalWrite(shiftRegister_IN, LOW);
        digitalWrite(shiftRegister_Clock, HIGH);
    }
    digitalWrite(shiftRegister_Latch, HIGH);
}

void ShiftRegisterIO::ledBlink(int time)
{

    //SR_IO sr_io;
    sr_io.PT100Reader_Enable = false;
    sr_io.TMuxS0 = false;
    sr_io.TMuxS1 = false;
    sr_io.TMuxS2 = false;
    sr_io.RMuxS2 = false;
    sr_io.RMuxS1 = false;
    sr_io.RMuxS0 = false;
    sr_io.Distance_Enable = false;

    sr_io.LED_Wifi = true;
    sr_io.LED_Error = true;
    sr_io.LED_Ready = true;
    sr_io.LED_RJ4 = true;
    sr_io.LED_RJ3 = true;
    sr_io.LED_RJ2 = true;
    sr_io.LED_RJ1 = true;
    sr_io.OLED_Enbable = false;

    sr_io.RJ4_Counter = false;
    sr_io.RJ4_Status = true;
    sr_io.RJ3_Counter = false;
    sr_io.RJ3_Status = true;
    sr_io.RJ2_Counter = false;
    sr_io.RJ2_Status = true;
    sr_io.RJ1_Counter = false;
    sr_io.RJ1_Status = true;

    write();
    delay(time);

    sr_io.LED_Wifi = false;
    sr_io.LED_Error = false;
    sr_io.LED_Ready = false;
    sr_io.LED_RJ4 = false;
    sr_io.LED_RJ3 = false;
    sr_io.LED_RJ2 = false;
    sr_io.LED_RJ1 = false;
    sr_io.RJ4_Status = false;
    sr_io.RJ3_Status = false;
    sr_io.RJ2_Status = false;
    sr_io.RJ1_Status = false;

    write();
    delay(time);

    sr_io.LED_Wifi = true;
    sr_io.LED_Error = true;
    sr_io.LED_Ready = true;
    sr_io.LED_RJ4 = true;
    sr_io.LED_RJ3 = true;
    sr_io.LED_RJ2 = true;
    sr_io.LED_RJ1 = true;
    sr_io.RJ4_Status = true;
    sr_io.RJ3_Status = true;
    sr_io.RJ2_Status = true;
    sr_io.RJ1_Status = true;
    delay(time);

    write();
}

void ShiftRegisterIO::r_MuxSelect(int channel)
{
    switch (channel)
    {
    case 0:
        sr_io.RMuxS0 = false;
        sr_io.RMuxS1 = false;
        sr_io.RMuxS2 = false;
        break;
    case 1:
        sr_io.RMuxS0 = true;
        sr_io.RMuxS1 = false;
        sr_io.RMuxS2 = false;
        break;
    case 2:
        sr_io.RMuxS0 = false;
        sr_io.RMuxS1 = true;
        sr_io.RMuxS2 = false;
        break;
    case 3:
        sr_io.RMuxS0 = true;
        sr_io.RMuxS1 = true;
        sr_io.RMuxS2 = false;
        break;
    case 4:
        sr_io.RMuxS0 = false;
        sr_io.RMuxS1 = false;
        sr_io.RMuxS2 = true;
        break;
    case 5:
        sr_io.RMuxS0 = true;
        sr_io.RMuxS1 = false;
        sr_io.RMuxS2 = true;
        break;
    case 6:
        sr_io.RMuxS0 = false;
        sr_io.RMuxS1 = true;
        sr_io.RMuxS2 = true;
        break;
    case 7:
        sr_io.RMuxS0 = true;
        sr_io.RMuxS1 = true;
        sr_io.RMuxS2 = true;
        break;
    default:
        sr_io.RMuxS0 = false;
        sr_io.RMuxS1 = false;
        sr_io.RMuxS2 = false;
        break;
    }
}

void ShiftRegisterIO::t_MuxSelect(int channel)
{
    switch (channel)
    {
    case 0:
        sr_io.TMuxS0 = false;
        sr_io.TMuxS1 = false;
        sr_io.TMuxS2 = false;
        break;
    case 1:
        sr_io.TMuxS0 = true;
        sr_io.TMuxS1 = false;
        sr_io.TMuxS2 = false;
        break;
    case 2:
        sr_io.TMuxS0 = false;
        sr_io.TMuxS1 = true;
        sr_io.TMuxS2 = false;
        break;
    case 3:
        sr_io.TMuxS0 = true;
        sr_io.TMuxS1 = true;
        sr_io.TMuxS2 = false;
        break;
    case 4:
        sr_io.TMuxS0 = false;
        sr_io.TMuxS1 = false;
        sr_io.TMuxS2 = true;
        break;
    case 5:
        sr_io.TMuxS0 = true;
        sr_io.TMuxS1 = false;
        sr_io.TMuxS2 = true;
        break;
    case 6:
        sr_io.TMuxS0 = false;
        sr_io.TMuxS1 = true;
        sr_io.TMuxS2 = true;
        break;
    case 7:
        sr_io.TMuxS0 = true;
        sr_io.TMuxS1 = true;
        sr_io.TMuxS2 = true;
        break;
    default:
        sr_io.TMuxS0 = false;
        sr_io.TMuxS1 = false;
        sr_io.TMuxS2 = false;
        break;
    }
}

void ShiftRegisterIO::checkMeterResistance(struct MeterData *meterData)
{
    r_MuxSelect(meterData->mux_resistance);
    write();

    int analogValue = analogRead(A0);
    meterData->mux_resistance_value = analogValue;

    //Serial.print(analogValue);
    //Serial.print(meterData->mux_resistance_threshold);

    if (analogValue <= meterData->mux_resistance_threshold)
    {

        if (!meterData->waterMeterState)
        {
            meterData->waterMeterState = true;
            //meterData->mux_resistance_edgeDetect = true;
        }

        //meterData->waterMeterState = true;
    }
    else
    {

        if (meterData->waterMeterState)
        {
            meterData->mux_resistance_edgeDetect = true;
            meterData->waterMeterState = false;
        }
        else
        {
            //meterData->waterMeterState = false;
            meterData->mux_resistance_edgeDetect = false;
        }

        //meterData->waterMeterState = false;
        //if (meterData->mux_resistance_edgeDetect)
        //{
        //    meterData->water_CounterValue_m3 += 5;
        //    meterData->mux_resistance_edgeDetect = false;
        //}
    }

    //meterData->waterMeterState = a

    r_MuxSelect(99);
    write();
}

void ShiftRegisterIO::checkForVoltage(HeaterData *heaterData)
{
    r_MuxSelect(heaterData->mux_select);
    write();
    int analogValue = analogRead(A0);
    heaterData->value_Analog = analogValue;
    bool oldState = heaterData->state;

    if (analogValue < heaterData->threshold_Analaog)
    {
        heaterData->state = true;
        heaterData->runtime_on_current = millis() - heaterData->runtime_on_start;
    }
    else
    {
        heaterData->state = false;
        heaterData->runtime_off_current = millis() - heaterData->runtime_off_start;
    }
    if (oldState != heaterData->state)
    {
        if (heaterData->state == true)
        {
            heaterData->pos_edge = true;
            heaterData->neg_edge = false;
        }
        else
        {
            heaterData->pos_edge = false;
            heaterData->neg_edge = true;
        }
    }
    else
    {
        heaterData->pos_edge = false;
        heaterData->neg_edge = false;
    }

    if (heaterData->pos_edge)
    {
        heaterData->runtime_on_start = millis();
        heaterData->counter_times_off++;
        heaterData->runtime_off_previous = heaterData->runtime_off_current;
        heaterData->runtime_off_current = 0.0;
    }
    if (heaterData->neg_edge)
    {
        heaterData->runtime_off_start = millis();
        heaterData->counter_times_on++;
        heaterData->runtime_on_previous = heaterData->runtime_on_current;
        heaterData->runtime_on_current = 0.0;
    }
}

String ShiftRegisterIO::checkButton(String name)
{
    if (name.equals("UP") || name.equals("up"))
    {
        r_MuxSelect(1);
        write();

        int analogValue = analogRead(A0);
        if (analogValue <= 100)
        {
            return "UP";
        }

        return "X";
    }
    if (name.equals("DOWN") || name.equals("down"))
    {
        r_MuxSelect(0);
        write();
        int analogValue = analogRead(A0);
        if (analogValue <= 100)
        {
            return "DOWN";
        }
        return "X";
    }
    if (name.equals("SELECT") || name.equals("select"))
    {
        r_MuxSelect(2);
        write();
        int analogValue = analogRead(A0);
        if (analogValue <= 100)
        {
            return "SELECT";
        }
        return "X";
    }
    return "";
}

void ShiftRegisterIO::toggleDisplay(bool toggle)
{
    sr_io.OLED_Enbable = toggle;
    write();
}

void ShiftRegisterIO::counter_RJX(int x, bool toggle)
{
    switch (x)
    {
    case 0:
        sr_io.RJ1_Counter = toggle;
        break;
    case 1:
        sr_io.RJ2_Counter = toggle;
        break;
    case 2:
        sr_io.RJ3_Counter = toggle;
        break;
    case 3:
        sr_io.RJ4_Counter = toggle;
        break;
    default:
        break;
    }
    write();
}

void ShiftRegisterIO::led_ERROR(bool toggle)
{

    sr_io.LED_Error = toggle;
    write();
}

void ShiftRegisterIO::led_READY(bool toggle)
{
    sr_io.LED_Ready = toggle;
    write();
}

void ShiftRegisterIO::led_WIFI(bool toggle)
{
    sr_io.LED_Wifi = toggle;
    write();
}
void ShiftRegisterIO::led_RJ1(bool toggle)
{
    sr_io.LED_RJ1 = toggle;
    write();
}
void ShiftRegisterIO::led_RJ2(bool toggle)
{
    sr_io.LED_RJ2 = toggle;
    write();
}
void ShiftRegisterIO::led_RJ3(bool toggle)
{
    sr_io.LED_RJ3 = toggle;
    write();
}
void ShiftRegisterIO::led_RJ4(bool toggle)
{
    sr_io.LED_RJ4 = toggle;
    write();
}

void ShiftRegisterIO::led_statusRJ1(bool toggle)
{
    sr_io.RJ1_Status = toggle;
    write();
}
void ShiftRegisterIO::led_statusRJ2(bool toggle)
{
    sr_io.RJ2_Status = toggle;
    write();
}
void ShiftRegisterIO::led_statusRJ3(bool toggle)
{
    sr_io.RJ3_Status = toggle;
    write();
}
void ShiftRegisterIO::led_statusRJ4(bool toggle)
{
    sr_io.RJ4_Status = toggle;
    //shiftRegisterIO. write();
    write();
}

void ShiftRegisterIO::write()
{

    init();
    // Close and ENABLE register
    //digitalWrite(shiftRegister_NENABLE, LOW);
    digitalWrite(shiftRegister_Latch, LOW);

    //Write all Values

    //U7 - ShiftRegister
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.PT100Reader_Enable);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.RMuxS2);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.RMuxS1);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.RMuxS0);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.TMuxS0);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.TMuxS1);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.TMuxS2);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, LOW); // NC PIN
    digitalWrite(shiftRegister_Clock, HIGH);

    //U6 - ShiftRegister
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.OLED_Enbable);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.RJ4_Status);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.RJ2_Counter);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.RJ2_Status);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.RJ3_Counter);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.RJ3_Status);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.RJ4_Counter);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.RJ1_Status);
    digitalWrite(shiftRegister_Clock, HIGH);

    //U5 - ShiftRegister
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.LED_Ready);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.LED_Error);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.LED_Wifi);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.RJ1_Counter);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.LED_RJ2);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.LED_RJ3);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.LED_RJ4);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io.LED_RJ1);
    digitalWrite(shiftRegister_Clock, HIGH);

    // Open register
    digitalWrite(shiftRegister_Latch, HIGH);
}
