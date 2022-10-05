/*
  NEWT
  By Phambili
  https://phambili.tech
  Developed by Darian Johnson

  This is the core library for the Newt, by Phambili. The Newt is an
  open-source device that operates as a low-power, internet connected
  device.

  Newt was built using a number of open source libraries, including:
  - Adafruit_GFX, Adafruit_Sharp_Memory (Adafruit)
  - RV 3028 library (Constantin Koch)
  - Font Conversion (Coby Graphics)
  - WiFi Manager (Tzapu)
  - MQTT library (Joel Gahwiler)

  Phambili has spent considerable time and effort building Newt and this library.
  Please support NEWT by considering purchasing a NEWT.
  Copyright (c) 2022 Phambili. All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
  - Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/


#include "FS.h"
#include "SPIFFS.h"
//#include "USB.h"
#define SWVERSION_MAJOR 1
#define SWVERSION_MINOR 1
#define SWVERSION_PATCH 3
#define RC false
#define SWVERSION_RC 0

/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true

#include <Newt_Display.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>
#include <time.h>
#include <driver/rtc_io.h>
#include <EEPROM.h>

#include <WiFiManager.h>  // https://github.com/tzapu/WiFiManager 
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Update.h>
#include <HTTPClient.h>
#include <MQTT.h>
#include <ArduinoJson.h>

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"
#include "esp_log.h"

//===================FONT AND DISPLAY MANAGEMENT==============================
#include <Newt_Fonts/Newt_Font_Lookup.h>
#include <Newt_Fonts/FreeSansBold60pt7b.h>
#include <Newt_Fonts/FreeSansBold50pt7b.h>
#include <Newt_Fonts/FreeSansBold24pt7b.h>
#include <Newt_Fonts/FreeSansBold18pt7b.h>
#include <Newt_Fonts/FreeSansBold14pt7b.h>
#include <Newt_Fonts/FreeSansBold12pt7b.h>
#include <Newt_Fonts/FreeSansBold9pt7b.h>
#include <Newt_Fonts/FreeSansBold7pt7b.h>

#include <Newt_Fonts/FreeSansBoldOblique12pt7b.h>

#include <Newt_Fonts/FreeMonoBold9pt7b.h>
#include <Newt_Fonts/FreeMonoBold12pt7b.h>

#include <Newt_Fonts/FreeSans12pt7b.h>
#include <Newt_Fonts/FreeSans9pt7b.h>

#include <Newt_Fonts/slateWeather14pt7b.h>
#include <Newt_Fonts/slateWeather22pt7b.h>
#include <Newt_Fonts/slateWeather30pt7b.h>

#include <Newt_Fonts/slateIcons30pt7b.h>
#include <Newt_Fonts/slateIcons14pt7b.h>
#include <Newt_Fonts/slateIcons9pt7b.h>
#include <Newt_Fonts/slateIcons6pt7b.h>

#include <Newt_Fonts/introIcons50pt7b.h>

#define displayOrientation 2
RTC_DATA_ATTR int TEXTCOLOR = 0;
RTC_DATA_ATTR int BGCOLOR = 1;
int displayWidth;
int displayHeight;
int displayRefreshHeight = 180;
int displayMarginW = 16;
int displayMarginH = 8;
int16_t xP, yP;
uint16_t w, h;

int autoDarkH = 23;
int autoLightH = 5;
int refreshScreenM = 3;

//===================WIFI and MQTT MANAGEMENT========================================
#define INIT_HOST "newt.darianmakes.com"
#define INIT_PATH "/Newt-Functions"
#define UPGRADE_HOST "phambili-pub.s3.amazonaws.com"
#define UPGRADE_PATH "/Newt.ino_latest.bin"
#define UPGRADE_PORT 80

volatile unsigned long wifiTimeoutCounter;
#define MILLS_BEFORE_WIFI_TIMEOUT 15000
RTC_DATA_ATTR boolean wifiFailed = false;
boolean wifiAttempting = false;
RTC_DATA_ATTR int refreshDataCount = 0;
RTC_DATA_ATTR boolean minorRefresh = false;

#define REFRESH_WEATHER_COUNT 30

boolean shouldConnect = false;
boolean initialConnection = false;
boolean shouldBeConnected = false;
boolean requestCurrentWeather = false;
boolean requestWeatherForecast = false;
boolean requestQuote = false;
boolean requestTodos = false;
boolean updatedTodos = false;
boolean requestAirQuality = false;
boolean requestOblique = false;
boolean requestRiddle = false;
boolean requestWeatherForecasts = false;

byte sleepCheckCWeather = 0;
byte sleepCheckQuote = 1;
byte sleepCheckOblique = 2;
byte sleepCheckDWeather = 3;
byte sleepCheckHWeather = 4;
byte sleepCheckAQI = 5;
byte sleepCheckRiddle = 6;

byte readyToSleepCount = 7;
byte readyToSleep[] = { 0, 0, 0, 0, 0, 0, 0};

const byte numOfTopics = 11;
char topics[numOfTopics][25] = {
  "deviceDetails",
  "forecast",
  "current",
  "quote",
  "todo",
  "airQuality",
  "obliques",
  "currentWeather",
  "dailyWeather",
  "hourlyWeather",
  "riddle"
};

RTC_DATA_ATTR char uniqId[40];
RTC_DATA_ATTR char timezone[50];
RTC_DATA_ATTR char tz_info[50];
RTC_DATA_ATTR float lat;
RTC_DATA_ATTR float lng;
RTC_DATA_ATTR char city[60];
RTC_DATA_ATTR char locale[60];
RTC_DATA_ATTR char units[12];
RTC_DATA_ATTR byte latest_ver_major;
RTC_DATA_ATTR byte latest_ver_minor;
RTC_DATA_ATTR byte latest_ver_patch;
RTC_DATA_ATTR char mqtt_server[60];
RTC_DATA_ATTR char mqtt_token[60];
RTC_DATA_ATTR char mqtt_user[60];
RTC_DATA_ATTR char mqtt_pub_topic[60];

RTC_DATA_ATTR char savedSSID[129];
RTC_DATA_ATTR char savedIP[40];

RTC_DATA_ATTR boolean versionUpdateAvailable = false;

//===================EEPROM AND PREFERENCES MANAGEMENT========================================
#define EEPROM_SIZE 510
#define WIFI_ENABLED_ADDR 0
#define TESTER_SUCCESSFUL 1
#define TWENTY_FOUR_HOUR_FORMAT_ADDR 2 //if value is 1 then 24 hour
#define MEASURE_ADDR 3
#define DARKMODE_ADDR 4
#define DEFAULTSCREEN_ADDR 5
#define DEFAULT_MEASURE_ADDR 6

#define IMPERIALMEASURE 2
#define METRICMEASURE 1
#define AUTOMEASURE 0

#define QUOTESCREEN 1
#define WEATHERSCREEN 2
#define AUTOSCREEN 0

#define AUTOMODE 1
#define DARKMODE 2
#define LIGHTMODE 0

//===================BATTERY MANAGEMENT========================================
boolean usb_plugged_in = false;
boolean low_battery = false;
boolean battery_charged = false;

//===================SCREEN MANAGEMENT and SAVED VALUES=======================+
volatile unsigned long timeoutCounter;
byte currentDisplay = 0;

RTC_DATA_ATTR byte unitSetting;
RTC_DATA_ATTR byte savedScreenSetting;
RTC_DATA_ATTR byte tempScreenSetting = 1;

RTC_DATA_ATTR boolean showWeather = false;
RTC_DATA_ATTR boolean showDailyWeather = false;
//RTC_DATA_ATTR int currWeather;
RTC_DATA_ATTR int currentTemp[] = {0, 0};
RTC_DATA_ATTR int currentFeelsTemp[] = {0, 0};
RTC_DATA_ATTR char currWeatherIcon[2];
RTC_DATA_ATTR int currHumidity;
RTC_DATA_ATTR int currAQ = -1;
RTC_DATA_ATTR int currUV = -1;
RTC_DATA_ATTR char todayWDay[5];
RTC_DATA_ATTR char tomWDay[5];
RTC_DATA_ATTR int currDayNo;
RTC_DATA_ATTR char dailyWeatherIcon[2][2];
RTC_DATA_ATTR char todaySunrise[3][6];
RTC_DATA_ATTR char todaySunset[3][6];
RTC_DATA_ATTR int todayHiTemp[] = {0, 0};
RTC_DATA_ATTR int todayLoTemp[] = {0, 0};
RTC_DATA_ATTR char tomSunrise[3][6];
RTC_DATA_ATTR char tomSunset[3][6];
RTC_DATA_ATTR int tomHiTemp[] = {0, 0};
RTC_DATA_ATTR int tomLoTemp[] = {0, 0};

RTC_DATA_ATTR boolean showQuote = false;
RTC_DATA_ATTR char currQuote[200];
RTC_DATA_ATTR char currAuthor[120];

RTC_DATA_ATTR char currOblique[1200];

RTC_DATA_ATTR char dailyRiddle[300];
RTC_DATA_ATTR char dailyRiddleAnswer[300];
RTC_DATA_ATTR char dailyRiddleAttr[100];
RTC_DATA_ATTR int riddleCategoryNo;

RTC_DATA_ATTR boolean alarmEnabled = false;
RTC_DATA_ATTR boolean alarmActive = false;
RTC_DATA_ATTR boolean timerEnabled = false;
RTC_DATA_ATTR boolean timerActive = false;
RTC_DATA_ATTR boolean timerSubMin = false;
RTC_DATA_ATTR boolean pomodoroInitiated = false;
RTC_DATA_ATTR boolean pomodoroSaved = false;
RTC_DATA_ATTR boolean pomodoroPaused = false;
RTC_DATA_ATTR boolean pomodoroEnabled = false;
RTC_DATA_ATTR boolean pomodoroNeedInterrupt = false;
RTC_DATA_ATTR byte pomodoroItem = 0;
RTC_DATA_ATTR unsigned long pomodoroSavedEpoch;
boolean showPomodoroMenu = false;
#define pomodoroWorkMinues 25
#define pomodoroShortBreakMinues 5
#define pomodoroLongBreakMinues 10

RTC_DATA_ATTR boolean outputTone = false;
#define TIME_TO_SLEEP_BUZZER 5
#define BUZZER_SETUP_FREQUENCY 1000
#define BEEP_DUTY_CYCLE 50
#define BEEP_FREQUENCY 3000
#define BUZZER_CHANNEL 0
#define BUZZER_RESOLUTION 8

#define MILLS_BEFORE_DEEP_SLEEP 30000
#define MILLS_BEFORE_DEEP_SLEEP_SHORT 10000
#define MILLS_BEFORE_DEEP_SLEEP_QUICK 3000
#define MILLS_BEFORE_DEEP_SLEEP_TIMER 5000
#define MILLS_BEFORE_DEEP_SLEEP_SETUP 300000  //5 mins

//locations for the buttons in relation to the display screen
#define BUTTON_A_PIN_Y 155
#define BUTTON_B_PIN_Y 225
#define BUTTON_AB_PIN_Y 190
#define BUTTON_AB_PIN_X 15
#define BUTTON_C_PIN_X 50
#define BUTTON_D_PIN_X 150
#define BUTTON_E_PIN_X 250
#define BUTTON_F_PIN_X 350
#define BUTTON_CDEF_PIN_Y 235
#define TIMING_X 255  //270

#define DEBOUNCE 100
#define KEYPAD_DEBOUNCE 150

//===================PIN DEFINITIONS========================================
#define SDA 33
#define SCL 34

// SHARP_DISPLAY PINS
#define SHARP_SCK 36   //18 //26
#define SHARP_MOSI 35  //5 //25
#define SHARP_SS 38    //21 //27 (this is ok)

#define RTC_PIN 16                //21 //16
#define RTC_PIN_NAME GPIO_NUM_16  //GPIO_NUM_21 //GPIO_NUM_16
#define RTC_BITMASK 0x10000       //2^16 try 16

#define WAKE_PIN 1
#define WAKE_PIN_NAME GPIO_NUM_1

#define BUTTON_A 14
#define BUTTON_A_NAME GPIO_NUM_14
#define BUTTON_A_TOUCH_NAME TOUCH_PAD_NUM14

#define BUTTON_B 13
#define BUTTON_B_NAME GPIO_NUM_13
#define BUTTON_B_TOUCH_NAME TOUCH_PAD_NUM13

#define BUTTON_C 12
#define BUTTON_C_NAME GPIO_NUM_12
#define BUTTON_C_TOUCH_NAME TOUCH_PAD_NUM12

#define BUTTON_D 11
#define BUTTON_D_NAME GPIO_NUM_11
#define BUTTON_D_TOUCH_NAME TOUCH_PAD_NUM11

#define BUTTON_E 10
#define BUTTON_E_NAME GPIO_NUM_10
#define BUTTON_E_TOUCH_NAME TOUCH_PAD_NUM10

#define BUTTON_F 9
#define BUTTON_F_NAME GPIO_NUM_9
#define BUTTON_F_TOUCH_NAME TOUCH_PAD_NUM9

#define PAD_ONE 5
#define PAD_ONE_NAME GPIO_NUM_5
#define PAD_ONE_TOUCH_NAME TOUCH_PAD_NUM5

#define PAD_TWO 6
#define PAD_TWO_NAME GPIO_NUM_6
#define PAD_TWO_TOUCH_NAME TOUCH_PAD_NUM6

#define PAD_THREE 7
#define PAD_THREE_NAME GPIO_NUM_7
#define PAD_THREE_TOUCH_NAME TOUCH_PAD_NUM7

#define PAD_FOUR 8
#define PAD_FOUR_NAME GPIO_NUM_8
#define PAD_FOUR_TOUCH_NAME TOUCH_PAD_NUM8

#define TOUCH_BUTTON_NUM 10  //2 //10
static const touch_pad_t button[TOUCH_BUTTON_NUM] = {
  BUTTON_A_TOUCH_NAME,
  BUTTON_B_TOUCH_NAME,
  BUTTON_C_TOUCH_NAME,
  BUTTON_D_TOUCH_NAME,
  BUTTON_E_TOUCH_NAME,
  BUTTON_F_TOUCH_NAME,
  PAD_ONE_TOUCH_NAME,
  PAD_TWO_TOUCH_NAME,
  PAD_THREE_TOUCH_NAME,
  PAD_FOUR_TOUCH_NAME
};

#define BUZZER 15
#define BUZZER_NAME GPIO_NUM_15

#define SPKR_CTRL 18
#define SPKR_CTRL_NAME GPIO_NUM_18

#define LBO 39
#define LBO_NAME GPIO_NUM_39

#define CHRG_STAT 21
#define CHRG_STAT_NAME GPIO_NUM_21

#define USB_IN 17
#define USB_IN_NAME GPIO_NUM_17
#define USB_BITMASK 0x20000  //2^17


//===================OTHER FACTORS========================================
RTC_DATA_ATTR boolean initialBoot = true;
RTC_DATA_ATTR boolean initialRTC = true;


#define uS_TO_S_FACTOR 1000000ULL /* Conversion factor for micro seconds to seconds */

