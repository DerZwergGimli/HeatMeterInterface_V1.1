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
//Scheduled Task setup
//#define _TASK_ESP32_DLY_THRESHOLD 40L
#define _TASK_SLEEP_ON_IDLE_RUN
#include <TaskScheduler.h>
//Delegates for platform.io
void displayTask_Callback();
void measureTask_Callback();
void readInterface();
void sendDataTask_Callback();
void buttonCheck_Callback();

//WiFiEventHandler connectedWIFIEventHandler, disconnectedWIFIEventHandler;

Scheduler runner;
Task displayTask(100, TASK_FOREVER, &displayTask_Callback, &runner, false);
Task measureTask(500, TASK_FOREVER, &measureTask_Callback, &runner, false);
Task buttonTask(300, TASK_FOREVER, &buttonCheck_Callback, &runner, false);
Task sendDataTask(1000, TASK_FOREVER, &sendDataTask_Callback, &runner, false);

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
//SR_IO sr_io;

#define RMUX_IN A0

//Configuration
ConfigInterface configInterface;
Configuratrion config;
HeaterData heaterData;
MeterData meterData[4];
int displayState = 0;
int counter;
String buttonState = "X";
WiFiClient espClient;
PubSubClient client(espClient);

void setup()
{
  Serial.begin(115200);
  while (!Serial && !Serial.available())
  {
  }
  delay(100);
  Log.begin(LOG_LEVEL_VERBOSE, &Serial);
  Log.notice(F(CR "******************************************" CR));
  Log.notice("***          Starting SYSTEM                " CR);
  Log.notice("*********************** " CR);

  shiftRegisterIO.init();
  shiftRegisterIO.write();
  //shiftRegisterIO.write(&sr_io);
  //shiftRegisterIO.write(&sr_io);

  //pinMode(RMUX_IN, INPUT);

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

  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);
  WiFi.begin(config.wifi_SSID, config.wifi_Password);

  delay(500);
  displayTask.enable();
  measureTask.enable();
  buttonTask.enable();
  sendDataTask.enable();
  runner.startNow();
}

void loop()
{

  if (millis() % 400)
  {
    shiftRegisterIO.led_READY(false);
  }
  else
  {
    shiftRegisterIO.led_READY(true);
    Log.notice("HEAP_SIZE=%i" CR, system_get_free_heap_size());
  }
  runner.execute();
}

