#include <Wire.h>
#include <Arduino.h>
#include <ArduinoLog.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "ShiftRegisterIO.h"
#include "ConfigInterface.h"
#include "TemperatureInterface.h"
#include "DisplayInterface.h"
#include <Adafruit_MAX31865.h>
#include <Esp.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MQTT.h>
#define _TASK_SLEEP_ON_IDLE_RUN
#include <TaskScheduler.h>

void switchInterfaceLED(int i, bool state);
void switchInterfaceLED_External(int i, bool state);

void displayDisplay_Callback();
void measureButtonState_Callback();
void measureInterfaceTemperature_Callback();
void measureInterfaceResistance_Callback();
void measureInterfaceVoltage_Callback();
void sendMQTT_Callback();

//Task Config
Scheduler runner;
Task displayTask(200, TASK_FOREVER, &displayDisplay_Callback, &runner, false);
Task buttonTask(200, TASK_FOREVER, &measureButtonState_Callback, &runner, false);
Task temperatureTask(10000, TASK_FOREVER, &measureInterfaceTemperature_Callback, &runner, false);
Task resistanceTask(50, TASK_FOREVER, &measureInterfaceResistance_Callback, &runner, false);
Task voltageTask(1000, TASK_FOREVER, &measureInterfaceVoltage_Callback, &runner, false);
Task mqttTask(5000, TASK_FOREVER, &sendMQTT_Callback, &runner, false);

// Display Configuration
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
DisplayInterface displayInterface;

//PT100 Configurartion
Adafruit_MAX31865 thermo = Adafruit_MAX31865(D8, D7, D6, D5);
TemperatureInterface temperatureInterface;
//#define RREF 430.0  old Version

// ShiftRegister Configuration
ShiftRegisterIO shiftRegisterIO;
//#define RMUX_IN A0

//Configuration
ConfigInterface configInterface;
Configuration config;
HeaterData heaterData;
MeterData meterData[4];
int displayState = 0;
int counter;
String buttonState = "X";
WiFiClient espClient;
PubSubClient client(espClient);
MQTT mqtt;
int displayCounterTime = 0;
int displayCounterTime_max = 10;

int temperatureInterface_counter;
int resistanceInterface_counter;

void setup()
{
  Serial.begin(115200);
  while (!Serial && !Serial.available())
  {
  }
  delay(100);
  
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.notice("******************************************" CR);
  Log.notice("******************************************" CR);
  Log.notice("***          Starting SYSTEM           ***" CR);
  Log.notice("******************************************" CR);

  shiftRegisterIO.init();
  shiftRegisterIO.write();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Log.error((F("SSD1306 allocation failed")));
    for (;;)
      Log.fatal("Error while connecting to Display");
  }

  displayInterface.boot(&display);

  configInterface.init();
  configInterface.loadConfig(&config, meterData);
  thermo.begin(MAX31865_2WIRE);
  mqtt.init();

  delay(500);
  displayTask.enable();
  buttonTask.enable();
  temperatureTask.enable();
  resistanceTask.enable();
  voltageTask.enable();
  mqttTask.enable();
  runner.startNow();
}

void loop()
{
  runner.execute();
}

void displayDisplay_Callback()
{
  shiftRegisterIO.led_READY(true);
  switch (displayState)
  {
  case 0:
    displayInterface.boot(&display);
    displayState++;
    break;

  case 1: //Show Meter 1
    displayInterface.displayMeter(&display, &meterData[0], &heaterData);
    if (displayCounterTime >= displayCounterTime_max)
    {
      //displayState++;
      displayCounterTime = 0;
      displayState = 90;
    }
    else
    {
      displayCounterTime++;
    }
    break;

  case 2: //Show Meter 2
    displayInterface.displayMeter(&display, &meterData[1], &heaterData);
    if (displayCounterTime >= displayCounterTime_max)
    {
      displayState++;
      displayCounterTime = 0;
    }
    else
    {
      displayCounterTime++;
    }
    break;

  case 3: //Show Meter 3
    displayInterface.displayMeter(&display, &meterData[2], &heaterData);
    if (displayCounterTime >= displayCounterTime_max)
    {
      displayState++;
      displayCounterTime = 0;
    }
    else
    {
      displayCounterTime++;
    }
    break;

  case 4: //Show Meter 4
    displayInterface.displayMeter(&display, &meterData[3], &heaterData);
    if (displayCounterTime >= displayCounterTime_max)
    {
      displayState = 90;
      displayCounterTime = 0;
    }
    else
    {
      displayCounterTime++;
    }
    break;
  case 5: //Show SettingsMain
    displayState = displayInterface.displaySettingsSelectInterface(&display, buttonState);
    buttonState = "X";
    break;
  case 6: // Select wether to edit Temp or Resistance
    displayState = displayInterface.displaySettingsSelectTempOrRes(&display, buttonState);
    buttonState = "X";
    break;
  case 7: // Edit values for data aquire
    displayState = displayInterface.displayConfigTemperature(&display, buttonState, meterData);
    buttonState = "X";
    break;
  case 90: //Show heater_data
    displayInterface.displayHeater(&display, &heaterData);
    if (displayCounterTime >= displayCounterTime_max)
    {
      displayState = 1;
      displayCounterTime = 0;
    }
    else
    {
      displayCounterTime++;
    }
    break;
  case 91: //Config heater_data
    displayState = displayInterface.displayConfigResistance(&display, buttonState, meterData);
    buttonState = "X";
    break;
  case 100: // Show screen when saving data back to flash
    displayInterface.displaySavingScreen(&display);
    configInterface.saveConfig(&config, meterData);
    displayState = 1;
    break;
  default:
    displayState = 0;
    break;
  }
  shiftRegisterIO.led_READY(false);
}

