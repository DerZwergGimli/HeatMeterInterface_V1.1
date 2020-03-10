#include <Wire.h>
#include <Arduino.h>
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
void sendDataTask_Callback();

void readyLED(bool state);

WiFiEventHandler connectedWIFIEventHandler, disconnectedWIFIEventHandler;

Scheduler runner;
Task displayTask(1100, TASK_FOREVER, &displayTask_Callback, &runner, true);
Task measureTask(700, TASK_FOREVER, &measureTask_Callback, &runner, true);
//Task sendDataTask(10000, TASK_FOREVER, &sendDataTask_Callback, &runner, false);

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
#define RREF 240.0
#define RNOMINAL 100.0

// ShiftRegister Configuration
ShiftRegisterIO shiftRegisterIO;
SR_IO sr_io;

#define RMUX_IN A0

//Configuration
ConfigInterface configInterface;
Configuratrion config;
MeterData meterData[4];
int displayState = 0;

void setup()
{
  Serial.begin(115200);
  Serial.print("Starting...");
  delay(100);

  shiftRegisterIO.init();
  shiftRegisterIO.write(&sr_io);
  shiftRegisterIO.write(&sr_io);

  //pinMode(RMUX_IN, INPUT);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      Serial.print("Error while connecting to Display"); // Don't proceed, loop forever
  }

  displayInterface.boot(&display);

  configInterface.init();
  configInterface.loadConfig(&config, meterData);

  temperatureInterface.init(thermo);

  //runner.init();
  //runner.addTask(displayTask);
  //runner.addTask(measureTask);
  // connectedWIFIEventHandler = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event) {
  //   Serial.print("Station connected, IP: ");
  //   Serial.println(WiFi.localIP());
  //   sendDataTask.enable();
  // });

  // disconnectedWIFIEventHandler = WiFi.onStationModeDisconnected([](const WiFiEventStationModeDisconnected &event) {
  //   Serial.println("Station disconnected");
  //   sendDataTask.disable();
  // });

  // WiFi.begin(config.wifi_SSID, config.wifi_Password);

  delay(1000);
  //displayTask.enable();
  //measureTask.enable();
  //shiftRegisterIO.led_READY(&shiftRegisterIO, &sr_io, true);
  runner.startNow();

  //shiftRegisterIO.ledBlink(1000);
}

void loop()
{
  //shiftRegisterIO.led_READY(&shiftRegisterIO, &sr_io, true);

  runner.execute();

  //1. Display
  //displayInterface.displayMeter(&display, &meterData[0]);
  //2.1 Take Measurements
  //2.2 Calculate
  //3. Send Measuremnts
  //4. check user input
  // do all over again

  //display.clearDisplay();
  //display.setCursor(10, 10);

  // int analogValue = analogRead(RMUX_IN);
  // Serial.print("Analaog: ");
  // Serial.println(String(analogValue));
  // //display.println(analogValue);

  // channel_RJ1.temperature_up_Celcius = temperatureInterface.readTemperature(thermo, RNOMINAL, config.RREF_RJ1_T1, true);

  // sr_io = shiftRegisterIO.t_MuxSelect(sr_io, 7);
  // shiftRegisterIO.write(sr_io);
  // channel_RJ1.temperature_down_Celcius = temperatureInterface.readTemperature(&thermo, RNOMINAL, config.RREF_RJ1_T2, true);

  // meterData[0].mux_up = 5;
  // meterData[0].mux_down = 7;
  // Serial.print(meterData[0].RREF_up);
  // //meterData[0].RREF_up = 240;
  //meterData[0].RREF_down = 240;
  //temperatureInterface.readTemperature(thermo, &sr_io, &meterData[0]);

  // Serial.print("UP ");
  // Serial.println(String(meterData[0].temperature_up_Celcius));
  // Serial.print("DOWN ");
  // Serial.println(String(meterData[0].temperature_down_Celcius));

  //readTemperature(0, 0, true);
  //sr_io = shiftRegisterIO.t_MuxSelect(sr_io, -1);
  //shiftRegisterIO.write(sr_io);

  //display.display();
  //delay(1000);
}

void displayTask_Callback()
{
  shiftRegisterIO.led_READY(&shiftRegisterIO, &sr_io, false);
  switch (displayState)
  {
  case 0:
    displayInterface.boot(&display);
    displayState++;
    break;

  case 1: //Show Meter 1
    displayInterface.displayMeter(&display, &meterData[0]);
    //displayState++;
    break;

  case 2: //Show Meter 2
    displayInterface.displayMeter(&display, &meterData[1]);
    displayState++;
    break;

  case 3: //Show Meter 3
    displayInterface.displayMeter(&display, &meterData[2]);
    displayState++;
    break;

  case 4: //Show Meter 4
    displayInterface.displayMeter(&display, &meterData[3]);
    displayState = 1;
    break;

  default:
    break;
  }
}

