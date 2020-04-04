#include "DisplayInterface.h"
#include "ConfigInterface.h"
#include <ESP8266WiFi.h>
#include <ArduinoLog.h>
#include <ShiftRegisterIO.h>
#include <Wire.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

struct MeterData *p_MeterData;
struct HeaterData *p_HeaterData;

int display_state = 0;
int showInterfaceTime_counter = 0;
int showInterfaceTime_limit = 10;

int selected_Interface = 0;
int selected_Entry = 0;
bool edit_enable = false;

DisplayInterface ::DisplayInterface()
{
}

void DisplayInterface::init()
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Log.error((F("SSD1306 allocation failed")));
        for (;;)
            Log.fatal("Error while connecting to Display");
    }
    display_state = 1;
}
void DisplayInterface::displayBootMessage()
{
    display.clearDisplay();
    display.setTextSize(2); // Draw 2X-scale text
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println("Booting...");
    display.display();
}

void DisplayInterface::displayTimer(int nextState)
{

    if (showInterfaceTime_counter < showInterfaceTime_limit)
    {
        showInterfaceTime_counter++;
    }
    else
    {
        showInterfaceTime_counter = 0;
        display_state = nextState;
    }
}
void DisplayInterface::displayStateMachine(struct MeterData *meterData, struct HeaterData *heaterData)
{
    p_MeterData = meterData;
    p_HeaterData = heaterData;

    switch (display_state)
    {
    case 0: // Setup Display
        init();
        display_state++;
        break;
    case 1: // DisplayBoot Message
        displayBootMessage();
        display_state = 10;
        break;
    case 10: // Display Interface Meter 1
        displayMeter(0);
        displayTimer(11);
        break;
    case 11: // Display Interface Meter 2
        displayMeter(1);
        displayTimer(12);
        break;
    case 12: // Display Interface Meter 3
        displayMeter(2);
        displayTimer(13);
        break;
    case 13: // Display Interface Meter 4
        displayMeter(3);
        displayTimer(15);
        break;
    case 15: // Display Interface Heater
        displayHeater();
        displayTimer(10);
        break;
    default:
        break;
    }
}

void DisplayInterface::displayMeter(int interface)
{
    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print(p_MeterData[interface].meter_ID);
    display.setTextSize(1);
    display.setCursor(0, 0);

    if (p_HeaterData->state == true)
    {
        //Draw heater symbol
        display.drawLine(14, 14, 28, 14, SSD1306_WHITE);
        display.drawLine(14, 14, 14, 2, SSD1306_WHITE);
        display.drawLine(14, 2, 28, 2, SSD1306_WHITE);
        display.drawLine(28, 14, 28, 2, SSD1306_WHITE);
        display.drawLine(14, 0, 28, 0, SSD1306_WHITE);
        display.drawLine(14, 0, 14, 2, SSD1306_WHITE);
        display.drawLine(28, 0, 28, 2, SSD1306_WHITE);
        display.drawTriangle(16, 7, 18, 5, 20, 7, SSD1306_WHITE);
        display.drawTriangle(22, 7, 24, 5, 26, 7, SSD1306_WHITE);
        display.drawTriangle(19, 11, 21, 9, 23, 11, SSD1306_WHITE);
    }

    display.drawTriangle(34, 2, 36, 0, 38, 2, SSD1306_WHITE);
    display.drawLine(36, 1, 36, 5, SSD1306_WHITE);
    display.print("       ");
    display.printf("%2.1f", p_MeterData[interface].temperature_up_Celcius);
    display.print("/");
    display.printf("%2.2f", p_MeterData[interface].temperature_up_Celcius_mean);
    display.print(" ");
    display.print((char)247);
    display.println("C");

    display.drawTriangle(34, 12, 36, 14, 38, 12, SSD1306_WHITE);
    display.drawLine(36, 9, 36, 13, SSD1306_WHITE);
    display.print("       ");
    display.printf("%2.1f", p_MeterData[interface].temperature_down_Celcius);
    display.print("/");
    display.printf("%2.2f", p_MeterData[interface].temperature_down_Celcius_mean);
    display.print(" ");
    display.print((char)247);
    display.println("C");

    display.println();
    printWithLeadingZeros(p_MeterData[interface].counterValue_MWh);
    display.printf("%.5f", p_MeterData[interface].counterValue_MWh);
    display.println(" MWh");

    // display->print("Engery: ");
    printWithLeadingZeros(p_MeterData[interface].counterValue_J);
    display.printf("%.5f", p_MeterData[interface].counterValue_J);
    display.println(" J");

    //display->print("Water:  ");
    printWithLeadingZeros(p_MeterData[interface].counterValue_m3);
    display.printf("%.5f", p_MeterData[interface].counterValue_m3);
    display.println(" m^3");

    display.display();
}

