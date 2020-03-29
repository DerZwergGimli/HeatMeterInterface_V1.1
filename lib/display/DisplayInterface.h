#ifndef DisplayInterface_h
#define DisplayInterface_h
#include "Adafruit_SSD1306.h"
#include "ConfigInterface.h"

class DisplayInterface
{
public:
    DisplayInterface();

    void boot(Adafruit_SSD1306 *display);
    void printHello(Adafruit_SSD1306 *display);
    void displayMeter(Adafruit_SSD1306 *display, struct MeterData *meterData);
    int displaySettingsSelectInterface(Adafruit_SSD1306 *display, String buttonState);
    int displaySettingsSelectTempOrRes(Adafruit_SSD1306 *display, String buttonState);
    //int dsiplayConfigInterface(Adafruit_SSD1306 *display, String buttonState, struct MeterData meterData[4]);
    int displayConfigTemperature(Adafruit_SSD1306 *display, String buttonState, struct MeterData meterData[4]);
    int displayConfigResistance();
    void displaySavingScreen(Adafruit_SSD1306 *display);
};

#endif
