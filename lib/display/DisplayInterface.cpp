#include "DisplayInterface.h"
#include "ConfigInterface.h"

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
    display->U
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
    display->print("Config ");
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
            return 8;
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