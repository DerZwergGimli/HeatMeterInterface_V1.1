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

struct Configuration *p_config;
struct MeterData *p_MeterData;
struct HeaterData *p_HeaterData;
ButtonState *p_ButtonState;

DisplayState e_DisplayState = display_Init;
int selected_Interface;
int selected_Entry;
bool edit_enable = false;

int display_state = 0;
int showInterfaceTime_counter = 0;
int showInterfaceTime_limit = 10;

int menue_selectedItem = 0;

DisplayInterface::DisplayInterface()
{
}

void DisplayInterface::init(struct Configuration *config, struct MeterData *meterData, struct HeaterData *heaterData, ButtonState *buttonState)
{
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Log.error((F("SSD1306 allocation failed")));
        for (;;)
            Log.fatal("Error while connecting to Display");
    }
    p_MeterData = meterData;
    p_HeaterData = heaterData;
    p_ButtonState = buttonState;
    p_config = config;

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

void DisplayInterface::displayTimer(DisplayState next_displayState)
{

    if (showInterfaceTime_counter < showInterfaceTime_limit)
    {
        showInterfaceTime_counter++;
    }
    else
    {
        showInterfaceTime_counter = 0;
        e_DisplayState = next_displayState;
    }
}
void DisplayInterface::displayStateMachine()
{
    switch (e_DisplayState)
    {
    case display_Init: // Setup Display
        e_DisplayState = display_Boot;
        break;
    case display_Boot: // DisplayBoot Message
        displayBootMessage();
        e_DisplayState = display_Interface1;
        break;
    case display_Interface1: // Display Interface Meter 1
        displayMeter(0);
        displayTimer(display_Interface1);

        if (*p_ButtonState == select)
        {
            *p_ButtonState = none;
            e_DisplayState = display_menue_select_SaveEdit;
        }
        break;
    case display_Interface2: // Display Interface Meter 2
        displayMeter(1);
        displayTimer(display_Interface3);

        if (*p_ButtonState == select)
        {
            *p_ButtonState = none;
            e_DisplayState = display_menue_select_SaveEdit;
        }
        break;
    case display_Interface3: // Display Interface Meter 3
        displayMeter(2);
        displayTimer(display_Interface4);

        if (*p_ButtonState == select)
        {
            *p_ButtonState = none;
            e_DisplayState = display_menue_select_SaveEdit;
        }
        break;
    case display_Interface4: // Display Interface Meter 4
        displayMeter(3);
        displayTimer(display_InterfaceHeater);

        if (*p_ButtonState == select)
        {
            *p_ButtonState = none;
            e_DisplayState = display_menue_select_SaveEdit;
        }
        break;
    case display_InterfaceHeater: // Display Interface Heater
        displayHeater();
        displayTimer(display_Interface1);
        if (*p_ButtonState == select)
        {
            *p_ButtonState = none;
            e_DisplayState = display_menue_select_SaveEdit;
        }
        break;
    case display_menue_select_SaveEdit: //Menu SELECT EDIT SAVE
        menu_EditSave();
        if (*p_ButtonState == select)
        {
            *p_ButtonState = none;
            switch (selected_Entry)
            {
            case 0:
                e_DisplayState = display_Interface1;
                break;
            case 1:
                selected_Entry = 0;
                e_DisplayState = display_menue_select_Interface;
                break;
            case 2:
                e_DisplayState = displaySave;
                break;
            default:
                break;
            }
        }
        break;
    case display_menue_select_Interface:
        menu_selectInterface();
        if (*p_ButtonState == select)
        {
            *p_ButtonState = none;
            switch (selected_Entry)
            {
            case 0:
                selected_Entry = 0;
                e_DisplayState = display_menue_select_SaveEdit;
                break;
            case 1:
                selected_Entry = 0;
                selected_Interface = 1;
                e_DisplayState = display_menue_select_TempOrRes;
                break;
            case 2:
                selected_Entry = 0;
                selected_Interface = 2;
                e_DisplayState = display_menue_select_TempOrRes;
                break;
            case 3:
                selected_Entry = 0;
                selected_Interface = 3;
                e_DisplayState = display_menue_select_TempOrRes;
                break;
            case 4:
                selected_Entry = 0;
                selected_Interface = 4;
                e_DisplayState = display_menue_select_TempOrRes;
                break;
            case 5:
                selected_Entry = 0;
                selected_Interface = 0;
                e_DisplayState = display_menue_select_Voltage;
                break;
            default:
                break;
            }
        }
        break;
    case display_menue_select_TempOrRes: //Menu Select Temp, Res
        menu_Select_TempRes();
        if (*p_ButtonState == select)
        {
            *p_ButtonState = none;
            switch (selected_Entry)
            {
            case 0:
                selected_Entry = 0;
                e_DisplayState = display_menue_select_Interface;
                break;
            case 1:
                selected_Entry = 0;
                e_DisplayState = display_menue_edit_Temperature;
                break;
            case 2:
                selected_Entry = 0;
                e_DisplayState = display_menue_edit_Resistance;
                break;
            default:
                break;
            }
        }
        break;
    case display_menue_select_Voltage: //Menu Select Voltage
        menu_Select_Voltage();
        if (*p_ButtonState == select)
        {
            *p_ButtonState = none;
            switch (selected_Entry)
            {
            case 0:
                selected_Entry = 0;
                e_DisplayState = display_menue_select_Interface;
                break;
            case 1:
                selected_Entry = 0;
                e_DisplayState = display_menue_edit_Voltage;
                break;
            default:
                break;
            }
        }
        break;
    case display_menue_edit_Temperature: //Edit Temperature
        menu_Edit_Temperature();
        if (*p_ButtonState == select)
        {
            *p_ButtonState = none;
            switch (selected_Entry)
            {
            case 0:
                selected_Entry = 0;
                e_DisplayState = display_menue_select_TempOrRes;
                break;
            case 1:
                if (!edit_enable)
                    edit_enable = true;
                else
                    edit_enable = false;
                break;
            case 2:
                if (!edit_enable)
                    edit_enable = true;
                else
                    edit_enable = false;
                break;
            default:
                break;
            }
        }
        break;
    case display_menue_edit_Resistance: //Edit Resistance
        menu_Edit_Resistance();
        if (*p_ButtonState == select)
        {
            *p_ButtonState = none;
            switch (selected_Entry)
            {
            case 0:
                selected_Entry = 0;
                e_DisplayState = display_menue_select_TempOrRes;
                break;
            case 1:
                if (!edit_enable)
                    edit_enable = true;
                else
                    edit_enable = false;
                break;
            default:
                break;
            }
        }
        break;
    case display_menue_edit_Voltage: //Edit Voltage
        menu_Edit_Voltage();
        if (*p_ButtonState == select)
        {
            *p_ButtonState = none;
            switch (selected_Entry)
            {
            case 0:
                selected_Entry = 0;
                e_DisplayState = display_menue_select_Voltage;
                break;
            case 1:
                if (!edit_enable)
                    edit_enable = true;
                else
                    edit_enable = false;
                break;
            default:
                break;
            }
        }
        break;
    default:
        break;
    }
}