void displayTask_Callback()
{
  //shiftRegisterIO.led_READY(false);

  switch (displayState)
  {
  case 0:
    displayInterface.boot(&display);
    displayState++;
    break;

  case 1: //Show Meter 1
    displayInterface.displayMeter(&display, &meterData[0], &heaterData);
    //displayState++;
    displayState = 90;
    break;

  case 2: //Show Meter 2
    displayInterface.displayMeter(&display, &meterData[1], &heaterData);
    displayState++;
    break;

  case 3: //Show Meter 3
    displayInterface.displayMeter(&display, &meterData[2], &heaterData);
    displayState++;
    break;

  case 4: //Show Meter 4
    displayInterface.displayMeter(&display, &meterData[3], &heaterData);
    displayState = 1;
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
}

void measureTask_Callback()
{
  unsigned long start = millis();
  int i = counter;
  if (i <= 4)
  {
    switch (i)
    {
    case 0:
      shiftRegisterIO.led_RJ1(true);
      break;

    case 1:
      shiftRegisterIO.led_RJ2(true);
      break;

    case 2:
      shiftRegisterIO.led_RJ3(true);
      break;

    case 3:
      shiftRegisterIO.led_RJ4(true);
      break;

    default:
      break;
    }
    temperatureInterface.readTemperature(&shiftRegisterIO, thermo, &meterData[i]);

    shiftRegisterIO.checkMeterResistance(&meterData[i]);
    if (meterData[i].waterMeterState)
    {
      switch (i)
      {
      case 0:
        shiftRegisterIO.led_statusRJ1(true);
        break;

      case 1:
        shiftRegisterIO.led_statusRJ2(true);
        break;

      case 2:
        shiftRegisterIO.led_statusRJ3(true);
        break;

      case 3:
        shiftRegisterIO.led_statusRJ4(true);
        break;

      default:
        break;
      }
    }
    else
    {
      switch (i)
      {
      case 0:
        shiftRegisterIO.led_statusRJ1(false);
        break;

      case 1:
        shiftRegisterIO.led_statusRJ2(false);
        break;

      case 2:
        shiftRegisterIO.led_statusRJ3(false);
        break;

      case 3:
        shiftRegisterIO.led_statusRJ4(false);
        break;

      default:
        break;
      }
    }
    if (meterData[i].mux_resistance_edgeDetect)
    {
      meterData[i].water_CounterValue_m3 += 5;
      meterData[i].delta_HeatEnergy_J += 4200 * 5 * (meterData[i].temperature_up_Celcius_mean - meterData[i].temperature_down_Celcius_mean);
      meterData[i].absolute_HeatEnergy_MWh = meterData[i].delta_HeatEnergy_J * 0.000000000277778;
    }

    switch (i)
    {
    case 0:
      shiftRegisterIO.led_RJ1(false);
      break;
    case 1:
      shiftRegisterIO.led_RJ2(false);
      break;
    case 2:
      shiftRegisterIO.led_RJ3(false);
      break;
    case 3:
      shiftRegisterIO.led_RJ4(false);
      break;

    default:
      break;
    }
    counter++;
  }

  if (counter >= 4)
  {
    counter = 0;
  }

  shiftRegisterIO.checkForVoltage(&heaterData);

  unsigned long end = millis();
  unsigned long duration = end - start;
  //Serial.print("Duration:");
  //Serial.println(duration);
}

void sendDataTask_Callback()
{
  //measureTask.disable();

  if (WiFi.status() == WL_CONNECTED)
  {
    shiftRegisterIO.led_WIFI(true);

    if (!client.connected())
    {
      Log.notice("Connecting to MQTT..." CR);
      client.setServer(config.mqtt_ServerAddress, config.mqtt_Port);

      if (client.connect(config.name))
      {
        Log.notice("Connected" CR);
      }
      else
      {
        Log.error("Failed to connect MQTT with State: %i" CR, client.state());

        shiftRegisterIO.led_ERROR(true);
      }
    }
    else
    {
      // if MQTT connected
      Log.notice("Sending MQTT Topics" CR);

      char topic[100];
      char message[100];

      for (size_t i = 0; i < 4; i++)
      {
        sprintf(topic, "%s/%d/water_CounterValue_m3", config.name, meterData[i].meterID);
        sprintf(message, "%f", meterData[i].water_CounterValue_m3);
        client.publish(topic, message);

        sprintf(topic, "%s/%d/absolute_HeatEnergy_MWh", config.name, meterData[i].meterID);
        sprintf(message, "%f", meterData[i].absolute_HeatEnergy_MWh);
        client.publish(topic, message);

        sprintf(topic, "%s/%d/temperature_up_Celcius", config.name, meterData[i].meterID);
        sprintf(message, "%0.2f", meterData[i].temperature_up_Celcius);
        client.publish(topic, message);

        sprintf(topic, "%s/%d/temperature_down_Celcius", config.name, meterData[i].meterID);
        sprintf(message, "%0.2f", meterData[i].temperature_down_Celcius);

        sprintf(topic, "%s/%d/state", config.name, meterData[i].meterID);
        sprintf(message, "%i", meterData[i].ledState);
        client.publish(topic, message);
      }

      sprintf(topic, "%s/%s/state", config.name, heaterData.name);
      sprintf(message, "%i", heaterData.state);
      client.publish(topic, message);

      sprintf(topic, "%s/%s/runtime_on_current", config.name, heaterData.name);
      sprintf(message, "%li", heaterData.runtime_on_current);
      client.publish(topic, message);

      sprintf(topic, "%s/%s/runtime_off_current", config.name, heaterData.name);
      sprintf(message, "%li", heaterData.runtime_off_current);
      client.publish(topic, message);

      sprintf(topic, "%s/%s/runtime_on_previous", config.name, heaterData.name);
      sprintf(message, "%li", heaterData.runtime_on_previous);
      client.publish(topic, message);

      sprintf(topic, "%s/%s/runtime_off_previous", config.name, heaterData.name);
      sprintf(message, "%li", heaterData.runtime_off_previous);
      client.publish(topic, message);

      sprintf(topic, "%s/info/freeRam", config.name);
      sprintf(message, "%i", (int)system_get_free_heap_size);
      client.publish(topic, message);

      client.loop();
    }
  }
  else
  {
    Log.error("No WIFI connection" CR);
    shiftRegisterIO.led_WIFI(false);
    shiftRegisterIO.led_ERROR(false);
  }
  //measureTask.enable();
  shiftRegisterIO.led_WIFI(false);
}

void buttonCheck_Callback()
{
  //shiftRegisterIO.led_READY(false);
  //buttonState = "X";

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
  if ((displayState == (1 || 2 || 3 || 4)) && (buttonState == "SELECT"))
  {
    displayState = 5;
    buttonState = "X";
  }
}