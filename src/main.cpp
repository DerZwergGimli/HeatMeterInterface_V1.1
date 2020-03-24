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
void buttonCheck_Callback();
void readyLED(bool state);

//WiFiEventHandler connectedWIFIEventHandler, disconnectedWIFIEventHandler;

Scheduler runner;
Task displayTask(150, TASK_FOREVER, &displayTask_Callback, &runner, false);
Task measureTask(500, TASK_FOREVER, &measureTask_Callback, &runner, false);
Task buttonTask(333, TASK_FOREVER, &buttonCheck_Callback, &runner, false);
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

// ShiftRegister Configuration
ShiftRegisterIO shiftRegisterIO;
//SR_IO sr_io;

#define RMUX_IN A0

//Configuration
ConfigInterface configInterface;
Configuratrion config;
MeterData meterData[4];
int displayState = 0;
String buttonState = "X";

void setup()
{
  Serial.begin(115200);
  Serial.print("Starting...");
  delay(100);

  shiftRegisterIO.init();
  shiftRegisterIO.write();
  //shiftRegisterIO.write(&sr_io);
  //shiftRegisterIO.write(&sr_io);

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
  thermo.begin(MAX31865_2WIRE);
  //temperatureInterface.init(thermo);

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

  //displayTask.enable();
  //measureTask.enable();
  //shiftRegisterIO.led_READY(&shiftRegisterIO, &sr_io, true);
  delay(500);
  displayTask.enable();
  measureTask.enable();
  buttonTask.enable();
  //runner.startNow();

  //shiftRegisterIO.ledBlink(1000);
}

void loop()
{
  shiftRegisterIO.led_READY(true);
  runner.execute();

  // digitalWrite(shiftRegister_Latch, LOW);
  // digitalWrite(shiftRegister_Clock, LOW);
  // digitalWrite(shiftRegister_IN, LOW);
  // digitalWrite(shiftRegister_Clock, HIGH);
  // digitalWrite(shiftRegister_Latch, HIGH);

  // delay(3000);
  // digitalWrite(shiftRegister_Latch, LOW);
  // digitalWrite(shiftRegister_Clock, LOW);
  // digitalWrite(shiftRegister_IN, HIGH);
  // digitalWrite(shiftRegister_Clock, HIGH);
  // digitalWrite(shiftRegister_Latch, HIGH);
  // delay(3000);

  //shiftRegisterIO.allOne();
  //shiftRegisterIO.led_READY(&shiftRegisterIO, &sr_io, true);

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
  //shiftRegisterIO.led_READY(false);

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
  case 5: //Show SettingsMain
    displayState = displayInterface.displaySettingsMain(&display, buttonState);
    buttonState = "X";
    break;
  case 6: // Edit values for data aquire
    displayState = displayInterface.dsiplayConfigInterface(&display, buttonState, meterData);
    buttonState = "X";
    break;
  case 100: // Show screen when saving data back to flash
    displayInterface.displaySavingScreen(&display);
    configInterface.saveConfig(&config, meterData);
    displayState = 1;
    break;

  default:
    break;
  }
}

void measureTask_Callback()
{
  //shiftRegisterIO.led_READY(false);

  unsigned long start = millis();

  shiftRegisterIO.led_RJ1(true);

  float RNOMINAL = 100.0;

  // UP
  shiftRegisterIO.t_MuxSelect(meterData[0].mux_up);
  shiftRegisterIO.write();

  float temp_up = thermo.temperature(RNOMINAL, meterData[0].RREF_up);
  if (meterData[0].temperature_up_Celcius_mean == 0)
  {
    meterData[0].temperature_up_Celcius_mean = temp_up;
    meterData[0].temperature_up_Celcius_sum = 0;
    meterData[0].temperature_up_Celcius_numberOfPoints = 0;
  }
  meterData[0].temperature_up_Celcius = temp_up;
  meterData[0].temperature_up_Celcius_sum += temp_up;
  meterData[0].temperature_up_Celcius_numberOfPoints++;
  meterData[0].temperature_up_Celcius_mean = meterData[0].temperature_up_Celcius_sum / meterData[0].temperature_up_Celcius_numberOfPoints;

  //      DOWN
  shiftRegisterIO.t_MuxSelect(meterData[0].mux_down);
  shiftRegisterIO.write();

  float temp_down = thermo.temperature(RNOMINAL, meterData[0].RREF_down);
  if (meterData[0].temperature_down_Celcius_mean == 0)
  {
    meterData[0].temperature_down_Celcius_mean = temp_down;
    meterData[0].temperature_down_Celcius_sum = 0;
    meterData[0].temperature_down_Celcius_numberOfPoints = 0;
  }

  meterData[0].temperature_down_Celcius = temp_down;
  meterData[0].temperature_down_Celcius_sum += temp_down;
  meterData[0].temperature_down_Celcius_numberOfPoints++;
  meterData[0].temperature_down_Celcius_mean = meterData[0].temperature_down_Celcius_sum / meterData[0].temperature_down_Celcius_numberOfPoints;

  shiftRegisterIO.checkMeterResistance(&meterData[0]);
  if (meterData[0].waterMeterState)
  {
    shiftRegisterIO.led_statusRJ1(true);
  }
  else
  {
    shiftRegisterIO.led_statusRJ1(false);
  }
  if (meterData[0].mux_resistance_edgeDetect)
  {
    meterData[0].water_CounterValue_m3 += 5;
    meterData[0].delta_HeatEnergy_J += 4200 * 5 * (meterData[0].temperature_up_Celcius_mean - meterData[0].temperature_down_Celcius_mean);
    meterData[0].absolute_HeatEnergy_MWh = meterData[0].delta_HeatEnergy_J * 0.000000000277778;
    //Serial.println(meterData->absolute_HeatEnergy_MWh);
  }
  shiftRegisterIO.led_RJ1(false);

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

  //shiftRegisterIO.led_WIFI(&shiftRegisterIO, &sr_io, true);
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
      shiftRegisterIO.led_ERROR(true);
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

  shiftRegisterIO.led_WIFI(false);

  shiftRegisterIO.led_ERROR(false);
}

void buttonCheck_Callback()
{
  //shiftRegisterIO.led_READY(false);
  //buttonState = "X";

  if (shiftRegisterIO.checkButton("UP") == "UP")
  {
    Serial.println("UP_Buton_Pressed");
    buttonState = "UP";
  }

  if (shiftRegisterIO.checkButton("DOWN") == "DOWN")
  {
    Serial.println("DOWN_Buton_Pressed");
    buttonState = "DOWN";
  }

  if (shiftRegisterIO.checkButton("SELECT") == "SELECT")
  {
    Serial.println("SELECT_Button_Presses");
    buttonState = "SELECT";
  }

  // Go into Settings
  if ((displayState == (1 || 2 || 3 || 4)) && (buttonState == "SELECT"))
  {
    displayState = 5;
    buttonState = "X";
  }
}