#ifndef DisplayInterface_h
#define DisplayInterface_h
#include "Adafruit_SSD1306.h"
#include "ConfigInterface.h"

enum DisplayState
{
    display_Init,
    display_Boot,
    display_Interface1,
    display_Interface2,
    display_Interface3,
    display_Interface4,
    display_InterfaceHeater,
    display_menue_main,
    display_menue_select_SaveEdit,
    display_menue_select_TempOrRes,
    display_menue_select_Voltage,
    display_menue_select_Interface,
    display_menue_edit_Temperature,
    display_menue_edit_Resistance,
    display_menue_edit_Voltage,
    display_menue_edit_InterfacHeater,
    displaySave,
};

enum MenueDisplayState
{

};

class DisplayInterface
{
public:
    DisplayInterface();

    void init(struct Configuration *config, struct MeterData *meterData, struct HeaterData *heaterData, ButtonState *buttonState);
    void displayBootMessage();

    void displayTimer(DisplayState next_displayState);

    void displayStateMachine();

    void displayMeter(int interface);
    void displayHeater();

    int displaySettingsSelectInterface(Adafruit_SSD1306 *display, String buttonState);
    int displaySettingsSelectTempOrRes(Adafruit_SSD1306 *display, String buttonState);
    //int dsiplayConfigInterface(Adafruit_SSD1306 *display, String buttonState, struct MeterData meterData[4]);
    int displayConfigTemperature(Adafruit_SSD1306 *display, String buttonState, struct MeterData meterData[4]);
    int displayConfigResistance(Adafruit_SSD1306 *display, String buttonState, struct MeterData meater[4]);
    void displaySavingScreen(Adafruit_SSD1306 *display);

    int update_menue();
    int check_bounds(int max, int min, int value);
    void menu_EditSave();
    void menu_selectInterface();
    void menu_Select_TempRes();
    void menu_Select_Voltage();
    void menu_Edit_Temperature();
    void menu_Edit_Resistance();
    void menu_Edit_Voltage();

private:
    void printWithLeadingZeros(float number);
};

#endif
