#include "DisplayInterface.h"
#include "ConfigInterface.h"
#include <ESP8266WiFi.h>

int selected_Interface = 0;
int selected_Entry = 0;
bool edit_enable = false;

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

void DisplayInterface::displayMeter(Adafruit_SSD1306 *display, struct MeterData *meterData, struct HeaterData *heaterData)
{
    display->clearDisplay();

    display->setTextSize(2);
    display->setCursor(0, 0);
    display->print(meterData->meterID);
    display->setTextSize(1);
    display->setCursor(0, 0);

    if (heaterData->state == true)
    {
        //Draw heater symbol
        display->drawLine(14, 14, 28, 14, SSD1306_WHITE);
        display->drawLine(14, 14, 14, 2, SSD1306_WHITE);
        display->drawLine(14, 2, 28, 2, SSD1306_WHITE);
        display->drawLine(28, 14, 28, 2, SSD1306_WHITE);
        display->drawLine(14, 0, 28, 0, SSD1306_WHITE);
        display->drawLine(14, 0, 14, 2, SSD1306_WHITE);
        display->drawLine(28, 0, 28, 2, SSD1306_WHITE);
        display->drawTriangle(16, 7, 18, 5, 20, 7, SSD1306_WHITE);
        display->drawTriangle(22, 7, 24, 5, 26, 7, SSD1306_WHITE);
        display->drawTriangle(19, 11, 21, 9, 23, 11, SSD1306_WHITE);
    }

    display->drawTriangle(34, 2, 36, 0, 38, 2, SSD1306_WHITE);
    display->drawLine(36, 1, 36, 5, SSD1306_WHITE);
    display->print("       ");
    display->printf("%2.1f", meterData->temperature_up_Celcius);
    display->print("/");
    display->printf("%2.2f", meterData->temperature_up_Celcius_mean);
    display->print(" ");
    display->print((char)247);
    display->println("C");

    display->drawTriangle(34, 12, 36, 14, 38, 12, SSD1306_WHITE);
    display->drawLine(36, 9, 36, 13, SSD1306_WHITE);
    display->print("       ");
    display->printf("%2.1f", meterData->temperature_down_Celcius);
    display->print("/");
    display->printf("%2.2f", meterData->temperature_down_Celcius_mean);
    display->print(" ");
    display->print((char)247);
    display->println("C");

    display->println();
    display->printf("%.5f", meterData->absolute_HeatEnergy_MWh, 5);
    display->println(" MWh");

    // display->print("Engery: ");
    display->printf("%.5f", meterData->delta_HeatEnergy_J);
    display->println(" J");

    //display->print("Water:  ");
    display->printf("%.5f", meterData->water_CounterValue_m3);
    display->println(" m^3");

    //display->print("T_Down: ");
    //display->print(meterData->temperature_down_Celcius);
    //display->print("/");
    //display->print(meterData->temperature_down_Celcius_mean);
    //display->println(" C");

    display->display();
}

