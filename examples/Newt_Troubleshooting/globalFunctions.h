const char* ntpServer = "pool.ntp.org";
const char* ntpServerB = "time.nist.gov";

struct tm now;

RTC_DATA_ATTR unsigned long timekeepingEpoch;

RTC_DATA_ATTR byte timerOrigH = 0;
RTC_DATA_ATTR byte timerOrigM = 0;
RTC_DATA_ATTR volatile byte timerH = 0;
RTC_DATA_ATTR volatile byte timerM = 0;
RTC_DATA_ATTR volatile byte timerS = 0;

RTC_DATA_ATTR volatile byte pausedPomodoroM = 0;
RTC_DATA_ATTR volatile byte pausedPomodoroS = 0;

RTC_DATA_ATTR volatile int32_t lastEpoch;

// -----Twenty Four Hour Functions ---------
boolean isTwentyFourHour() {
  //return true;
  if (EEPROM.read(TWENTY_FOUR_HOUR_FORMAT_ADDR) != 1) {
    return false;
  }
  return true;
}

void toggleClockFormat() {
  if (EEPROM.read(TWENTY_FOUR_HOUR_FORMAT_ADDR) == 1) {
    EEPROM.write(TWENTY_FOUR_HOUR_FORMAT_ADDR, 0);
  } else {
    EEPROM.write(TWENTY_FOUR_HOUR_FORMAT_ADDR, 1);
  }
  EEPROM.commit();
}


// -----Unit of Measure Functions ---------

void setDefaultMeasureOnWifi(byte setting) {
  if (EEPROM.read(DEFAULT_MEASURE_ADDR) != setting) {
    EEPROM.write(DEFAULT_MEASURE_ADDR, setting);
    EEPROM.commit();
  }
}

void setSystemMeasure(byte setting = AUTOMEASURE) {
  EEPROM.write(MEASURE_ADDR, setting);
  EEPROM.commit();
}


byte getSystemMeasure() {

  byte reading = EEPROM.read(MEASURE_ADDR);

  if ((reading != IMPERIALMEASURE) && (reading != METRICMEASURE) && (reading != AUTOMEASURE)) {
    setSystemMeasure();
    reading = AUTOMEASURE;
  }

  return reading;
}

void saveShortTermMeasure() {
  byte val = getSystemMeasure();
  switch (val) {
    case AUTOMEASURE:
      unitSetting = EEPROM.read(DEFAULT_MEASURE_ADDR);
      break;
    default:
      unitSetting = val - 1;
      break;
  }
}

void toggleSystemMeasure() {

  byte reading = EEPROM.read(MEASURE_ADDR);

  reading++;
  if (reading > 2) {
    reading = 0;
  }
  setSystemMeasure(reading);
  saveShortTermMeasure();
}

// -----Dark/Light Mode Functions ---------

void setDefaultLightMode(byte setting = AUTOMODE) {
  EEPROM.write(DARKMODE_ADDR, setting);
  EEPROM.commit();
}

byte getLightMode() {
  byte reading = EEPROM.read(DARKMODE_ADDR);
  if ((reading != AUTOMODE) && (reading != DARKMODE) && (reading != LIGHTMODE)) {
    setDefaultLightMode();
    reading = AUTOMODE;
  }
  return reading;
}

void setColorMode() {

  byte lightSetting = getLightMode();

  if (lightSetting == AUTOMODE) {

    while (!rtc.updateTime());

    boolean twentyFourHourVal = isTwentyFourHour();

    int compare_hr = rtc.getHours(); //no need to refresh rtc register; this is done

    if (!twentyFourHourVal) {
      if (rtc.isPM()) {
        compare_hr = compare_hr + 12;
      }
      if (compare_hr > 24) {
        compare_hr = 0;
      }
    }

    if ((compare_hr > autoDarkH - 1) || (autoLightH > compare_hr)) {
      lightSetting = DARKMODE;
    } else {
      lightSetting = LIGHTMODE;
    }
  }


  switch (lightSetting) {
    case DARKMODE :
      TEXTCOLOR = 1;
      BGCOLOR = 0;
      break;
    case LIGHTMODE :
      TEXTCOLOR = 0;
      BGCOLOR = 1;
      break;
    default:
      TEXTCOLOR = 0;
      BGCOLOR = 1;
      break;
  }
}

void toggleLightMode() {
  byte reading = EEPROM.read(DARKMODE_ADDR);
  reading++;
  if (reading > 2) {
    reading = 0;
  }
  setDefaultLightMode(reading);
  setColorMode();
}

// -----Default Screen Functions ---------

