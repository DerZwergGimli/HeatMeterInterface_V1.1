#include "DisplayInterface.h"
#include "ConfigInterface.h"

DisplayInterface ::DisplayInterface()
{
}

void DisplayInterface::boot(Adafruit_SSD1306 *display)
{
    display->clearDisplay();
    display->setTextSize(2); // Draw 2X-scale text
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(0, 0);
    display->println("Booting...");
    display->display();
}

void DisplayInterface ::displayMeter(Adafruit_SSD1306 *display, struct MeterData *meterData)
{
    display->clearDisplay();
    display->setCursor(0, 0);
    display->setTextSize(2);
    display->print(meterData->meterID);
    display->setTextSize(1);
    display->print("     ");
    display->println(meterData->absolute_HeatEnergy_MWh, 5);
    display->println();

    display->setTextSize(1);
    display->print("Engery: ");
    display->println(meterData->delta_HeatEnergy_J);

    display->print("Water:  ");
    display->print(meterData->water_CounterValue_m3);
    display->println(" m^3");

    display->print("T_Up:   ");
    display->print(meterData->temperature_up_Celcius);
    display->print("/");
    display->print(meterData->temperature_up_Celcius_mean);
    display->println(" C");

    display->print("T_Down: ");
    display->print(meterData->temperature_down_Celcius);
    display->print("/");
    display->print(meterData->temperature_down_Celcius_mean);
    display->println(" C");

    display->print("State: ");
    display->println(meterData->waterMeterState);

    display->display();
}

void DisplayInterface::printHello(Adafruit_SSD1306 *display)
{
    display->clearDisplay();
    display->setCursor(0, 0);
    display->setTextSize(2);
    display->print("Hello");
    display->display();
}