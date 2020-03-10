#include "ShiftRegisterIO.h"
#include <Arduino.h>
#include "ConfigInterface.h"
#include "Arduino.h"

//### SHIFT REGISTER PINS
int shiftRegister_Latch = D3;
int shiftRegister_IN = D4;                       //D4;
int shiftRegister_NENABLE = shiftRegister_Latch; //D3;
int shiftRegister_Clock = D0;

// Constructor
ShiftRegisterIO::ShiftRegisterIO()
{
}

void ShiftRegisterIO::init()
{
    pinMode(shiftRegister_IN, OUTPUT);
    pinMode(shiftRegister_NENABLE, OUTPUT);
    pinMode(shiftRegister_Clock, OUTPUT);
    pinMode(shiftRegister_Latch, OUTPUT);
}

void ShiftRegisterIO::allOne()
{
    digitalWrite(shiftRegister_NENABLE, LOW);

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
    digitalWrite(shiftRegister_NENABLE, LOW);

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

    SR_IO sr_io;
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

    write(&sr_io);
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

    write(&sr_io);
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

    write(&sr_io);
}

void ShiftRegisterIO::r_MuxSelect(struct SR_IO *sr_io, int channel)
{
    switch (channel)
    {
    case 0:
        sr_io->RMuxS0 = false;
        sr_io->RMuxS1 = false;
        sr_io->RMuxS2 = false;
        break;
    case 1:
        sr_io->RMuxS0 = true;
        sr_io->RMuxS1 = false;
        sr_io->RMuxS2 = false;
        break;
    case 2:
        sr_io->RMuxS0 = false;
        sr_io->RMuxS1 = true;
        sr_io->RMuxS2 = false;
        break;
    case 3:
        sr_io->RMuxS0 = true;
        sr_io->RMuxS1 = true;
        sr_io->RMuxS2 = false;
        break;
    case 4:
        sr_io->RMuxS0 = false;
        sr_io->RMuxS1 = false;
        sr_io->RMuxS2 = true;
        break;
    case 5:
        sr_io->RMuxS0 = true;
        sr_io->RMuxS1 = false;
        sr_io->RMuxS2 = true;
        break;
    case 6:
        sr_io->RMuxS0 = false;
        sr_io->RMuxS1 = true;
        sr_io->RMuxS2 = true;
        break;
    case 7:
        sr_io->RMuxS0 = true;
        sr_io->RMuxS1 = true;
        sr_io->RMuxS2 = true;
        break;
    default:
        sr_io->RMuxS0 = false;
        sr_io->RMuxS1 = false;
        sr_io->RMuxS2 = false;
        break;
    }
}

void ShiftRegisterIO::t_MuxSelect(struct SR_IO *sr_io, int channel)
{
    switch (channel)
    {
    case 0:
        sr_io->TMuxS0 = false;
        sr_io->TMuxS1 = false;
        sr_io->TMuxS2 = false;
        break;
    case 1:
        sr_io->TMuxS0 = true;
        sr_io->TMuxS1 = false;
        sr_io->TMuxS2 = false;
        break;
    case 2:
        sr_io->TMuxS0 = false;
        sr_io->TMuxS1 = true;
        sr_io->TMuxS2 = false;
        break;
    case 3:
        sr_io->TMuxS0 = true;
        sr_io->TMuxS1 = true;
        sr_io->TMuxS2 = false;
        break;
    case 4:
        sr_io->TMuxS0 = false;
        sr_io->TMuxS1 = false;
        sr_io->TMuxS2 = true;
        break;
    case 5:
        sr_io->TMuxS0 = true;
        sr_io->TMuxS1 = false;
        sr_io->TMuxS2 = true;
        break;
    case 6:
        sr_io->TMuxS0 = false;
        sr_io->TMuxS1 = true;
        sr_io->TMuxS2 = true;
        break;
    case 7:
        sr_io->TMuxS0 = true;
        sr_io->TMuxS1 = true;
        sr_io->TMuxS2 = true;
        break;
    default:
        sr_io->TMuxS0 = false;
        sr_io->TMuxS1 = false;
        sr_io->TMuxS2 = false;
        break;
    }
}