void DisplayInterface::displayHeater(Adafruit_SSD1306 *display, struct HeaterData *heaterData)
{
    display->clearDisplay();

    display->setTextSize(2);
    display->setCursor(0, 0);
    display->print("H");

    if (heaterData->state == true)
    {
        //Draw heater symbol
        display->drawLine(14, 14, 28, 14, SSD1306_WHITE);
        display->drawLine(14, 14, 14, 2, SSD1306_WHITE);
        display->drawLine(14, 2, 28, 2, SSD1306_WHITE);
        display->drawLine(28, 14, 28, 2, SSD1306_WHITE);
        display->drawLine(14, 0, 28, 0, SSD1306_WHITE);
        display->drawLine(14, 0, 14, 2, SSD1306_WHITE);
        display->drawLine(28, 0, 28, 2, SSD1306_WHITE);
        display->drawTriangle(16, 7, 18, 5, 20, 7, SSD1306_WHITE);
        display->drawTriangle(22, 7, 24, 5, 26, 7, SSD1306_WHITE);
        display->drawTriangle(19, 11, 21, 9, 23, 11, SSD1306_WHITE);
    }
    display->setTextSize(1);
    display->setCursor(0, 17);

    display->drawLine(0, 15, 128, 15, SSD1306_WHITE);
    display->drawLine(70, 15, 70, 64, SSD1306_WHITE);
    display->drawLine(18, 15, 18, 64, SSD1306_WHITE);
    display->print("    OFF      ON\n");
    display->drawLine(0, 25, 128, 25, SSD1306_WHITE);

    display->setCursor(0, 30);
    display->print("N");
    display->setCursor(25, 30);

    if (((float)heaterData->runtime_off_current / 1000.0 / 60 / 60) > 1.0)
    {
        display->printf("%.1fh", (float)heaterData->runtime_off_current / 1000.0 / 60 / 60);
    }
    else
    {
        if (((float)heaterData->runtime_off_current / 1000.0 / 60) > 1.0)
        {
            display->printf("%.2fmin", (float)heaterData->runtime_off_current / 1000.0 / 60);
        }
        else
        {
            display->printf("%.2fs", (float)heaterData->runtime_off_current / 1000.0);
        }
    }

    display->setCursor(75, 30);
    if (((float)heaterData->runtime_on_current / 1000.0 / 60 / 60) > 1.0)
    {
        display->printf("%.1fh", (float)heaterData->runtime_on_current / 1000.0 / 60 / 60);
    }
    else
    {
        if (((float)heaterData->runtime_on_current / 1000.0 / 60) > 1.0)
        {
            display->printf("%.2fmin", (float)heaterData->runtime_on_current / 1000.0 / 60);
        }
        else
        {
            display->printf("%.2fs", (float)heaterData->runtime_on_current / 1000.0);
        }
    }

    display->setCursor(0, 40);
    display->print("N-1");

    display->setCursor(25, 40);
    if (((float)heaterData->runtime_off_previous / 1000.0 / 60 / 60) > 1.0)
    {
        display->printf("%.1fh", (float)heaterData->runtime_off_previous / 1000.0 / 60 / 60);
    }
    else
    {
        if (((float)heaterData->runtime_off_previous / 1000.0 / 60) > 1.0)
        {
            display->printf("%.2fmin", (float)heaterData->runtime_off_previous / 1000.0 / 60);
        }
        else
        {
            display->printf("%.2fs", (float)heaterData->runtime_off_previous / 1000.0);
        }
    }

    display->setCursor(75, 40);
    if (((float)heaterData->runtime_on_previous / 1000.0 / 60 / 60) > 1.0)
    {
        display->printf("%.1fh", (float)heaterData->runtime_on_previous / 1000.0 / 60 / 60);
    }
    else
    {
        if (((float)heaterData->runtime_on_previous / 1000.0 / 60) > 1.0)
        {
            display->printf("%.2fmin", (float)heaterData->runtime_on_previous / 1000.0 / 60);
        }
        else
        {
            display->printf("%.2fs", (float)heaterData->runtime_on_previous / 1000.0);
        }
    }

    display->display();
}

int DisplayInterface::displaySettingsSelectInterface(Adafruit_SSD1306 *display, String buttonState)
{
    if (buttonState == "UP")
    {
        selected_Interface--;
    }

    if (buttonState == "DOWN")
    {
        selected_Interface++;
    }

    // Do not go out of display
    if (selected_Interface < 0)
    {
        selected_Interface = 5;
    }
    if (selected_Interface > 5)
    {
        selected_Interface = 0;
    }

    display->clearDisplay();

    display->setCursor(0, 0);
    display->setTextSize(2);
    display->print("Config");

    display->setTextSize(1);
    if (selected_Interface == 0)
    {
        display->print("<- Back");
        if (buttonState == "SELECT")
        {
            return 1;
        }
    }

    display->println();
    display->println();
    display->println();

    if (selected_Interface == 1)
    {
        display->print("- ");
    }
    else
    {
        display->print("  ");
    }

    display->println("Interface 1");

    if (selected_Interface == 2)
    {
        display->print("- ");
    }
    else
    {
        display->print("  ");
    }
    display->println("Interface 2");

    if (selected_Interface == 3)
    {
        display->print("- ");
    }
    else
    {
        display->print("  ");
    }
    display->println("Interface 3");

    if (selected_Interface == 4)
    {
        display->print("- ");
    }
    else
    {
        display->print("  ");
    }
    display->println("Interface 4");

    if (selected_Interface == 5)
    {
        display->print("- ");
    }
    else
    {
        display->print("  ");
    }
    display->println("== SAVE ==");

    display->display();

    if (buttonState.equals("SELECT"))
    {
        if (selected_Interface == 5)
        {
            return 100;
        }
        else
        {
            return 6;
        }
    }
    else
    {
        return 5;
    }
}

int DisplayInterface::displaySettingsSelectTempOrRes(Adafruit_SSD1306 *display, String buttonState)
{

    if (buttonState == "UP")
    {
        selected_Entry--;
    }

    if (buttonState == "DOWN")
    {
        selected_Entry++;
    }

    // Do not go out of display
    if (selected_Entry < 0)
    {
        selected_Entry = 3;
    }
    if (selected_Entry > 3)
    {
        selected_Entry = 0;
    }

    display->clearDisplay();

    display->setCursor(0, 0);
    display->setTextSize(2);
    display->print("CONF ");
    display->print(selected_Interface);

    display->setTextSize(1);
    if (selected_Entry == 0)
    {
        display->print("<- Back");
    }

    display->println();
    display->println();
    display->println();

    if (selected_Entry == 1)
    {
        display->print("- ");
    }
    else
    {
        display->print("  ");
    }

    display->println("Temperature");

    if (selected_Entry == 2)
    {
        display->print("- ");
    }
    else
    {
        display->print("  ");
    }
    display->println("Resistance");

    display->display();

    if (buttonState.equals("SELECT"))
    {
        switch (selected_Entry)
        {
        case 0:
            return 5;
            break;
        case 1:
            return 7;
            break;
        case 2:
            return 91;
            break;

        default:
            break;
        }
    }
    else
    {
        return 6;
    }
}