void measureTask_Callback()
{
  shiftRegisterIO.led_READY(&shiftRegisterIO, &sr_io, false);

  unsigned long start = millis();

  shiftRegisterIO.led_RJ1(&shiftRegisterIO, &sr_io, true);
  temperatureInterface.readTemperature(thermo, &sr_io, &meterData[0]);
  shiftRegisterIO.checkMeterResistance(&shiftRegisterIO, &sr_io, &meterData[0]);
  if (meterData[0].waterMeterState)
  {
    shiftRegisterIO.led_statusRJ1(&shiftRegisterIO, &sr_io, true);
  }
  else
  {
    shiftRegisterIO.led_statusRJ1(&shiftRegisterIO, &sr_io, false);
  }
  if (meterData[0].mux_resistance_edgeDetect)
  {
    meterData->water_CounterValue_m3 += 5;
    meterData->delta_HeatEnergy_J += 4200 * 5 * (meterData->temperature_up_Celcius_mean - meterData->temperature_down_Celcius_mean);
    meterData->absolute_HeatEnergy_MWh = meterData->delta_HeatEnergy_J * 0.000000000277778;
    Serial.println(meterData->absolute_HeatEnergy_MWh);
  }

  shiftRegisterIO.led_RJ1(&shiftRegisterIO, &sr_io, false);

  //shiftRegisterIO.led_RJ2(&shiftRegisterIO, &sr_io, true);
  //temperatureInterface.readTemperature(thermo, &sr_io, &meterData[1]);
  //shiftRegisterIO.led_RJ2(&shiftRegisterIO, &sr_io, false);

  //shiftRegisterIO.led_RJ3(&shiftRegisterIO, &sr_io, true);
  //temperatureInterface.readTemperature(thermo, &sr_io, &meterData[2]);
  //shiftRegisterIO.led_RJ3(&shiftRegisterIO, &sr_io, false);

  //shiftRegisterIO.led_RJ4(&shiftRegisterIO, &sr_io, true);
  //temperatureInterface.readTemperature(thermo, &sr_io, &meterData[3]);
  //shiftRegisterIO.led_RJ4(&shiftRegisterIO, &sr_io, false);

  unsigned long end = millis();
  unsigned long duration = end - start;
  Serial.print("Duration:");
  Serial.println(duration);
}

void sendDataTask_Callback()
{

  measureTask.disable();
  WiFiClient espClient;
  PubSubClient client(espClient);

  // WiFi.forceSleepWake();

  // WiFi.mode(WIFI_STA);

  // WiFi.begin(config.wifi_SSID, config.wifi_Password);

  // Serial.print("Connecting to WIFI");
  // while (WiFi.status() != WL_CONNECTED)
  // {
  //   shiftRegisterIO.led_WIFI(&shiftRegisterIO, &sr_io, true);

  //   unsigned long timestamp = millis();
  //   //while (millis() <= timestamp + 500)
  //   //{
  //   //  ;
  //   //}
  //   //delay(100);
  //   delay(500);
  //   Serial.print(".");

  //   shiftRegisterIO.led_WIFI(&shiftRegisterIO, &sr_io, false);
  //   //system_soft_wdt_feed();
  //   //ESP.wdtFeed();
  //   //system_soft_wdt_feed();
  //   //wdt_reset();
  //   //ESP.wdtFeed();
  // }
  //Serial.println("Connected to the WiFi network");

  shiftRegisterIO.led_WIFI(&shiftRegisterIO, &sr_io, true);
  client.setServer(config.mqtt_ServerAddress, config.mqtt_Port);
  //client.setCallback(callback

  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (client.connect(config.name))
    {

      Serial.println("connected");
    }
    else
    {

      Serial.print("failed with state ");
      Serial.print(client.state());
      shiftRegisterIO.led_ERROR(&shiftRegisterIO, &sr_io, true);
      //delay(2000);
    }

    client.publish("esp/test", "Hello from ESP8266");

    //char *s = ""; // initialized properly
    //char *s;
    //sprintf(char *s, "%s/temperature_down_Celcius", config.name);
    //Serial.print(s);
    char topic[100];
    char message[100];

    sprintf(topic, "%s/%d/water_CounterValue_m3", config.name, meterData[0].meterID);
    sprintf(message, "%f", meterData[0].water_CounterValue_m3);
    client.publish(topic, message);

    sprintf(topic, "%s/%d/absolute_HeatEnergy_MWh", config.name, meterData[0].meterID);
    sprintf(message, "%f", meterData[0].absolute_HeatEnergy_MWh);
    client.publish(topic, message);

    sprintf(topic, "%s/%d/temperature_up_Celcius", config.name, meterData[0].meterID);
    sprintf(message, "%0.2f", meterData[0].temperature_up_Celcius);
    client.publish(topic, message);

    sprintf(topic, "%s/%d/temperature_down_Celcius", config.name, meterData[0].meterID);
    sprintf(message, "%0.2f", meterData[0].temperature_down_Celcius);
    client.publish(topic, message);
  }
  client.disconnect();
  //wifi_set_sleep_type(LIGHT_SLEEP_T);
  //WiFi.disconnect();
  //WiFi.mode(WIFI_OFF);
  //WiFi.disconnect();
  //WiFi.forceSleepBegin();

  measureTask.enable();

  shiftRegisterIO.led_WIFI(&shiftRegisterIO, &sr_io, false);

  shiftRegisterIO.led_ERROR(&shiftRegisterIO, &sr_io, false);
}

void readyLED(bool state)
{
  sr_io.LED_Ready = state;
  shiftRegisterIO.write(&sr_io);
}