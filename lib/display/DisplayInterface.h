#ifndef DisplayInterface_h
#define DisplayInterface_h
#include "Adafruit_SSD1306.h"
#include "ConfigInterface.h"

class DisplayInterface
{
public:
    DisplayInterface();

    void init();
    void displayBootMessage();
    void displayTimer(int nextState);

    void displayStateMachine(struct MeterData *meterData, struct HeaterData *heaterData);

    void displayMeter(int interface);
    void displayHeater();

    int displaySettingsSelectInterface(Adafruit_SSD1306 *display, String buttonState);
    int displaySettingsSelectTempOrRes(Adafruit_SSD1306 *display, String buttonState);
    //int dsiplayConfigInterface(Adafruit_SSD1306 *display, String buttonState, struct MeterData meterData[4]);
    int displayConfigTemperature(Adafruit_SSD1306 *display, String buttonState, struct MeterData meterData[4]);
    int displayConfigResistance(Adafruit_SSD1306 *display, String buttonState, struct MeterData meater[4]);
    void displaySavingScreen(Adafruit_SSD1306 *display);

private:
    void printWithLeadingZeros(float number);
};

#endif
