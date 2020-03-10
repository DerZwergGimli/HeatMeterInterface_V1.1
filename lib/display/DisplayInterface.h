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
};

#endif
