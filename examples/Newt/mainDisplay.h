boolean showMenu = false;
boolean showSeconds = false;
int menuStart = 0;
boolean overrideAlarmEnabled = false;

struct MENU_ICON {
  char displayName[10];
  char displayIcon[2];
  uint32_t functionNumber;
  uint32_t assignedTouchpad;
};

#define HOME 100
#define WAKE 7
#define ALARM 1
#define TIMER 2
#define WEATHER 3
#define HABIT 4
#define TODO 5
#define KEYPAD 6
#define WIFISETUP 8
#define SETTINGS 9
#define SETTINGSINFO 91
#define AIRQUALITY 10
#define OBLIQUE 11
#define UPGRADE 12
#define POMODORO 13
#define UV 14
#define QUOTE 15
#define ALERTS 16
#define RIDDLE 17
#define DUMMY 200
#define CANCEL 201
#define FACTORYCHECK 202

const byte numOfMenuItems = 12; //8;
MENU_ICON menuIconArray[numOfMenuItems] = {
  {"ALARM", alarmG, ALARM, 0},
  {"TIMER", hourglassG, TIMER, 0},
  {"WEATHER", weatherG, WEATHER, 0},
  {"HABITS", calendarG, HABIT, 0},
  {"AIR QUAL", pollutionG, AIRQUALITY, 0},
  {"UV", uvG, UV , 0},
  {"POMO", tomatoG, POMODORO , 0},
  {"QUOTE", quoteG, QUOTE, 0},
  {"OBLIQUE", slashG, OBLIQUE, 0},
  {"RIDDLE", puzzleG, RIDDLE, 0},
  {"ALERTS", warningG, ALERTS, 0},
  {"SETTINGS", equalizerG, SETTINGS, 0}
};

void batteryManagement() {
  pinMode(CHRG_STAT, INPUT);
  pinMode(USB_IN, INPUT);
  pinMode(LBO, INPUT);

  if (digitalRead(CHRG_STAT) == HIGH) {
    battery_charged = true;
  } else {
    battery_charged = false;
  }

  if (digitalRead(USB_IN) == HIGH) {
    usb_plugged_in = true;
  } else {
    usb_plugged_in = false;
  }

  if (digitalRead(LBO) == LOW) {
    low_battery = true;
  } else {
    low_battery = false;
  }

  pinMode(CHRG_STAT, OUTPUT);
  pinMode(USB_IN, OUTPUT);
  pinMode(LBO, OUTPUT);
}

void checkLatestVersion() {

  int latestMajorLarger = -1;
  int latestMinorLarger = -1;
  int latestPatchLarger = -1;

  if (latest_ver_major > SWVERSION_MAJOR) {
    latestMajorLarger = 1;
  } else if (latest_ver_major == SWVERSION_MAJOR) {
    latestMajorLarger = 0;
  }

  if (latest_ver_minor > SWVERSION_MINOR) {
    latestMinorLarger = 1;
  } else if (latest_ver_minor == SWVERSION_MINOR) {
    latestMinorLarger = 0;
  }

  if (latest_ver_patch > SWVERSION_PATCH) {
    latestPatchLarger = 1;
  } else if (latest_ver_patch == SWVERSION_PATCH) {
    latestPatchLarger = 0;
  }


  if (latestMajorLarger > 0) {
    versionUpdateAvailable = true;
    return;
  }

  if (latestMajorLarger == 0 && latestMinorLarger > 0) {
    versionUpdateAvailable = true;
    return;
  }

  if (latestMajorLarger == 0 && latestMinorLarger == 0 && latestPatchLarger > 0) {
    versionUpdateAvailable = true;
    return;
  }


  if ((SWVERSION_MAJOR == latest_ver_major) && (SWVERSION_MINOR == latest_ver_minor) && (SWVERSION_PATCH == latest_ver_patch) && SWVERSION_RC) {
    versionUpdateAvailable = true;
    return;
  }

  versionUpdateAvailable = false;

}