void checkTempScreenSetting() {
  if (savedScreenSetting == AUTOSCREEN) {
    if (refreshDataCount % refreshScreenM  == 0) {
      tempScreenSetting++;
      if (tempScreenSetting > 2) {
        tempScreenSetting = 1;
      }
    }
  } else {
    tempScreenSetting = savedScreenSetting;
  }
}

void setDefaultScreen(byte setting = AUTOSCREEN) {
  EEPROM.write(DEFAULTSCREEN_ADDR, setting);
  EEPROM.commit();
  savedScreenSetting = setting;
  checkTempScreenSetting();
}

byte getDefaultScreen() {
  byte reading = EEPROM.read(DEFAULTSCREEN_ADDR);
  if ((reading != QUOTESCREEN) && (reading != WEATHERSCREEN) && (reading != AUTOSCREEN)) {
    setDefaultScreen();
    reading = AUTOSCREEN;
  }
  return reading;
}

void toggleDefaultScreen() {
  byte reading = EEPROM.read(DEFAULTSCREEN_ADDR);
  reading++;
  if (reading > 2) {
    reading = 0;
  }
  setDefaultScreen(reading);
}


//------Clear EEPROM------------

void factoryErase() {
  for (int i = 0; i < EEPROM_SIZE + 1; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();


}

//-----------Weather Functions---------------------

char * aqDesc (int reading)
{
  char * buf = (char *) malloc (14);

  if (reading < 0) {
    strcpy (buf, "");
  } else if (reading < 51) {
    strcpy (buf, "Good");
  } else if (reading < 101) {
    strcpy (buf, "Moderate");
  } else if (reading < 151) {
    strcpy (buf, "Sensitive");
  } else if (reading < 201) {
    strcpy (buf, "Unhealthy");
  } else if (reading < 301) {
    strcpy (buf, "V.Unhealthy");
  } else {
    strcpy (buf, "Hazardous");
  }

  return buf;
}

char * uvDesc (int reading)
{
  char * buf = (char *) malloc (14);

  if (reading < 0) {
    strcpy (buf, "");
  } else if (reading < 3) {
    strcpy (buf, "Low");
  } else if (reading < 6) {
    strcpy (buf, "Moderate");
  } else if (reading < 8) {
    strcpy (buf, "High");
  } else if (reading < 11) {
    strcpy (buf, "Very High");
  } else {
    strcpy (buf, "Extreme");
  }

  return buf;
}

char * uvIcon (int reading)
{
  char * buf = (char *) malloc (4);

  if (reading < 0) {
    strcpy (buf, "G");
  } else if (reading < 3) {
    strcpy (buf, "G");
  } else if (reading < 6) {
    strcpy (buf, "Y");
  } else if (reading < 8) {
    strcpy (buf, "O");
  } else if (reading < 11) {
    strcpy (buf, "X");
  } else {
    strcpy (buf, "P");
  }

  return buf;
}




// -----Time Functions ---------

unsigned long getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return (0);
  }
  time(&now);
  return now;
}

void setRTC_UNIX(uint32_t epoch) {
  rtc.setUNIX(epoch);
}

void setRTC_TZ() {

  setenv("TZ", tz_info, 1);
  tzset();

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  rtc.set24Hour();
  if (rtc.setTime(timeinfo.tm_sec, timeinfo.tm_min, timeinfo.tm_hour, timeinfo.tm_wday, timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900) == false) {
    Serial.println("Something went wrong setting the time");
  }

  if (!isTwentyFourHour) {
    rtc.set12Hour();
  }

  time_t now;
  time(&now);
  rtc.setUNIX(now);
}

void syncTimeNTP() {
  configTime(0, 0, ntpServer, ntpServerB);
  unsigned long epoch = getTime();
  setRTC_TZ();
}

void syncTimeRTCEpoch() {

  Serial.println("Start resync");

  setenv("TZ", tz_info, 1);
  tzset();

  while (!rtc.updateTime());
  int32_t unixtime = rtc.getUNIX();
  timeval epoch = {unixtime, 0};
  settimeofday((const timeval*)&epoch, 0);
  Serial.println("Stop resync");

}


void setRTCInterrupts() {

  rtc.enablePeriodicUpdateInterrupt(false, true);
  rtc.disableClockOut();
  rtc.enableClockOut(FD_CLKOUT_1);

}

void initRTCInterrupts() {

  //Serial.print("Initing RTC");
  rtc.disableAlarmInterrupt();
  rtc.clearAlarmInterruptFlag();
  rtc.disableTimer();
  rtc.disableTimerInterrupt();
  rtc.clearTimerInterruptFlag();

  setRTCInterrupts();
}