//===================ATTRIBUTION========================================
int attributionItems = 7;
char entities[7][20] = { "Adafruit", "Constantin Koch", "Rop Gonggrijp", "Coby Graphics", "Tzapu", "Joel Gahwiler", "Apple Weather" };
char contributions[7][60] = { "GFX & Sharp Display libraries", "RV 3028 library", "Font Conversion", "Newt icon", "WiFi Manager", "MQTT library", "Weather API" };

//===================INITALIZATION========================================

//USBCDC USBSerial;
RV3028 rtc;
WiFiClient net;
WiFiClient client_upgrade;
MQTTClient client(2000);
Adafruit_SharpMem display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 400, 240);

#include "globalFunctions.h"
#include "mainDisplay.h"
#include "wifiFunctions.h"
#include "alarmDisplay.h"
#include "timerDisplay.h"
#include "weatherDisplay.h"
#include "habitDisplay.h"
#include "todoDisplay.h"
#include "settingsDisplay.h"
#include "textDisplay.h"
#include "buttonFunctions.h"


void refreshCheck(boolean ignore = false) {

  if (ignore) {
    timeoutCounter = 0;
    return;
  }

  if (!isWifiInited()) {
    return;
  }

  if (refreshDataCount > REFRESH_WEATHER_COUNT) {
    refreshDataCount = 0;
    wifiAttempting = true;
    renderMainDisplay();  //render the display to display the correct time before getting new information
    if (minorRefresh) {
      requestWeatherForecasts = true;
      minorRefresh = false;
    } else {
      requestWeatherForecasts  = true;
      requestAirQuality = true;
      //requestOblique = true;
      //requestRiddle = true;
      requestQuote = true;
      minorRefresh = true;

      while (!rtc.updateTime());

      rtc.set24Hour(); //change to 24 hour time
      if (rtc.getHours() == 2) { //refresh Riddle and Oblique every day between 2-3AM
        requestOblique = true;
        requestRiddle = true;
      }

      if (!isTwentyFourHour) { //switch back to 12 hour time if needed
        rtc.set12Hour();
      }


    }
    shouldConnect = true;
    wifiTimeoutCounter = millis() + MILLS_BEFORE_WIFI_TIMEOUT;
    timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP;

  } else {
    refreshDataCount++;
    timeoutCounter = 0;
  }

  checkTempScreenSetting();

  Serial.print("Refresh Count: ");
  Serial.println(refreshDataCount);
}

