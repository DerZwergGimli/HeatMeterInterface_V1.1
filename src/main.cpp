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
#include <MQTT.h>
#define _TASK_SLEEP_ON_IDLE_RUN
#include <TaskScheduler.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

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
Task displayTask(300, TASK_FOREVER, &displayDisplay_Callback, &runner, false);
Task buttonTask(300, TASK_FOREVER, &measureButtonState_Callback, &runner, false);
Task temperatureTask(1000, TASK_FOREVER, &measureInterfaceTemperature_Callback, &runner, false);
Task resistanceTask(5000, TASK_FOREVER, &measureInterfaceResistance_Callback, &runner, false);
Task voltageTask(1000, TASK_FOREVER, &measureInterfaceVoltage_Callback, &runner, false);
Task mqttTask(5000, TASK_FOREVER, &sendMQTT_Callback, &runner, false);

// Display Configuration
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET -1    // Reset pin # (or -1 if sharing Arduino reset pin)
DisplayInterface displayInterface;

//PT100 Configurartion
Adafruit_MAX31865 thermo = Adafruit_MAX31865(D8, D7, D6, D5);
TemperatureInterface temperatureInterface;

// ShiftRegister Configuration
ShiftRegisterIO shiftRegisterIO;

//Configuration
ConfigInterface configInterface;
Configuration config;
HeaterData heaterData;
MeterData meterData[4];
int displayState = 0;
int counter;
ButtonState buttonState;
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

  shiftRegisterIO.init();
  shiftRegisterIO.write();

  shiftRegisterIO.toggleDisplay(true);

  configInterface.init();
  configInterface.loadConfig(&config, meterData, &heaterData);

  displayInterface.init(&config, meterData, &heaterData, &buttonState);
  thermo.begin(MAX31865_2WIRE);
  mqtt.init();

  displayTask.setInterval(config.task_Display_Interval);
  temperatureTask.setInterval(config.task_Temperature_Interval);
  resistanceTask.setInterval(config.task_Resistance_Interval);
  voltageTask.setInterval(config.task_Voltage_Interval);
  mqttTask.setInterval(config.task_MQTT_Interval);

  displayTask.enable();
  buttonTask.enable();
  temperatureTask.enable();
  resistanceTask.enable();
  voltageTask.enable();
  mqttTask.enable();
  Log.notice("---- DEVICE BOOTED ----" CR);
  runner.startNow();
}

void loop()
{
  runner.execute();
}

void displayDisplay_Callback()
{
  shiftRegisterIO.led_READY(true);
  shiftRegisterIO.led_READY(false);
  shiftRegisterIO.toggleDisplay(true);
  displayInterface.displayStateMachine();
}

void measureButtonState_Callback()
{
  if (shiftRegisterIO.checkButton("UP") == "UP")
  {
    Log.notice("UP_Buton_Pressed");
    buttonState = up;
  }

  if (shiftRegisterIO.checkButton("DOWN") == "DOWN")
  {
    Log.notice("DOWN_Buton_Pressed");
    buttonState = down;
  }

  if (shiftRegisterIO.checkButton("SELECT") == "SELECT")
  {
    Log.notice("SELECT_Button_Presses");
    buttonState = select;
  }
}
void measureInterfaceTemperature_Callback()
{
  switchInterfaceLED(temperatureInterface_counter, true);
  temperatureInterface.readTemperature(&shiftRegisterIO, thermo, &meterData[temperatureInterface_counter]);
  Log.notice("--> Interface_%i Temperature CHECKED" CR, temperatureInterface_counter);
  switchInterfaceLED(temperatureInterface_counter, false);

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
    meterData[resistanceInterface_counter].counterValue_m3 += (5.0 / 1000.0);
    meterData[resistanceInterface_counter].counterValue_m3_delta += (5.0 / 1000.0);
    float deltaEnergy = (4200.0 * 5.0 / 1000.0 * (meterData[resistanceInterface_counter].temperature_up_Celcius_mean - meterData[resistanceInterface_counter].temperature_down_Celcius_mean));
    meterData[resistanceInterface_counter].counterValue_J += deltaEnergy;
    meterData[resistanceInterface_counter].counterValue_J_delta += deltaEnergy;
    meterData[resistanceInterface_counter].counterValue_MWh += (deltaEnergy * 0.000000000277778);
    meterData[resistanceInterface_counter].counterValue_MWh += (deltaEnergy * 0.000000000277778);
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
  if (resistanceInterface_counter >= 4)
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