void DisplayInterface::printWithLeadingZeros(float number)
{
    if ((number / 1000000) >= 1.0)
    {
        for (size_t i = 0; i < 0; ++i)
        {
            display.print(" ");
        }
    }
    else if ((number / 100000) >= 1.0)
    {
        for (size_t i = 0; i < 1; ++i)
        {
            display.print(" ");
        }
    }
    else if ((number / 10000) >= 1.0)
    {
        for (size_t i = 0; i < 2; ++i)
        {
            display.print(" ");
        }
    }
    else if ((number / 1000) >= 1.0)
    {
        for (size_t i = 0; i < 3; ++i)
        {
            display.print(" ");
        }
    }
    else if ((number / 100) >= 1.0)
    {
        for (size_t i = 0; i < 4; ++i)
        {
            display.print(" ");
        }
    }
    else if ((number / 10) >= 1.0)
    {
        for (size_t i = 0; i < 5; ++i)
        {
            display.print(" ");
        }
    }
    else if ((number / 1) >= 1.0)
    {
        for (size_t i = 0; i < 5; ++i)
        {
            display.print(" ");
        }
    }
}

void DisplayInterface::displayHeater()
{
    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("H");

    if (p_HeaterData->state == true)
    {
        //Draw heater symbol
        display.drawLine(14, 14, 28, 14, SSD1306_WHITE);
        display.drawLine(14, 14, 14, 2, SSD1306_WHITE);
        display.drawLine(14, 2, 28, 2, SSD1306_WHITE);
        display.drawLine(28, 14, 28, 2, SSD1306_WHITE);
        display.drawLine(14, 0, 28, 0, SSD1306_WHITE);
        display.drawLine(14, 0, 14, 2, SSD1306_WHITE);
        display.drawLine(28, 0, 28, 2, SSD1306_WHITE);
        display.drawTriangle(16, 7, 18, 5, 20, 7, SSD1306_WHITE);
        display.drawTriangle(22, 7, 24, 5, 26, 7, SSD1306_WHITE);
        display.drawTriangle(19, 11, 21, 9, 23, 11, SSD1306_WHITE);
    }
    display.setTextSize(1);
    display.setCursor(0, 17);

    display.drawLine(0, 15, 128, 15, SSD1306_WHITE);
    display.drawLine(70, 15, 70, 64, SSD1306_WHITE);
    display.drawLine(18, 15, 18, 64, SSD1306_WHITE);
    display.print("    OFF      ON\n");
    display.drawLine(0, 25, 128, 25, SSD1306_WHITE);

    display.setCursor(0, 30);
    display.print("N");
    display.setCursor(25, 30);

    if (((float)p_HeaterData->runtime_off_current / 1000.0 / 60 / 60) > 1.0)
    {
        display.printf("%.1fh", (float)p_HeaterData->runtime_off_current / 1000.0 / 60 / 60);
    }
    else
    {
        if (((float)p_HeaterData->runtime_off_current / 1000.0 / 60) > 1.0)
        {
            display.printf("%.2fm", (float)p_HeaterData->runtime_off_current / 1000.0 / 60);
        }
        else
        {
            display.printf("%.2fs", (float)p_HeaterData->runtime_off_current / 1000.0);
        }
    }

    display.setCursor(75, 30);
    if (((float)p_HeaterData->runtime_on_current / 1000.0 / 60 / 60) > 1.0)
    {
        display.printf("%.1fh", (float)p_HeaterData->runtime_on_current / 1000.0 / 60 / 60);
    }
    else
    {
        if (((float)p_HeaterData->runtime_on_current / 1000.0 / 60) > 1.0)
        {
            display.printf("%.2fm", (float)p_HeaterData->runtime_on_current / 1000.0 / 60);
        }
        else
        {
            display.printf("%.2fs", (float)p_HeaterData->runtime_on_current / 1000.0);
        }
    }

    display.setCursor(0, 40);
    display.print("N-1");

    display.setCursor(25, 40);
    if (((float)p_HeaterData->runtime_off_previous / 1000.0 / 60 / 60) > 1.0)
    {
        display.printf("%.1fh", (float)p_HeaterData->runtime_off_previous / 1000.0 / 60 / 60);
    }
    else
    {
        if (((float)p_HeaterData->runtime_off_previous / 1000.0 / 60) > 1.0)
        {
            display.printf("%.2fmin", (float)p_HeaterData->runtime_off_previous / 1000.0 / 60);
        }
        else
        {
            display.printf("%.2fs", (float)p_HeaterData->runtime_off_previous / 1000.0);
        }
    }

    display.setCursor(75, 40);
    if (((float)p_HeaterData->runtime_on_previous / 1000.0 / 60 / 60) > 1.0)
    {
        display.printf("%.1fh", (float)p_HeaterData->runtime_on_previous / 1000.0 / 60 / 60);
    }
    else
    {
        if (((float)p_HeaterData->runtime_on_previous / 1000.0 / 60) > 1.0)
        {
            display.printf("%.2fmin", (float)p_HeaterData->runtime_on_previous / 1000.0 / 60);
        }
        else
        {
            display.printf("%.2fs", (float)p_HeaterData->runtime_on_previous / 1000.0);
        }
    }

    display.display();
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
