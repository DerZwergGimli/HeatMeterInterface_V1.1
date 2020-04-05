#ifndef DisplayInterface_h
#define DisplayInterface_h
#include "Adafruit_SSD1306.h"
#include "ConfigInterface.h"

class DisplayInterface
{
public:
    DisplayInterface();

    void init(struct Configuration *config, struct MeterData *meterData, struct HeaterData *heaterData, ButtonState *buttonState);
    void displayBootMessage();
    void displayTimer(int nextState);

    void displayStateMachine();

    void displayMeter(int interface);
    void displayHeater();

    int displaySettingsSelectInterface(Adafruit_SSD1306 *display, String buttonState);
    int displaySettingsSelectTempOrRes(Adafruit_SSD1306 *display, String buttonState);
    //int dsiplayConfigInterface(Adafruit_SSD1306 *display, String buttonState, struct MeterData meterData[4]);
    int displayConfigTemperature(Adafruit_SSD1306 *display, String buttonState, struct MeterData meterData[4]);
    int displayConfigResistance(Adafruit_SSD1306 *display, String buttonState, struct MeterData meater[4]);
    void displaySavingScreen(Adafruit_SSD1306 *display);

    void menu_EditSave();
    void menu_Select_TempRes();
    void menu_Select_Voltage();
    void menu_Edit_Temperature();
    void menu_Edit_Resistance();
    void menu_Edit_Voltage();

private:
    void printWithLeadingZeros(float number);
};

#endif