int DisplayInterface::displayConfigTemperature(Adafruit_SSD1306 *display, String buttonState, struct MeterData meterData[4])
{
    if (buttonState == "UP")
    {
        if (!edit_enable)
        {
            selected_Entry--;
        }
        else
        {
            if (selected_Entry == 1)
            {
                meterData[selected_Interface - 1].RREF_up++;
            }
            if (selected_Entry == 2)
            {
                meterData[selected_Interface - 1].RREF_down++;
            }
        }
    }

    if (buttonState == "DOWN")
    {
        if (!edit_enable)
        {
            selected_Entry++;
        }
        else
        {
            if (selected_Entry == 1)
            {
                meterData[selected_Interface - 1].RREF_up--;
            }
            if (selected_Entry == 2)
            {
                meterData[selected_Interface - 1].RREF_down--;
            }
        }
    }

    if (buttonState == "SELECT")
    {
        if (!edit_enable)
        {
            edit_enable = true;
        }
        else
        {
            edit_enable = false;
        }
    }

    // Do not go out of display
    if (selected_Entry < 0)
    {
        selected_Entry = 2;
    }
    if (selected_Entry > 2)
    {
        selected_Entry = 0;
    }

    display->clearDisplay();

    display->setCursor(0, 0);
    display->setTextSize(2);
    display->print("CONF ");
    display->print(selected_Interface);
    display->setTextSize(1);
    if (selected_Entry == 0)
    {
        display->print("<- Back");
        if (buttonState == "SELECT")
        {
            return 6;
        }
    }
    display->println();
    display->println();
    display->println();

    if (selected_Entry == 1)
    {
        if (!edit_enable)
        {
            display->print("- ");
        }
        else
        {
            display->print("+ ");
        }
    }
    else
    {
        display->print("  ");
    }

    display->print("T_UP_RREF:   ");
    display->println(meterData[selected_Interface - 1].RREF_up);
    display->print("  T_UP:        ");
    display->println(meterData[selected_Interface - 1].temperature_up_Celcius);

    if (selected_Entry == 2)
    {
        if (!edit_enable)
        {
            display->print("- ");
        }
        else
        {
            display->print("+ ");
        }
    }
    else
    {
        display->print("  ");
    }

    display->print("T_DOWN_RREF: ");
    display->println(meterData[selected_Interface - 1].RREF_down);
    display->print("  T_DOWN:      ");
    display->println(meterData[selected_Interface - 1].temperature_down_Celcius);

    display->display();
    return 7;
}

int DisplayInterface::displayConfigResistance(Adafruit_SSD1306 *display, String buttonState, struct MeterData meterData[4])
{
    if (buttonState == "UP")
    {
        if (!edit_enable)
        {
            selected_Entry--;
        }
        else
        {
            if (selected_Entry == 1)
            {
                meterData[selected_Interface - 1].mux_resistance_threshold++;
            }
        }
    }

    if (buttonState == "DOWN")
    {
        if (!edit_enable)
        {
            selected_Entry++;
        }
        else
        {
            if (selected_Entry == 1)
            {
                meterData[selected_Interface - 1].mux_resistance_threshold--;
            }
        }
    }

    if (buttonState == "SELECT")
    {
        if (!edit_enable)
        {
            edit_enable = true;
        }
        else
        {
            edit_enable = false;
        }
    }

    // Do not go out of display
    if (selected_Entry < 0)
    {
        selected_Entry = 1;
    }
    if (selected_Entry > 1)
    {
        selected_Entry = 0;
    }

    display->clearDisplay();

    display->setCursor(0, 0);
    display->setTextSize(2);
    display->print("CONF ");
    display->print(selected_Interface);
    display->setTextSize(1);
    if (selected_Entry == 0)
    {
        display->print("<- Back");
        if (buttonState == "SELECT")
        {
            return 6;
        }
    }
    display->println();
    display->println();
    display->println();
    display->println("Resistance:");
    display->println("");
    if (selected_Entry == 1)
    {
        if (!edit_enable)
        {
            display->print("- ");
        }
        else
        {
            display->print("+ ");
        }
    }
    else
    {
        display->print("  ");
    }

    display->print("Threshold: ");
    display->println(meterData[selected_Interface - 1].mux_resistance_threshold);
    display->print("  Value:     ");
    display->println(meterData[selected_Interface - 1].mux_resistance_value);
    display->print("  State:     ");
    display->println(meterData[selected_Interface - 1].waterMeterState);

    display->display();
    return 91;
}

void DisplayInterface::displaySavingScreen(Adafruit_SSD1306 *display)
{
    display->clearDisplay();

    display->setCursor(0, 0);
    display->setTextSize(2);
    display->print("SAVING... ");

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