void measureButtonState_Callback()
{
  if (shiftRegisterIO.checkButton("UP") == "UP")
  {
    Log.notice("UP_Buton_Pressed");
    buttonState = "UP";
  }

  if (shiftRegisterIO.checkButton("DOWN") == "DOWN")
  {
    Log.notice("DOWN_Buton_Pressed");
    buttonState = "DOWN";
  }

  if (shiftRegisterIO.checkButton("SELECT") == "SELECT")
  {
    Log.notice("SELECT_Button_Presses");
    buttonState = "SELECT";
  }

  // Go into Settings
  if (((displayState == 1) || (displayState == 2) || (displayState == 3) || (displayState == 4) || (displayState == 90)) && (buttonState == "SELECT"))
  {
    displayState = 5;
    buttonState = "X";
  }
}
void measureInterfaceTemperature_Callback()
{
  switchInterfaceLED(temperatureInterface_counter, true);
  temperatureInterface.readTemperature(&shiftRegisterIO, thermo, &meterData[temperatureInterface_counter]);
  Log.notice("--> Interface_%i Temperature CHECKED" CR, temperatureInterface_counter);
  switchInterfaceLED(temperatureInterface_counter, true);

  temperatureInterface_counter++;
  if (temperatureInterface_counter > 3)
  {
    temperatureInterface_counter = 0;
  }
}
void measureInterfaceResistance_Callback()
{
  switchInterfaceLED(resistanceInterface_counter, true);
  shiftRegisterIO.checkMeterResistance(&meterData[resistanceInterface_counter]);

  if (meterData[resistanceInterface_counter].mux_resistance_edgeDetect)
  {
    meterData[resistanceInterface_counter].water_CounterValue_m3 += 5;
    meterData[resistanceInterface_counter].delta_HeatEnergy_J += 4200 * 5 * (meterData[resistanceInterface_counter].temperature_up_Celcius_mean - meterData[resistanceInterface_counter].temperature_down_Celcius_mean);
    meterData[resistanceInterface_counter].absolute_HeatEnergy_MWh = meterData[resistanceInterface_counter].delta_HeatEnergy_J * 0.000000000277778;
  }
  if (meterData[resistanceInterface_counter].waterMeterState)
  {
    switchInterfaceLED_External(resistanceInterface_counter, true);
  }
  else
  {
    switchInterfaceLED_External(resistanceInterface_counter, false);
  }

  switchInterfaceLED(resistanceInterface_counter, false);
  Log.notice("--> Interface_%i Resistance CHECKED" CR, resistanceInterface_counter);

  resistanceInterface_counter++;
  if (resistanceInterface_counter > 3)
  {
    resistanceInterface_counter = 0;
  }
}
void measureInterfaceVoltage_Callback()
{
  shiftRegisterIO.checkForVoltage(&heaterData);
  Log.notice("--> Voltage CHECKED" CR);
}
void sendMQTT_Callback()
{
  mqtt.send(&shiftRegisterIO, &config, meterData, &heaterData);
}

void switchInterfaceLED(int i, bool state)
{
  switch (i)
  {
  case 0:
    shiftRegisterIO.led_RJ1(state);
    break;

  case 1:
    shiftRegisterIO.led_RJ2(state);
    break;

  case 2:
    shiftRegisterIO.led_RJ3(state);
    break;

  case 3:
    shiftRegisterIO.led_RJ4(state);
    break;

  default:
    break;
  }
}

void switchInterfaceLED_External(int i, bool state)
{
  switch (i)
  {
  case 0:
    shiftRegisterIO.led_statusRJ1(state);
    break;

  case 1:
    shiftRegisterIO.led_statusRJ2(state);
    break;

  case 2:
    shiftRegisterIO.led_statusRJ3(state);
    break;

  case 3:
    shiftRegisterIO.led_statusRJ4(state);
    break;

  default:
    break;
  }
}
