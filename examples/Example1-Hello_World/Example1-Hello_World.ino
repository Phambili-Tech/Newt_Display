/*
  NEWT - Display and Touchpads
  By Phambili
  https://phambili.tech
  Developed by Darian Johnson

  This is an example that shows how to:
  1 - write to the screen
  2 - interact with the touch pads
  3-  trigger the buzzer

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

//===================LIBRARIES DEFINITIONS========================================
#include "FS.h"
#include "SPIFFS.h"
/* You only need to format SPIFFS the first time you run a
   test or else use the SPIFFS plugin to create a partition
   https://github.com/me-no-dev/arduino-esp32fs-plugin */
#define FORMAT_SPIFFS_IF_FAILED true
#include <Newt_Display.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SharpMem.h>

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/touch_pad.h"
#include "esp_log.h"


//===================FONT AND DISPLAY MANAGEMENT==============================
#include <Newt_Fonts/Newt_Font_Lookup.h>
#include <Newt_Fonts/FreeSansBold12pt7b.h>
#include <Newt_Fonts/FreeSans12pt7b.h>


//===================DISPLAY DEFINITIONS========================================
#define displayOrientation 2
RTC_DATA_ATTR int TEXTCOLOR = 0;
RTC_DATA_ATTR int BGCOLOR = 1;
int displayWidth;
int displayHeight;
int displayRefreshHeight = 180;
int displayMarginW = 16;
int displayMarginH = 8;

//===================BUZZER DEFINITIONS========================================
#define BUZZER_SETUP_FREQUENCY 1000
#define BEEP_DUTY_CYCLE 100
#define BEEP_FREQUENCY 3000
#define BUZZER_CHANNEL 0
#define BUZZER_RESOLUTION 8

//===================PIN DEFINITIONS========================================
#define SDA 33
#define SCL 34

// SHARP_DISPLAY PINS
#define SHARP_SCK 36
#define SHARP_MOSI 35
#define SHARP_SS 38

#define RTC_PIN 16
#define RTC_PIN_NAME GPIO_NUM_16
#define RTC_BITMASK 0x10000 //2^16

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

#define TOUCH_BUTTON_NUM 10
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

//===================INITALIZATION========================================
#include "buttonFunctions.h"
RV3028 rtc;
Adafruit_SharpMem display(SHARP_SCK, SHARP_MOSI, SHARP_SS, 400, 240);

void setup() {

  Serial.begin();

  //setup I2C
  Wire.begin(SDA, SCL);

  //start RTC - NOTE RTC Begin is important. The RTC sends a 1 HZ pulse to the display to stop screen burn-in
  if (rtc.begin() == false) {
    Serial.println("RTC CHECK - FAILED");
    display.refresh();
    while (1);
  }

  // start & clear the display---------------------------------------------------------------------
  display.begin();
  display.setRotation(displayOrientation);

  displayWidth = display.width();
  displayHeight = display.height();

  display.fillScreen(BGCOLOR);
  display.setTextColor(TEXTCOLOR);

  int startX = displayMarginW / 2;
  int startY = displayMarginH * 2 + 10;

  display.setCursor(startX, startY);
  display.setFont(&FreeSansBold12pt7b);
  display.print("TOUCH ANY PAD");

  display.refresh();

  // Start task to read values by pads---------------------------------------------------------------------
  xTaskCreate(&touchpad_check_value, "touchpad_check_value", 2048, NULL, 5, NULL);
  setupTouchPadWhileAwake();

  //setup Buzzer---------------------------------------------------------------------
  pinMode(SPKR_CTRL, OUTPUT);
  ledcSetup(BUZZER_CHANNEL, BUZZER_SETUP_FREQUENCY, BUZZER_RESOLUTION);
  ledcAttachPin(BUZZER, BUZZER_CHANNEL);

}

void beep() {
  digitalWrite(SPKR_CTRL, HIGH);
  ledcWriteTone(BUZZER_CHANNEL, BEEP_FREQUENCY);
}

void beepOff(){
    ledcWriteTone(BUZZER_CHANNEL, 0);
  digitalWrite(SPKR_CTRL, LOW);
}

int pressCount = 0;
int yTab = 25;
int xTab = 10;

void loop() {
  if (padPressed) {

    if (pressCount > 6) {
      pressCount = 0;
      display.fillScreen(BGCOLOR);
      display.setTextColor(TEXTCOLOR);
      int startX = displayMarginW / 2;
      int startY = displayMarginH * 2 + 10;

      display.setCursor(startX, startY);
      display.setFont(&FreeSansBold12pt7b);
      display.print("TOUCH ANY PAD");
    }

    display.setCursor(xTab, 60 + yTab * pressCount);
    display.setFont(&FreeSans12pt7b);
    display.print("Pressed: ");
    Serial.print("Pressed-Serial: ");

    switch (selectedTouchpad) {
      case PAD_ONE:
        display.print("PAD_ONE");
        Serial.print("PAD_ONE");
        break;
      case PAD_TWO:
        display.print("PAD_TWO");
        Serial.print("PAD_TWO");
        break;
      case PAD_THREE:
        display.print("PAD_THREE");
        Serial.print("PAD_THREE");
        break;
      case PAD_FOUR:
        display.print("PAD_FOUR");
        Serial.print("PAD_FOUR");
        break;
      case BUTTON_A:
        display.print("BUTTON_A");
        Serial.print("BUTTON_A");
        break;
      case BUTTON_B:
        display.print("BUTTON_B");
        Serial.print("BUTTON_B");
        break;
      case BUTTON_C:
        display.print("BUTTON_C");
        Serial.print("BUTTON_C");
        break;
      case BUTTON_D:
        display.print("BUTTON_D");
        Serial.print("BUTTON_D");
        break;
      case BUTTON_E:
        display.print("BUTTON_E");
        Serial.print("BUTTON_E");
        break;
      case BUTTON_F:
        display.print("BUTTON_F");
        Serial.print("BUTTON_F");
        break;
      default:
        // statements
        break;
    }
    pressCount++;
    padPressed = false;
    display.refresh();
    beep();
    delay(50);
    beepOff();

  }

  delay(50);

}