int DisplayInterface::update_menue()
{
    if (*p_ButtonState == up)
    {
        *p_ButtonState = none;
        return -1;
    }
    else if (*p_ButtonState == down)
    {
        *p_ButtonState = none;
        return 1;
    }
    else
    {
        return 0;
    }
}

int DisplayInterface::check_bounds(int max, int min, int value)
{
    if ((selected_Entry + value) > max)
    {
        return max;
    }
    else if ((selected_Entry + value) < min)
    {
        return min;
    }
    else
    {
        return (selected_Entry + value);
    }
}

void DisplayInterface::menu_EditSave()
{

    selected_Entry = check_bounds(2, 0, update_menue());

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Menu");

    display.setCursor(90, 5);

    display.setTextSize(1);

    if (selected_Entry == 0)
        display.print("> Back");
    else
        display.print("  Back");

    display.setCursor(0, 20);

    if (selected_Entry == 1)
        display.println("> EDIT");
    else
        display.println("  EDIT");

    if (selected_Entry == 2)
        display.println("> SAVE");
    else
        display.println("  SAVE");

    display.display();
}

void DisplayInterface::menu_selectInterface()
{

    selected_Entry = check_bounds(5, 0, update_menue());

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Edit");

    display.setCursor(90, 5);

    display.setTextSize(1);

    if (selected_Entry == 0)
        display.print("> Back");
    else
        display.print("  Back");

    display.setCursor(0, 20);

    if (selected_Entry == 1)
        display.println("> Meter 1");
    else
        display.println("  Meter 1");

    if (selected_Entry == 2)
        display.println("> Meter 2");
    else
        display.println("  Meter 2");

    if (selected_Entry == 3)
        display.println("> Meter 3");
    else
        display.println("  Meter 3");

    if (selected_Entry == 4)
        display.println("> Meter 4");
    else
        display.println("  Meter 4");

    if (selected_Entry == 5)
        display.println("> Heater");
    else
        display.println("  Heater");

    display.display();
}