void renderQuote(int startY) {

  display.setFont(&FreeSansBoldOblique12pt7b);
  display.getTextBounds("X", 0, 0, &xP, &yP, &w, &h);

  int lineH = h + 5;
  int spaceFromTime = 20;
  int quoteSpacer = 7;
  startY = startY + lineH + spaceFromTime;
  display.setCursor(displayMarginW, startY);

  char quote[200];
  strcpy(quote, currQuote);

  char delim[] = " ";
  int lineLimit = displayWidth - (displayMarginW * 5.5) - 5; //note: we are ok going a little past the self-imposed margins
  int lineLength = 0;
  int lineCount = 1;

  char *ptr = strtok(quote, delim);

  while (ptr != NULL)
  {
    //printf("'%s'\n", ptr);
    display.getTextBounds(ptr, 0, 0, &xP, &yP, &w, &h);
    //Serial.println(w);

    if ((lineLength + w) > lineLimit) {
      lineLength = w;
      startY = startY + lineH;
      display.setCursor(displayMarginW, startY);
      lineCount++;
    } else {
      lineLength = lineLength + w;
    }
    display.print(ptr);
    display.print(" ");

    ptr = strtok(NULL, delim);

  }

  display.setFont(&FreeSans9pt7b);
  display.getTextBounds(currAuthor, 0, 0, &xP, &yP, &w, &h);

  int quoterX;
  if (lineCount < 3) {
    startY = startY + lineH + quoteSpacer;
    quoterX = displayMarginW * 2;
  } else {
    startY = startY + quoteSpacer;
    quoterX = displayWidth - (w + displayMarginW);
  }

  display.setCursor(quoterX, startY);
  display.print(currAuthor);
  display.getTextBounds("-  ", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(quoterX - w, startY);
  display.print("-  ");

}

void renderCurrentWeather (int startY, int wday) {

  int hi;
  int lo;
  char sr[6];
  char sram[3];
  char ss[6];
  char ssam[3];
  char weekDay[5];
  char dayWeatherIcon[2];

  bool twh = 1;
  if (isTwentyFourHour()) {
    twh = 0;
  }

  if (wday == currDayNo) {

    strcpy(weekDay, todayWDay);

    hi = todayHiTemp[unitSetting];
    lo = todayLoTemp[unitSetting];

    strcpy(sr, todaySunrise[twh]);
    strcpy(sram, todaySunrise[2]);

    strcpy(ss, todaySunset[twh]);
    strcpy(ssam, todaySunset[2]);

    strcpy(dayWeatherIcon, dailyWeatherIcon[0]);
  } else {

    strcpy(weekDay, tomWDay);

    hi = tomHiTemp[unitSetting];
    lo = tomLoTemp[unitSetting];

    strcpy(sr, tomSunrise[twh]);
    strcpy(sram, tomSunrise[2]);

    strcpy(ss, tomSunset[twh]);
    strcpy(ssam, tomSunset[2]);

    strcpy(dayWeatherIcon, dailyWeatherIcon[1]);

  }

  display.setFont(&FreeSansBold18pt7b);
  display.getTextBounds("X", 0, 0, &xP, &yP, &w, &h);
  startY = startY + h;
  int startX = displayMarginW;
  int xTab = (displayWidth - (displayMarginW * 2)) / 3  + 10;

  int xLabel = displayMarginW;
  int xLeft = 20;
  int xMid = 125;
  int xRight = 280;
  int xLabelDay = xRight - 10;
  int yLabelDay = startY + 10;
  int yTab = 45;
  int dayBoxH = 85;
  int dayBoxW = 120;
  int dayBoxR = 4;

  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("X", 0, 0, &xP, &yP, &w, &h);
  startY += h;
  int lineTab = h + 4;


  //Row 1, Column 1
  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(xLeft, startY);
  display.print("Feels Like");


  //----
  char tempChar[4];
  itoa(currentFeelsTemp[unitSetting], tempChar, 10);

  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds(tempChar, 0, 0, &xP, &yP, &w, &h);

  //y += h + 9;//changed from 5 to 9

  display.setCursor(xLeft, startY + lineTab);
  display.print(tempChar);
  display.fillCircle(xLeft + w + 8, startY + lineTab - (h * .8), 4, TEXTCOLOR);
  display.fillCircle(xLeft + w + 8, startY + lineTab - (h * .8), 1, BGCOLOR);


  //-----


  //Row 1, Column 2
  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(xMid, startY);
  display.print("Air Quality");

  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(xMid, startY + lineTab);

  char * aq = aqDesc (currAQ);
  display.print(aq);
  free (aq);


  //row 2

  startY += yTab;
  //Row 2, Column 1
  startX = displayMarginW; //+ xTabStart;
  display.setCursor(xLeft, startY);

  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(xLeft, startY);
  display.print("Humidity");

  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(xLeft, startY + lineTab);
  display.print(currHumidity);
  display.print("%");


  //Row 2, Column 2
  display.setCursor(startX, startY);

  display.setFont(&FreeSansBold9pt7b);
  display.setCursor(xMid, startY);
  display.print("UV Index");

  display.setFont(&FreeSansBold12pt7b);
  display.setCursor(xMid, startY + lineTab);
  char * uv = uvDesc (currUV);
  display.print(uv);
  free (uv);


  //TODAY Column

  if (!showDailyWeather) {
    return;
  }

  display.setFont(&FreeSansBold7pt7b);
  display.getTextBounds("MON", 0, 0, &xP, &yP, &w, &h);
  display.drawRoundRect(xLabelDay, yLabelDay - h / 2, dayBoxW, dayBoxH, dayBoxR, TEXTCOLOR);
  display.fillRect(xLabelDay + 8, yLabelDay - h / 2 , w + 8, 2, BGCOLOR);

  display.setCursor(xLabelDay + 11, yLabelDay);
  display.setFont(&FreeSansBold7pt7b);
  display.print(weekDay);

  //startY -= yTab;

  yLabelDay += lineTab;

  int iconTab;

  //display.setCursor(xRight, yLabelDay);
  //display.setFont(&slateWeather14pt7b);
  //display.print(dayWeatherIcon);
  //display.getTextBounds(dayWeatherIcon, 0, 0, &xP, &yP, &w, &h);
  //iconTab = w;
  iconTab = 0;

  display.setFont(&FreeSansBold9pt7b);

  char tempHi[4];
  itoa(hi, tempHi, 10);
  display.getTextBounds(tempHi, 0, 0, &xP, &yP, &w, &h);

  display.setCursor(xRight + iconTab, yLabelDay);
  display.print(tempHi);
  display.fillCircle(xRight + iconTab + w + 8, yLabelDay  - (h * .8), 4, TEXTCOLOR);
  display.fillCircle(xRight + iconTab + w + 8, yLabelDay - (h * .8), 1, BGCOLOR);

  int xRightLow = xRight + iconTab + w + 8 + 4;

  //startY += lineTab;

  display.setCursor(xRightLow, yLabelDay);
  display.setFont(&FreeSansBold9pt7b);
  display.print(" | ");
  display.getTextBounds(" | ", 0, 0, &xP, &yP, &w, &h);
  int spacerW = w;


  char tempLo[4];
  itoa(lo, tempLo, 10);
  display.getTextBounds(tempLo, 0, 0, &xP, &yP, &w, &h);

  //display.setCursor(xRightLow + iconTab, yLabelDay);
  display.print(tempLo);
  display.fillCircle(xRightLow + spacerW + w + 8 + 4, yLabelDay  - (h * .8), 4, TEXTCOLOR);
  display.fillCircle(xRightLow + spacerW + w + 8 + 4, yLabelDay - (h * .8), 1, BGCOLOR);

  //startY += lineTab;
  yLabelDay += lineTab;

  display.setFont(&slateWeather14pt7b);
  display.getTextBounds("u", 0, 0, &xP, &yP, &w, &h);

  display.setCursor(xRight, yLabelDay + h / 4);
  display.setFont(&slateWeather14pt7b);
  display.print("u");
  display.setCursor(xRight + w, yLabelDay);
  display.setFont(&FreeSansBold9pt7b);
  display.print(" ");
  display.print(sr);

  if (twh > 0) {
    display.setFont(&FreeSansBold7pt7b);
    display.print(sram);
  }

  //startY += lineTab;
  yLabelDay += lineTab;

  display.setCursor(xRight, yLabelDay + h / 4);
  display.setFont(&slateWeather14pt7b);
  display.print("U");
  display.setCursor(xRight + w, yLabelDay);
  display.setFont(&FreeSansBold9pt7b);
  display.print(" ");
  display.print(ss);
  if (twh > 0) {
    display.setFont(&FreeSansBold7pt7b);
    display.print(ssam);
  }

}

void renderDeviceStatus() {

  int tab = 0;
  int hTab = 0;

  batteryManagement();

  display.setFont(&slateIcons14pt7b);
  display.getTextBounds(battlowG, 0, 0, &xP, &yP, &w, &h);

  display.setCursor(displayMarginW / 2, displayMarginH + h);

  if (low_battery && !usb_plugged_in) {
    display.print(battlowG);
    tab = w + 5;
    hTab = h / 4;
  } else if (!battery_charged && usb_plugged_in) {
    display.print(battchargeG );
    tab = w + 5;
    hTab = h / 4;
  } else if (battery_charged && usb_plugged_in) {
    display.print(battfullG);
    tab = w + 5;
    hTab = h / 4;
  }


  if (versionUpdateAvailable) {
    display.setFont(&slateIcons9pt7b);
    display.getTextBounds(downloadG, 0, 0, &xP, &yP, &w, &h);
    display.setCursor(tab + displayMarginW / 2, displayMarginH + h - hTab);
    display.print(downloadG);
    tab += w + 5;
  }

  if (wifiAttempting || wifiFailed) {
    display.setFont(&slateIcons9pt7b);
    display.getTextBounds(wifiG, 0, 0, &xP, &yP, &w, &h);
    display.setCursor(tab + displayMarginW / 2, displayMarginH / 3 + h);
    if (wifiAttempting) {
      display.setFont(&slateIcons9pt7b);
      display.print(wifiG);
    } else {
      display.print(resetwifiG);
    }
    tab += w + 5;
  }

}

void renderMenuWidgetItem(int arrayNum, int location) {

  int assignedTouchpad;
  switch (location) {
    case 0:
      location = BUTTON_C_PIN_X;
      assignedTouchpad = BUTTON_C;
      break;
    case 1:
      location = BUTTON_D_PIN_X;
      assignedTouchpad = BUTTON_D;
      break;
    case 2:
      location = BUTTON_E_PIN_X;
      assignedTouchpad = BUTTON_E;
      break;
    case 3:
      location = BUTTON_F_PIN_X;
      assignedTouchpad = BUTTON_F;
      break;
  }

  //assign touchpad to function
  menuIconArray[arrayNum].assignedTouchpad = assignedTouchpad;

  display.setFont(&FreeSans9pt7b);
  display.getTextBounds(menuIconArray[arrayNum].displayName, 0, 0, &xP, &yP, &w, &h);
  int textH = h * 1.5;
  int textW = w;

  display.setFont(&slateIcons14pt7b);
  display.getTextBounds(menuIconArray[arrayNum].displayIcon, 0, 0, &xP, &yP, &w, &h);
  display.setCursor(location - (w / 2), BUTTON_CDEF_PIN_Y - textH);
  display.print(menuIconArray[arrayNum].displayIcon);

  display.setFont(&FreeSans9pt7b);
  display.setCursor(location - (textW / 2), BUTTON_CDEF_PIN_Y);
  display.print(menuIconArray[arrayNum].displayName);

}

void resetMenuButtons() {
  for (int j = 0; j < numOfMenuItems; j++) {
    menuIconArray[j].assignedTouchpad = 0;
  }
}

void renderMenuWidget() {

  //first clear all menu assignments
  resetMenuButtons();

  if (menuStart >= numOfMenuItems) {
    menuStart = 0;
  }

  //render menu icon and assign touchpad
  for (int i = 0; i < 4; i++) {
    renderMenuWidgetItem(menuStart, i);
    menuStart++;
    if (menuStart == numOfMenuItems) {
      break;
    }
  }
}

void renderPomodoroMenu() {
  display.setTextSize(1);
  display.setFont(&FreeSansBold9pt7b);

  if (pomodoroInitiated) {

    if (!pomodoroPaused) {
      display.getTextBounds("PAUSE", 0, 0, &xP, &yP, &w, &h);
      display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
      display.print("PAUSE");
    } else {
      display.getTextBounds("RESUME", 0, 0, &xP, &yP, &w, &h);
      display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
      display.print("RESUME");
    }

    display.getTextBounds("SKIP", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_D_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("SKIP");

    display.getTextBounds("CANCEL", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_E_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("CANCEL");

  } else {

    display.getTextBounds("START", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("START");
  }


  display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("EXIT");

  int textH = h * 1.5;
  display.setFont(&slateIcons14pt7b);
  if (pomodoroInitiated) {

    if (!pomodoroPaused) {
      display.getTextBounds(pauseG, 0, 0, &xP, &yP, &w, &h);
      display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y - textH);
      display.print(pauseG);//Pause
    } else {
      display.getTextBounds(playG, 0, 0, &xP, &yP, &w, &h);
      display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y - textH);
      display.print(playG); //Play
    }

    display.getTextBounds(forwardG, 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_D_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y - textH);
    display.print(forwardG);

    display.getTextBounds(cancelG, 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_E_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y - textH);
    display.print(cancelG);
  } else {

    display.getTextBounds(playG, 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y - textH);
    display.print(playG); //Play

  }

  display.getTextBounds(menuG, 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y - textH);
  display.print(menuG);

}

void renderWeatherWidget(int startY) {

  display.setFont(&slateWeather30pt7b);
  display.getTextBounds(currWeatherIcon, 0, 0, &xP, &yP, &w, &h);

  int iconY = startY + h + 10;
  int iconX = displayMarginW;
  int iconW = w;
  int iconH = h;
  display.setCursor(iconX, iconY);
  display.print(currWeatherIcon);

  char currWeatherChar[4];
  //itoa(currWeather, currWeatherChar, 10);
  itoa(currentTemp[unitSetting], currWeatherChar, 10);

  display.setFont(&FreeSansBold18pt7b);
  //display.setFont(&FreeSansBold14pt7b);
  display.getTextBounds(currWeatherChar, 0, 0, &xP, &yP, &w, &h);

  int tempY = iconY + h + 9;
  int degreeAdd = 7;
  int outerDegreeRadius = 4;
  int innerDegreeRadius = 1;
  int totalTempW = w + degreeAdd + outerDegreeRadius;
  int tempX = iconX + (iconW / 2) - (totalTempW / 2);

  display.setCursor(tempX, tempY);
  display.print(currWeatherChar);

  display.fillCircle(tempX + w + degreeAdd, tempY - (h * .8), outerDegreeRadius, TEXTCOLOR);
  display.fillCircle(tempX + w + degreeAdd, tempY - (h * .8), innerDegreeRadius, BGCOLOR);

}


int renderAlarmWidget(int startY) {

  display.setFont(&slateIcons9pt7b);
  display.getTextBounds(alarmG, 0, 0, &xP, &yP, &w, &h);

  int alarmY = startY + displayMarginH * 1.25 + h;
  display.setCursor(TIMING_X, alarmY);

  if (!alarmActive) {
    display.print(alarmG);
  } else {
    showMenu = false; //do not show menu
    resetMenuButtons(); //clear out all menu buttons if the device was awake

    display.print(bellG);
    if (!timerActive) {
      handleBuzzer();
    }
  }

  display.setFont(&FreeSansBold12pt7b);
  int alarm_hr = getAlarmHr(); //rtc.getAlarmHours();
  int alarm_min = getAlarmMin(); //rtc.getAlarmMinutes();
  boolean alarm_pm = false;

  boolean twentyFourHourVal = isTwentyFourHour();

  if (!twentyFourHourVal) {

    if (alarm_hr > 12) {
      alarm_pm = true;
      alarm_hr = alarm_hr - 12;
    }

    if (alarm_hr == 0) {
      alarm_hr = 12;
    }

  }

  char alm[6];

  if (alarm_hr < 10 && twentyFourHourVal) {
    sprintf(alm, " 0%d:%02d", alarm_hr, alarm_min);
  } else {
    sprintf(alm, " %d:%02d", alarm_hr, alarm_min);
  }

  display.print(alm);

  if (!twentyFourHourVal) {

    display.setFont(&FreeSansBold9pt7b);
    if (alarm_pm) {
      display.print(" PM");
    } else {
      display.print(" AM");
    }

  }

  return alarmY - displayMarginH * .5;
}

void handleTimer() {

  if (timerActive) {
    return;
  }
  int32_t decrement;
  int32_t currentEpoch;

  while (!rtc.updateTime());
  currentEpoch = rtc.getUNIX();

  decrement = currentEpoch - lastEpoch;
  lastEpoch = currentEpoch;

  int32_t t = hmsToSeconds(timerH, timerM, timerS, decrement);

  if ((timerH == 86) || (decrement < 0)) { //this is an error when sometimes the timer does not get to zero and counts to 86H and 27 mins
    timerH = 0;
    timerM = 0;
    timerS = 0;
    t = 0;
  }

  if (t == 0) {
    timerActive = true;
    rtc.disableTimerInterrupt();
    rtc.disableTimer();
    return;
  }

  timerS = t % 60;

  t = (t - timerS) / 60;
  timerM = t % 60;

  t = (t - timerM) / 60;

  timerH = t;

  if (timerH < 1 && timerM < 2 && !timerSubMin) {
    setRTCTimer(1);
    timerSubMin = true;
  }

  if (pomodoroPaused && !pomodoroSaved) {
    pomodoroSaved = true;
    t = hmsToSeconds(timerH, timerM, timerS, 0);
    //savePomodoroHMS(t);
    rtc.disableTimerInterrupt();
    rtc.disableTimer();
    saveTimerHMS(t);
  }

  if (pomodoroNeedInterrupt && timerS == 59) {
    pomodoroNeedInterrupt = false;
    setRTCTimer(60);
  }


}

void renderTimerWidget(int startY) {

  display.setFont(&slateIcons9pt7b);
  display.getTextBounds(alarmG, 0, 0, &xP, &yP, &w, &h);

  int timerY = startY + displayMarginH * 1.25 + h;
  display.setCursor(TIMING_X, timerY);

  if (timerActive) {
    display.print(bellG);
    display.setFont(&FreeSansBold12pt7b);
    display.print(" 00:00:00");

    if (!alarmActive) {
      handleBuzzer();
    }

  } else {

    if (pomodoroEnabled) {
      if (!pomodoroPaused && !pomodoroSaved) {
        handleTimer();
      }
    } else {
      handleTimer();
    }

    display.setFont(&slateIcons9pt7b);

    if (pomodoroEnabled) {
      if (pomodoroSaved && pomodoroPaused) {
        display.print(pauseG);
      } else {

        if (pomodoroItem % 2 == 0) { //if even, then break
          if (pomodoroItem == 8) { //long break
            display.print(spoonknifeG);
          } else { //short break
            display.print(mugG);
          }
        } else { //if odd, then work
          display.print(clipboardG);
        }
      }
    } else {
      display.print(hourglassG);
    }

    display.setFont(&FreeSansBold12pt7b);

    char timerChar[10];
    sprintf(timerChar, " %02d:%02d", timerH, timerM);
    display.print(timerChar);

    if ((timerH == 0 && timerM < 2) || showSeconds) {

      char tmrS[4];
      sprintf(tmrS, ":%02d", timerS);
      display.print(tmrS);

    } else {
      display.print(":---");
    }
  }

}

void pomodoroTracker(int startY) {

  display.setFont(&slateIcons14pt7b);
  display.getTextBounds(tomatoG, 0, 0, &xP, &yP, &w, &h);

  int iconY = startY + h + 20;
  int iconX = displayMarginW;
  int iconW = w;
  int iconH = h;
  display.setCursor(iconX, iconY);
  display.print(tomatoG);

  display.setFont(&FreeSansBold9pt7b);
  display.getTextBounds("POMODORO", 0, 0, &xP, &yP, &w, &h);

  startY = startY + iconH + 5;
  int startX = displayMarginW + iconW + 5;
  display.setCursor(startX, startY);
  display.print("POMODORO");
  startY = startY + h * 1.5;

  display.setCursor(startX, startY);
  if ((pomodoroItem % 2 == 0) && (pomodoroItem != 0)) { //if even, then break
    display.print("BREAK #");
  } else { //if odd, then work
    display.print("TASK #");
  }

  int taskItem = (pomodoroItem + 1) / 2;

  if (taskItem == 0) {
    taskItem = 1;
  }
  display.print(taskItem);

}


void renderMainDisplay(bool inverse = false) {

  syncTimeRTCEpoch();

  Serial.println("render display");

  currentDisplay = HOME;

  char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
  //char monthsOfTheYear[12][12] = {"January", "Febuary", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
  char monthsOfTheYear[12][5] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sept", "Oct", "Nov", "Dec"};

  if (rtc.readAlarmInterruptFlag()) {
    rtc.clearAlarmInterruptFlag();
    if (alarmEnabled) {
      if (manualAlarmCheck()) { //sometimes we incorrectly trigger the alarm, so lets check to ensure the real time and alarm time matches
        overrideAlarmEnabled = false;
        alarmActive = true;
        Serial.println("Alarm active!!!");
      } else {
        //do nothing; lets assume that we should trigger an alarm at a later point in time
      }
    } else {
      //we don't need the alarm at all; for some reason we incorrectly triggered the alarm, so lets remove the
      rtc.disableAlarmInterrupt();
      rtc.clearAlarmInterruptFlag();
    }
  }

  if (overrideAlarmEnabled) {
    alarmActive = true;
    Serial.println("Alarm active - manual override!!!");
  }

  //rtc.set12Hour();

  boolean twentyFourHourVal = isTwentyFourHour();

  if (!twentyFourHourVal) {
    rtc.set12Hour();
  } else {
    rtc.set24Hour();
  }

  while (!rtc.updateTime());

  if (rtc.readPeriodicUpdateInterruptFlag()) {
    rtc.clearPeriodicUpdateInterruptFlag();
    Serial.println("Periodic update!");
  }

  if (rtc.readTimerInterruptFlag()) {
    rtc.clearTimerInterruptFlag();
    Serial.println("Timer update!");
    //handleTimer(); //we call this twice
  }


  int hr_i;
  int m_i;
  int wday_i;
  int dt_i;
  int mon_i;

  boolean pm;

  struct tm timeinfo;

  if (!getLocalTime(&timeinfo))
  {

    hr_i = rtc.getHours();
    m_i = rtc.getMinutes();
    pm = rtc.isPM();
    mon_i = rtc.getMonth();
    dt_i = rtc.getDate();
    wday_i = rtc.getWeekday();

  } else {

    hr_i = timeinfo.tm_hour;
    m_i = timeinfo.tm_min;
    mon_i = timeinfo.tm_mon + 1;
    dt_i = timeinfo.tm_mday;
    wday_i = timeinfo.tm_wday;

    if (!twentyFourHourVal) {
      pm = false;
      if (hr_i > 11) {
        pm = true;
        if (hr_i > 12) {
          hr_i = hr_i - 12;
        }
      }
    }

    /*pm = false;
      if (hr_i > 11) {
      pm = true;
      if (hr_i > 12) {
        hr_i = hr_i - 12;
      }
      }*/

  }

  char hr[3];

  if (!twentyFourHourVal) {
    if (hr_i == 0) {
      hr_i = 12;
    }
    sprintf(hr, "%d", hr_i);

  } else {
    if (hr_i < 10) {
      sprintf(hr, "0%d", hr_i);
    } else {
      sprintf(hr, "%d", hr_i);
    }
  }



  char mi[4];
  sprintf(mi, ":%02d", m_i);

  char dateChar[80];
  sprintf(dateChar, "%s, %s %u\n", daysOfTheWeek[wday_i], monthsOfTheYear[mon_i - 1], dt_i);

  if (inverse) {
    display.fillScreen(TEXTCOLOR);
    display.setTextColor(BGCOLOR);
  } else {
    display.fillScreen(BGCOLOR);
    display.setTextColor(TEXTCOLOR);
  }

  display.setFont(&FreeSansBold12pt7b);

  int dateH = 0;
  int displayMarginWTemp = displayMarginW * 2;

  display.getTextBounds(dateChar, 0, 0, &xP, &yP, &w, &h);
  int dateY = 0 + displayMarginH / 2 + h;
  int dateX = displayWidth - (w + displayMarginWTemp);
  dateH = h;

  display.setCursor(dateX, dateY);
  display.println(dateChar);
  display.setFont(&FreeSansBold60pt7b);

  display.getTextBounds(":", 0, 0, &xP, &yP, &w, &h);
  int colonW = (w / 2) + 2;

  display.getTextBounds(":00", 0, 0, &xP, &yP, &w, &h);
  int colonX = displayWidth - (w + displayMarginWTemp);
  int timeY =  dateH + displayMarginH + h;
  display.setCursor(colonX, timeY);
  display.print(mi);

  display.getTextBounds(hr, 0, 0, &xP, &yP, &w, &h);
  display.setCursor(colonX - (w + colonW / 2) , timeY);
  display.print(hr);

  if (showWeather) { //& !isAlarmActive()
    renderWeatherWidget(dateY);
  }

  int timerY = timeY;

  if (pomodoroEnabled || showPomodoroMenu) {
    pomodoroTracker(timeY);
  }

  if (alarmEnabled) {
    timerY = renderAlarmWidget(timeY);
  }

  if (timerEnabled) {
    renderTimerWidget(timerY);
  }

  renderDeviceStatus();

  if (showMenu) {
    if (showPomodoroMenu) {
      renderPomodoroMenu();
    } else {
      renderMenuWidget();
    }
  } else {
    if (!alarmEnabled && !timerEnabled) {
      if (showQuote && tempScreenSetting == QUOTESCREEN) { //re-add after coding/testing
        renderQuote(timerY);
      }
      if (showWeather && tempScreenSetting == WEATHERSCREEN) {
        renderCurrentWeather(timerY, wday_i);
      }

    }
  }

  display.refresh();
}


void keepMenu() {
  menuStart = (menuStart < 4) ? 0 : menuStart - 4;
  renderMainDisplay();
}

void exitScreensRoutine(boolean menuKeep = true) {
  timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP_QUICK;
  delay(DEBOUNCE); //adding a second debounce when we go back to the main screen
  if (menuKeep) {
    keepMenu();
  }

}