void handleWakeupReason() {

  touch_pad_t touchPin;

  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  Serial.println("Wakeup reason:");
  Serial.println(wakeup_reason);

  rtc_gpio_deinit(RTC_PIN_NAME);
  rtc_gpio_deinit(USB_IN_NAME);
  rtc_gpio_deinit(PAD_ONE_NAME);
  rtc_gpio_deinit(PAD_TWO_NAME);
  rtc_gpio_deinit(PAD_THREE_NAME);
  rtc_gpio_deinit(PAD_FOUR_NAME);
  rtc_gpio_deinit(BUTTON_A_NAME);
  rtc_gpio_deinit(BUTTON_B_NAME);
  rtc_gpio_deinit(BUTTON_C_NAME);
  rtc_gpio_deinit(BUTTON_D_NAME);
  rtc_gpio_deinit(BUTTON_E_NAME);
  rtc_gpio_deinit(BUTTON_F_NAME);

  setupTouchPadWhileAwake();
  pinMode(RTC_PIN, INPUT);
  attachInterrupt(RTC_PIN, RTC_isr, FALLING);
  pinMode(USB_IN, INPUT);
  attachInterrupt(USB_IN, USB_isr, RISING);

  uint64_t GPIO_reason;
  int selectedGPIO;

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:
      Serial.println("Wakeup caused by external signal using RTC_IO");
      break;
    case ESP_SLEEP_WAKEUP_EXT1:
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      GPIO_reason = esp_sleep_get_ext1_wakeup_status();
      selectedGPIO = (log(GPIO_reason)) / log(2);
      Serial.print("GPIO that triggered the wake up: GPIO ");
      Serial.println(selectedGPIO);

      timeoutCounter = 0;
      renderMainDisplay();

      /*if (selectedGPIO == RTC_PIN) {
        //timeoutCounter = 0;
        //refreshCheck(); I don't need refresh for this; as of right now; but may add if I use RTC for periodic wackup
        timeoutCounter = 0;
        renderMainDisplay(); //I shouldnt need this...
        //refreshCheck(true);
        break;
        }

        if (selectedGPIO == USB_IN) {
        //wake-up function
        timeoutCounter = 0;
        break;
        }*/
      break;
    case ESP_SLEEP_WAKEUP_TIMER:
      Serial.println("Wakeup caused by internal timer");
      //timeoutCounter = 0;
      refreshCheck();

      break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD:
      if (alarmActive || timerActive) {  //the user is turning off the buzzer
        if (timerActive) {
          disableRTCTimer();
          handlePomodoroAlarm();
        }
        if (alarmActive) {
          alarmEnabled = false;
          disableRTCAlarm();
        }
        timeoutCounter = 0;
      } else {
        showSeconds = true;
        showMenu = true;
        menuStart = 0;
        timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP_SHORT;
        renderMainDisplay();
      }
      break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
    default: Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

void introScreen(int message = 0) {

  int startY = displayMarginH * 2;
  int startX = displayMarginW;
  int spacer = 10;
  display.fillScreen(BGCOLOR);
  display.setTextColor(TEXTCOLOR);

  display.setFont(&introIcons50pt7b);
  display.getTextBounds("N", 0, 0, &xP, &yP, &w, &h);
  startY = startY + h;
  display.setCursor(displayWidth / 2 - w / 2, startY);
  display.print("N");

  display.setFont(&FreeSansBold9pt7b);
  display.getTextBounds("Welcome to", 0, 0, &xP, &yP, &w, &h);
  startY = startY + h + spacer;
  display.setCursor(displayWidth / 2 - w / 2, startY);
  display.print("Welcome to");

  display.setFont(&FreeSansBold24pt7b);
  display.getTextBounds("NEWT", 0, 0, &xP, &yP, &w, &h);
  startY = startY + h + spacer;
  display.setCursor(displayWidth / 2 - w / 2, startY);
  display.print("NEWT");
  display.refresh();

  switch (message) {
    case 0:
      delay(500);
      display.setFont(&FreeSansBold9pt7b);
      display.getTextBounds("by Phambili", 0, 0, &xP, &yP, &w, &h);
      startY = startY + h + spacer;
      display.setCursor(displayWidth - (displayMarginW * 3 + w), startY);
      display.print("by Phambili");
      startY = startY + h + spacer;
      display.setCursor(displayWidth - (displayMarginW * 3 + w), startY);
      display.print("version: ");
      display.print(SWVERSION_MAJOR); display.print("."); display.print(SWVERSION_MINOR); display.print("."); display.print(SWVERSION_PATCH);
      display.refresh();

      delay(1500);
      display.fillScreen(BGCOLOR);
      display.setTextColor(TEXTCOLOR);
      delay(500);
      startX = displayMarginW / 2;
      startY = displayMarginH * 2 + 10;

      display.setCursor(startX, startY);
      display.setFont(&FreeSansBold12pt7b);
      display.print("Attribution:");

      for (int i = 0; i < attributionItems; i++) {
        startY = startY + 20;
        display.setFont(&FreeSansBold9pt7b);
        display.setCursor(startX, startY);
        display.print(entities[i]);
        display.print(": ");
        display.setFont(&FreeSans9pt7b);
        display.print(contributions[i]);
        delay(200);
        display.refresh();
      }
      delay(1000);
      break;
    case 1:  //About to attempt Wifi connection
      delay(500);
      display.setFont(&slateIcons9pt7b);
      display.getTextBounds(wifiG, 0, 0, &xP, &yP, &w, &h);
      startY = startY + h + spacer * 4;
      display.setCursor(startX, startY);
      display.print(wifiG);
      display.setFont(&FreeSansBold9pt7b);
      display.print(" Connecting to Wifi");
      display.refresh();
      break;
    case 2:  //Print initializing device (connecting to init function)
      delay(500);
      display.setFont(&slateIcons9pt7b);
      display.getTextBounds(wifiG, 0, 0, &xP, &yP, &w, &h);
      startY = startY + h + spacer * 4;
      display.setCursor(startX, startY);
      display.print(cogG);
      display.setFont(&FreeSansBold9pt7b);
      display.print(" Initializing Device");
      display.refresh();
      break;
    case 3:  //Print location
      delay(500);
      display.setFont(&slateIcons9pt7b);
      display.getTextBounds(cogG, 0, 0, &xP, &yP, &w, &h);
      startY = startY + h + spacer * 4;
      display.setCursor(startX, startY);
      display.print(cogG);
      display.setFont(&FreeSansBold9pt7b);
      display.print(" Location: ");
      display.print(city);
      display.print(" (");
      display.print(locale);
      display.print(")");
      display.refresh();
      break;
    case 4:  //Print timezone
      delay(500);
      display.setFont(&slateIcons9pt7b);
      display.getTextBounds(wifiG, 0, 0, &xP, &yP, &w, &h);
      startY = startY + h + spacer * 4;
      display.setCursor(startX, startY);
      display.print(cogG);
      display.setFont(&FreeSansBold9pt7b);
      display.print(" Timezone: ");
      display.print(timezone);
      display.refresh();
      break;
    case 5:  //Set Clock
      delay(500);
      display.setFont(&slateIcons9pt7b);
      display.getTextBounds(wifiG, 0, 0, &xP, &yP, &w, &h);
      startY = startY + h + spacer * 4;
      display.setCursor(startX, startY);
      display.print("a");
      display.setFont(&FreeSansBold9pt7b);
      display.print(" Setting Clock");
      display.refresh();
      break;
    case 6:  //Done
      delay(200);
      display.setFont(&slateIcons9pt7b);
      display.getTextBounds(wifiG, 0, 0, &xP, &yP, &w, &h);
      startY = startY + h + spacer * 4;
      display.setCursor(startX, startY);
      display.setFont(&FreeSansBold9pt7b);
      display.print("DONE!");
      display.refresh();
      delay(300);
      renderMainDisplay();
      break;
    case 7:  //Error getting data
      delay(200);
      display.setFont(&slateIcons9pt7b);
      display.getTextBounds(wifiG, 0, 0, &xP, &yP, &w, &h);
      startY = startY + h + spacer * 4;
      display.setCursor(startX, startY);
      display.setFont(&FreeSansBold9pt7b);
      display.print("There was an error getting device data. Please reboot NEWT");
      display.refresh();
      delay(300);
      renderMainDisplay();
      break;
  }
}

void configModeCallback(WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  setupWifiDisplay();
}

//BYPASS_SPIRAM_TEST(true);

void setup() {

  Serial.begin();
  //USB.begin();

  //ensure SPIFFS is set-up
  if (!SPIFFS.begin(FORMAT_SPIFFS_IF_FAILED)) {
    Serial.println("SPIFFS Mount Failed");
    delay(500);
    ESP.restart();
    return;
  }

  //ititalize EEPROM
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(500);
    ESP.restart();
  }

  //setup Buzzer
  pinMode(SPKR_CTRL, OUTPUT);
  ledcSetup(BUZZER_CHANNEL, BUZZER_SETUP_FREQUENCY, BUZZER_RESOLUTION);
  ledcAttachPin(BUZZER, BUZZER_CHANNEL);

  //setup I2C
  Wire.begin(SDA, SCL);

  // start & clear the display---------------------------------------------------------------------
  display.begin();
  display.setRotation(displayOrientation);

  displayWidth = display.width();
  displayHeight = display.height();


  //initialBoot = false; //Temp
  // initialize RTC--------------------------------------------------------------------------------
  initRTC(initialBoot);

  if (initialBoot) {
    initialBoot = false;
    byte mac[6];
    WiFi.macAddress(mac);
    sprintf(uniqId, "newt%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    rtc.writeRegister(RV3028_STATUS, 0x00);

    introScreen();
    introScreen(1);  //connecting to Wifi

    WiFi.mode(WIFI_STA);
    WiFiManager wm;

    //reset settings - wipe credentials for testing
    //wm.resetSettings();

    wm.setConfigPortalTimeout(120);
    wm.setAPCallback(configModeCallback);

    bool res;
    res = wm.autoConnect(uniqId);  // anonymous ap

    if (!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
    } else {
      //if you get here you have connected to the WiFi
      Serial.println("We have connected.)");

      //strcpy(savedSSID, WiFi.SSID());

      WiFi.SSID().toCharArray(savedSSID, 128);
      //savedIP = WiFi.localIP().toString().c_str();

      IPAddress ip = WiFi.localIP();
      sprintf(savedIP, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

      if (!isWifiInited()) {
        noteWifiInited();
      }
    }

    if (res) {

      if (WiFi.status() != WL_CONNECTED) {
        WiFi.begin();
      }

      Serial.print("checking wifi...");
      while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
      }
      introScreen(2);  //initializing Device
      int statusMsg = getDeviceDetails();

      if (statusMsg > 0) {
        introScreen(7);  //display error message
        delay(3000);
      } else {
        introScreen(3);  //print location
        delay(2000);
        introScreen(4);  //print timezone
        delay(2000);
        introScreen(5);  //set clock
      }
      syncTimeNTP();
      saveShortTermMeasure(); //get units of measure
      savedScreenSetting = getDefaultScreen(); //get and save default screen
      checkTempScreenSetting();

      wifiAttempting = true;
      initialConnection = true;
      requestQuote = true;
      requestAirQuality = true;
      requestOblique = true;
      requestRiddle = true;
      requestWeatherForecasts = true;
    }

    initRTCInterrupts();
    introScreen(6);  //done
    timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP;

    setColorMode(); //do this after RTC init in case we are on auto mode
  }

  //Print the wakeup reason for ESP32
  handleWakeupReason();
  /* Start task to read values by pads. */
  xTaskCreate(&touchpad_check_value, "touchpad_check_value", 2048, NULL, 5, NULL);

}

int timerDisplayCounter = 0;

void loop() {

  if (WiFi.status() != WL_CONNECTED && shouldConnect) {
    WiFi.begin();
    Serial.println("attempting to connect to wifi");
    initialConnection = true;
    shouldConnect = false;
    wifiAttempting = true;
  }

  if (WiFi.status() != WL_CONNECTED && wifiAttempting && millis() > wifiTimeoutCounter) {
    wifiFailed = true;
    shouldConnect = false;

    if (currentDisplay == HOME) {
      timeoutCounter = 0;
    }
  }


  if (WiFi.status() == WL_CONNECTED && initialConnection) {

    Serial.println("wifi connected");
    delay(50);

    // Configure MQTT Client
    client.begin(mqtt_server, net);
    client.onMessage(messageReceived);

    connect();

    wifiFailed = false;
    initialConnection = false;
    shouldBeConnected = true;
  }

  mqttLoop();

  if (requestTodos && shouldBeConnected) {
    Serial.println("We need to request todos");
    requestTodoList();
    requestTodos = false;
  }


  if (requestWeatherForecasts && shouldBeConnected) {
    Serial.println("Send All Forecast request");
    readyToSleep[sleepCheckCWeather] = 1;  //for current weather
    readyToSleep[sleepCheckDWeather] = 1;  //for daily weather
    readyToSleep[sleepCheckHWeather] = 1;  //for hourly weather
    requestWeatherForecastsCall();
    requestWeatherForecasts = false;
  }

  if (requestQuote && shouldBeConnected) {
    Serial.println("Send New Quote request");
    readyToSleep[sleepCheckQuote] = 1;  //for quote
    requestNewQuote();
    requestQuote = false;
  }

  if (requestAirQuality && shouldBeConnected) {
    Serial.println("Send Air Quality request");
    readyToSleep[sleepCheckAQI] = 1;  //for air quality
    requestAQ();
    requestAirQuality = false;
  }

  if (requestOblique && shouldBeConnected) {
    Serial.println("Send Oblique request");
    readyToSleep[sleepCheckOblique] = 1;  //for oblique
    requestDailyOblique();
    requestOblique = false;
  }

  if (requestRiddle && shouldBeConnected) {
    Serial.println("Send Riddle request");
    readyToSleep[sleepCheckRiddle] = 1;  //for riddle
    requestDailyRiddle();
    requestRiddle = false;
  }

  if (padPressed) {
    selectFunction();
    padPressed = false;
    selectedTouchpad = 0;
    delay(DEBOUNCE);
  }

  if (showSeconds && timerEnabled && currentDisplay == HOME && (millis() > timerDisplayCounter)) {
    timerDisplayCounter = millis() + 500;
    keepMenu();
  }

  if (rtcIrq) {
    Serial.println("interrupt fired; right now, no logic here. keep in case we decide to move logic here");
    rtcIrq = false;
  }

  if (usbIrq) {
    Serial.println("interrupt fired; need to enable USB plugged in function");
    usbIrq = false;
  }

  if (rtc.readPeriodicUpdateInterruptFlag() && currentDisplay == HOME) {
    keepMenu();
  }

  if (rtc.readTimerInterruptFlag() && currentDisplay == HOME) {
    keepMenu();
  }

  if (rtc.readAlarmInterruptFlag()) {
    if (!alarmEnabled) {
      Serial.println("Alarm Interrupt Flag - False Fire");  //adding this logic because there are times where the interrupt flag is not cleared
      disableRTCAlarm();
    } else {
      Serial.println("Alarm Interrupt Flag");
      timeoutCounter = 0;  //force immediate alarm logic
    }
    Serial.println("Alarm Interrupt Flag");
  }

  //add this logic due to bug in rtc.readAlarmInterruptFlag when device is awake
  if (alarmEnabled && !alarmActive) {

    if (manualAlarmCheck()) {
      overrideAlarmEnabled = true;
      timeoutCounter = 0;  //force immediate alarm logic
    }
  }

  if (millis() > timeoutCounter) {
    showSeconds = false;
    showMenu = false;
    wifiAttempting = false;
    renderMainDisplay();
    if (millis() > timeoutCounter) {  //check for a second time, because we may have change it due to an alarm
      handleSleep();
    }
  }

  delay(50);
}

void handleSleep() {

  disconnectWifi();

  setupTouchPadForSleep(PAD_ONE_TOUCH_NAME);  //setup Touch Pad
  esp_sleep_enable_touchpad_wakeup();

  if (alarmActive || timerActive) {  //set timer for sleep if alarm is firing
    Serial.println("Alarm firing; Set RTC wakeup and Timer for buzzer wakeup");
    esp_sleep_enable_ext1_wakeup(RTC_BITMASK, ESP_EXT1_WAKEUP_ALL_LOW);  //setup RTC interrupt pin
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP_BUZZER * uS_TO_S_FACTOR);
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP_BUZZER) + " Seconds");
  } else {
    if (timerEnabled) {
      Serial.println("Set RTC wakeup");
      esp_sleep_enable_ext1_wakeup(RTC_BITMASK, ESP_EXT1_WAKEUP_ALL_LOW);  //setup RTC interrupt pin
    } else {
      Serial.println("Set Internal Counter wakeup");

      if (usb_plugged_in) {
        esp_sleep_enable_ext1_wakeup(USB_BITMASK, ESP_EXT1_WAKEUP_ALL_LOW);  //usb is plugged in; wake up when unplugged
      } else {
        esp_sleep_enable_ext1_wakeup(USB_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);  //usb is unplugged in; wake up when plugged in
      }

      struct tm timeinfo;
      int s_i;
      if (!getLocalTime(&timeinfo))
      {
        while (!rtc.updateTime());
        s_i = rtc.getSeconds();
      } else {
        s_i = timeinfo.tm_sec;
      }

      int time_to_sleep = (60 - s_i);
      esp_sleep_enable_timer_wakeup(time_to_sleep * uS_TO_S_FACTOR);
      Serial.println("Setup ESP32 to sleep for " + String(time_to_sleep) + " Seconds");
    }
  }

  //if ((timerEnabled && !timerActive && timerH < 1 && timerM < 2) || (usb_plugged_in) ){
  if (timerEnabled && !timerActive && timerH < 1 && timerM < 2) {
    Serial.println("light sleep");
    Serial.flush();
    esp_light_sleep_start();
    handleWakeupReason();

  } else {
    Serial.println("Going to deep sleep now");
    Serial.flush();
    //setCpuFrequencyMhz(240); //set at highest speed for processing

    Wire.end(); // shutdown/power off I2C hardware,
    pinMode(SDA, INPUT); // needed because Wire.end() enables pullups, power Saving
    pinMode(SCL, INPUT); // needed because Wire.end() enables pullups, power Saving
    esp_deep_sleep_start();
  }
}