void DisplayInterface::menu_Select_TempRes()
{
    selected_Entry = check_bounds(2, 0, update_menue());

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Meter");
    display.print(selected_Interface);

    display.setCursor(90, 5);

    display.setTextSize(1);

    if (selected_Entry == 0)
        display.print("> Back");
    else
        display.print("  Back");

    display.setCursor(0, 20);

    if (selected_Entry == 1)
        display.println("> Temperature");
    else
        display.println("  Temperature");

    if (selected_Entry == 2)
        display.println("> Resistance");
    else
        display.println("  Resistance");

    display.display();
}
void DisplayInterface::menu_Select_Voltage()
{
    selected_Entry = check_bounds(1, 0, update_menue());

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Heater");

    display.setCursor(90, 5);

    display.setTextSize(1);

    if (selected_Entry == 0)
        display.print("> Back");
    else
        display.print("  Back");

    display.setCursor(0, 20);

    if (selected_Entry == 1)
        display.println("> Voltage");
    else
        display.println("  Voltage");

    display.display();
}

void DisplayInterface::menu_Edit_Temperature()
{
    if (!edit_enable)
        selected_Entry = check_bounds(2, 0, update_menue());
    else
    {
        switch (selected_Entry)
        {
        case 1:
            p_MeterData[selected_Interface].RREF_up -= update_menue();
            break;
        case 2:
            p_MeterData[selected_Interface].RREF_down -= update_menue();
        default:
            break;
        }
    }

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Temp.");
    display.print(selected_Interface);

    display.setCursor(90, 5);

    display.setTextSize(1);

    if (selected_Entry == 0)
        display.print("> Back");
    else
        display.print("  Back");

    display.setCursor(0, 20);

    if (selected_Entry == 1 && !edit_enable)
        display.print("> RREF. UP   = ");
    else if (selected_Entry == 1 && edit_enable)
        display.print("$ RREF. UP   = ");
    else
        display.print("  RREF. UP   = ");
    display.println(p_MeterData[selected_Interface].RREF_up);
    display.print("  Temp. UP   = ");
    display.println(p_MeterData[selected_Interface].temperature_up_Celcius);

    if (selected_Entry == 2 && !edit_enable)
        display.print("> RREF. DOWN = ");

    else if (selected_Entry == 2 && edit_enable)
        display.print("$ RREF. DOWN = ");
    else
        display.print("  RREF. DOWN = ");
    display.println(p_MeterData[selected_Interface].RREF_down);
    display.print("  Temp. DOWN = ");
    display.println(p_MeterData[selected_Interface].temperature_down_Celcius);

    display.display();
}
void DisplayInterface::menu_Edit_Resistance()
{
    if (!edit_enable)
        selected_Entry = check_bounds(1, 0, update_menue());
    else
    {
        if (selected_Entry == 1)
            p_MeterData[selected_Interface].mux_resistance_threshold -= update_menue();
    }

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Res.");
    display.print(selected_Interface);

    display.setCursor(90, 5);

    display.setTextSize(1);

    if (selected_Entry == 0)
        display.print("> Back");
    else
        display.print("  Back");

    display.setCursor(0, 20);

    if (selected_Entry == 1 && !edit_enable)
        display.print("> Trigg. Value = ");
    else if (selected_Entry == 1 && edit_enable)
        display.print("$ Trigg. Value = ");
    else
        display.print("  Trigg. Value = ");
    display.println(p_MeterData[selected_Interface].mux_resistance_threshold);

    display.print("  Analog Value = ");
    display.println(p_MeterData[selected_Interface].mux_resistance_value);
    display.print("  State Value = ");
    display.println(p_MeterData[selected_Interface].waterMeterState);

    display.display();
}
void DisplayInterface::menu_Edit_Voltage()
{
    if (!edit_enable)
        selected_Entry = check_bounds(1, 0, update_menue());
    else
    {
        if (selected_Entry == 1)
            p_HeaterData->threshold_Analaog -= update_menue();
    }

    display.clearDisplay();

    display.setTextSize(2);
    display.setCursor(0, 0);
    display.print("Voltage");

    display.setCursor(90, 5);

    display.setTextSize(1);

    if (selected_Entry == 0)
        display.print("> Back");
    else
        display.print("  Back");

    display.setCursor(0, 20);

    if (selected_Entry == 1 && !edit_enable)
        display.print("> Trigg. Value = ");
    else if (selected_Entry == 1 && edit_enable)
        display.print("$ Trigg. Value = ");
    else
        display.print("  Trigg. Value = ");
    display.println(p_HeaterData->threshold_Analaog);

    display.print("  Analog Value = ");
    display.println(p_HeaterData->value_Analog);

    display.display();
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
    else
    {
        for (size_t i = 0; i < 6; ++i)
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

void DisplayInterface::displaySavingScreen(Adafruit_SSD1306 *display)
{
    display->clearDisplay();

    display->setCursor(0, 0);
    display->setTextSize(2);
    display->print("SAVING... ");

    display->display();
}