void ShiftRegisterIO::checkMeterResistance(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, struct MeterData *meterData)
{
    shiftRegisterIO->r_MuxSelect(sr_io, meterData->mux_resistance);
    shiftRegisterIO->write(sr_io);

    int analogValue = analogRead(A0);

    // Serial.print(analogValue);
    // Serial.print(meterData->mux_resistance_threshold);

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

    shiftRegisterIO->r_MuxSelect(sr_io, 99);
    shiftRegisterIO->write(sr_io);
}

String ShiftRegisterIO::checkButton(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, String name)
{
    if (name.equals("UP") || name.equals("up"))
    {
        shiftRegisterIO->r_MuxSelect(sr_io, 1);
        shiftRegisterIO->write(sr_io);

        int analogValue = analogRead(A0);
        Serial.print("B:");
        Serial.print(analogValue);

        return "UP";
    }
    if (name.equals("DOWN") || name.equals("down"))
    {
        shiftRegisterIO->r_MuxSelect(sr_io, 1);
        return "DOWN";
    }
    if (name.equals("SELECT") || name.equals("select"))
    {
        shiftRegisterIO->r_MuxSelect(sr_io, 2);
        return "SELECT";
    }
    return "";
}

void ShiftRegisterIO::led_ERROR(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, bool toggle)
{

    sr_io->LED_Error = toggle;
    shiftRegisterIO->write(sr_io);
}

void ShiftRegisterIO::led_READY(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, bool toggle)
{
    sr_io->LED_Ready = toggle;
    shiftRegisterIO->write(sr_io);
}

void ShiftRegisterIO::led_WIFI(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, bool toggle)
{
    sr_io->LED_Wifi = toggle;
    shiftRegisterIO->write(sr_io);
}
void ShiftRegisterIO::led_RJ1(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, bool toggle)
{
    sr_io->LED_RJ1 = toggle;
    shiftRegisterIO->write(sr_io);
}
void ShiftRegisterIO::led_RJ2(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, bool toggle)
{
    sr_io->LED_RJ2 = toggle;
    shiftRegisterIO->write(sr_io);
}
void ShiftRegisterIO::led_RJ3(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, bool toggle)
{
    sr_io->LED_RJ3 = toggle;
    shiftRegisterIO->write(sr_io);
}
void ShiftRegisterIO::led_RJ4(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, bool toggle)
{
    sr_io->LED_RJ4 = toggle;
    shiftRegisterIO->write(sr_io);
}

void ShiftRegisterIO::led_statusRJ1(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, bool toggle)
{
    sr_io->RJ1_Status = toggle;
    shiftRegisterIO->write(sr_io);
}
void ShiftRegisterIO::led_statusRJ2(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, bool toggle)
{
    sr_io->RJ2_Status = toggle;
    shiftRegisterIO->write(sr_io);
}
void ShiftRegisterIO::led_statusRJ3(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, bool toggle)
{
    sr_io->RJ3_Status = toggle;
    shiftRegisterIO->write(sr_io);
}
void ShiftRegisterIO::led_statusRJ4(struct ShiftRegisterIO *shiftRegisterIO, struct SR_IO *sr_io, bool toggle)
{
    sr_io->RJ4_Status = toggle;
    shiftRegisterIO->write(sr_io);
}

void ShiftRegisterIO::write(struct SR_IO *sr_io)
{
    // Close and ENABLE register
    digitalWrite(shiftRegister_NENABLE, LOW);
    digitalWrite(shiftRegister_Latch, LOW);

    //Write all Values
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->RJ1_Status);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->RJ1_Counter);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->RJ2_Status);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->RJ2_Counter);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->RJ3_Status);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->RJ3_Counter);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->RJ4_Status);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->RJ4_Counter);
    digitalWrite(shiftRegister_Clock, HIGH);

    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->OLED_Enbable);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->LED_RJ1);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->LED_RJ2);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->LED_RJ3);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->LED_RJ4);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->LED_Ready);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->LED_Error);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->LED_Wifi);
    digitalWrite(shiftRegister_Clock, HIGH);

    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->Distance_Enable);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->RMuxS0);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->RMuxS1);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->RMuxS2);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->TMuxS2);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->TMuxS1);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->TMuxS0);
    digitalWrite(shiftRegister_Clock, HIGH);
    digitalWrite(shiftRegister_Clock, LOW);
    digitalWrite(shiftRegister_IN, sr_io->PT100Reader_Enable);
    digitalWrite(shiftRegister_Clock, HIGH);

    // Open register
    digitalWrite(shiftRegister_Latch, HIGH);
}