void handleBuzzer() {

  outputTone = !outputTone;
  if (outputTone) {
    digitalWrite(SPKR_CTRL, HIGH);
    ledcWrite(BUZZER_CHANNEL, BEEP_DUTY_CYCLE);
    ledcWriteTone(BUZZER_CHANNEL, BEEP_FREQUENCY);
    timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP_TIMER;
  } else {
    digitalWrite(SPKR_CTRL, LOW);
    ledcWriteTone(BUZZER_CHANNEL, 0);
    ledcWrite(BUZZER_CHANNEL, 0);
    timeoutCounter = 0;
  }
}

void disableBuzzer() {
  outputTone = false;
  digitalWrite(SPKR_CTRL, LOW);
  digitalWrite(BUZZER, LOW);
  ledcDetachPin(BUZZER);
  ledcWriteTone(BUZZER_CHANNEL, 0);
  ledcWrite(BUZZER_CHANNEL, 0);
}

void disableRTCAlarm() {
  rtc.disableAlarmInterrupt();
  rtc.clearAlarmInterruptFlag();
  alarmActive = false;
  alarmEnabled = false;
  rtc.disableClockOut();
  rtc.enableClockOut(FD_CLKOUT_1);

  disableBuzzer();
}

void setRTCTimer(byte ticks) {

  Serial.print("Ticks:");
  Serial.print(ticks);
  rtc.setTimer(true, 1, ticks, true, true, false);
  timerEnabled = true;

}

void disableRTCTimer() {
  timerSubMin = false;
  rtc.disableTimer();
  rtc.disableTimerInterrupt();
  rtc.clearTimerInterruptFlag();

  timerActive = false;
  timerEnabled = false;
  timerOrigH = 0;
  timerOrigM = 0;
  timerH = 0;
  timerM = 0;
  timerS = 0;

  disableBuzzer(); //this wasn't here - was this a mistake? I added it v 0.0.12.Gamma

}

void setRTCAlarm(byte hourAlarmValue, byte minuteAlarmValue, boolean pm) {

  alarmEnabled = true;

  boolean twentyFourHourVal = isTwentyFourHour();

  if (!twentyFourHourVal) {

    if (hourAlarmValue == 12 && !pm) {
      hourAlarmValue = 0;
    }

    if (pm) {
      hourAlarmValue = hourAlarmValue + 12;
    };

  }


  rtc.enableAlarmInterrupt(minuteAlarmValue, hourAlarmValue, 2, false, 4, false);
  rtc.disableClockOut();
  rtc.enableClockOut(FD_CLKOUT_1);

  Serial.println("alarm setting:");


}

boolean isTimeSet() {

  while (!rtc.updateTime());

  if (rtc.getDate() > 0) {
    return true;
  }
  return false;
}

void initRTC(boolean resetRTC) {

  //if (rtc.begin(Wire,false,true,false,false) == false) {
  //if (rtc.begin() == false) {
  if (rtc.begin(resetRTC) == false) {
    Serial.println("Something went wrong with RTC, forcing restart");
    Wire.endTransmission(true);
    esp_sleep_enable_timer_wakeup(1 * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
    while (1);
  }
  //rtc.setBackupSwitchoverMode(0); moved this to rtc.begin function
}

uint8_t getAlarmMin() {
  uint8_t m = rtc.BCDtoDEC(rtc.readRegister(RV3028_MINUTES_ALM));

  if (m > 59) {
    m = 0;
  }
  return m;
}

uint8_t getAlarmHr() {
  uint8_t h = rtc.BCDtoDEC(rtc.readRegister(RV3028_HOURS_ALM));

  if (h > 23) {
    h = 0;
  }
  return h;
}

boolean manualAlarmCheck() {
  //rtc.set12Hour();
  while (!rtc.updateTime());

  boolean twentyFourHourVal = isTwentyFourHour();

  int alarm_hr = getAlarmHr();
  int alarm_min = getAlarmMin();
  int compare_hr = rtc.getHours(); //no need to refresh rtc register; this is done
  int compare_min = rtc.getMinutes();

  if (!twentyFourHourVal) {
    if (rtc.isPM()) {
      compare_hr = compare_hr + 12;
    }
    if (compare_hr > 24) {
      compare_hr = 0;
    }
  }

  if ((compare_hr == alarm_hr) && (compare_min == alarm_min)) {
    return true;
  } else {
    return false;
  }
}

int32_t hmsToSeconds(int H, int M, int S, int decrement) {

  int32_t t = (H * 60 * 60) + (M * 60) + S - decrement;
  return t;
}

void saveTimerHMS(int32_t t) {

  timerS = t % 60;
  t = (t - timerS) / 60;
  timerM = t % 60;

  t = (t - timerM) / 60;
  timerH = t;

}
