#define alarmSettingPartH 0
#define alarmSettingPartM 1
#define alarmSettingPartPM 2
volatile byte alarmSettingH = 0;
volatile byte alarmSettingM = 0;
volatile bool alarmSettingPM = false;
volatile byte alarmSettingPart = 0; //0 - hour; 1 - min; 2 - am/pm
volatile bool startAlarmSetting = false;
bool alarmSettingFirstLoad = true;

void renderSetAlarmDisplay() {

  currentDisplay = ALARM;
  display.fillScreen(BGCOLOR);
  display.setTextSize(1);

  int x = 10;
  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("SET ALARM", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(x, displayMarginH + h);
  display.print("SET ALARM");

  boolean twentyFourHourVal = isTwentyFourHour();

  if (!startAlarmSetting) {


    //while (!rtc.updateTime());

    alarmSettingH = getAlarmHr(); //rtc.getAlarmHours();
    alarmSettingM = getAlarmMin(); //rtc.getAlarmMinutes();

    if (!twentyFourHourVal) {

      if (alarmSettingH > 12) {
        alarmSettingPM = true;
        alarmSettingH = alarmSettingH - 12;
      }

      if (alarmSettingH == 0) {
        alarmSettingH = 12;
      }

    }

    alarmSettingPart = 0;
    startAlarmSetting = true;
  }

  char hr[3];

  if (twentyFourHourVal && alarmSettingH < 10) {
    sprintf(hr, "0%u", alarmSettingH);
  } else {
    sprintf(hr, "%u", alarmSettingH);
  }

  char mi[4];
  sprintf(mi, ":%02u", alarmSettingM);

  display.setTextColor(TEXTCOLOR);

  display.setFont(&slateIcons14pt7b);
  display.getTextBounds("t", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_AB_PIN_X - h / 2, BUTTON_A_PIN_Y);
  display.print(uptoggleG);

  display.setCursor(BUTTON_AB_PIN_X - h / 2, BUTTON_B_PIN_Y);
  display.print(downtoggleG);

  display.setTextSize(1);
  display.setFont(&FreeSansBold9pt7b);

  display.getTextBounds("SET", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("SET");

  display.getTextBounds("BACK", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_D_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("BACK");

  display.setCursor(BUTTON_E_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("");

  display.getTextBounds("CANCEL", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("CANCEL");

  display.setTextColor(TEXTCOLOR);
  if (alarmSettingPart == 1) {
    display.setFont(&FreeSansBold60pt7b);
  } else {
    display.setFont(&FreeSansBold50pt7b);
  }

  display.getTextBounds(":", 0, 0, &xP, &yP, &w, &h);
  int colonW = w / 2;

  display.getTextBounds(":00", 0, 0, &xP, &yP, &w, &h);
  int colonX = displayWidth / 2 - colonW * 4 ;

  int timeY =  displayHeight / 2 + h / 2 ;
  display.setCursor(colonX, timeY);
  display.print(mi);

  if (!twentyFourHourVal) {

    if (alarmSettingPart == 2) {
      display.setFont(&FreeSansBold24pt7b);
    } else {
      display.setFont(&FreeSansBold12pt7b);
    }
    if (alarmSettingPM) {
      display.print("PM");
    } else {
      display.print("AM");
    }
  }

  if (alarmSettingPart == 0) {
    display.setFont(&FreeSansBold60pt7b);
  } else {
    display.setFont(&FreeSansBold50pt7b);
  }
  display.getTextBounds(hr, 0, 0, &xP, &yP, &w, &h);

  display.setCursor(colonX - (w + colonW) , timeY);
  display.print(hr);
  display.refresh();

  if (alarmSettingFirstLoad && alarmSettingPart == 0) { //blink to indicate that the alarm is ready to be set
    delay(150);
    for (int i = 0; i < 4; i++) {

      if (i % 2 == 0) {
        display.setTextColor(BGCOLOR);
      } else {
        display.setTextColor(TEXTCOLOR);
      }
      display.setCursor(colonX - (w + colonW) , timeY);
      display.print(hr);
      delay(100);
      display.refresh();
    }
    alarmSettingFirstLoad = false;
    display.setTextColor(TEXTCOLOR);
  }
}

void incrementAlarmDisplay(int button) {

  int alarmHMax;
  int alarmHMin;

  boolean twentyFourHourVal = isTwentyFourHour();

  if (twentyFourHourVal) {
    alarmHMax = 23;
    alarmHMin = 0;
  } else {
    alarmHMax = 12;
    alarmHMin = 1;
  }

  if (alarmSettingPart == alarmSettingPartH) {
    switch (button)
    {
      case BUTTON_A :
        if (alarmSettingH == alarmHMax) {
          alarmSettingH = alarmHMin;
        } else {
          alarmSettingH++;
        }
        renderSetAlarmDisplay();
        break;
      case BUTTON_B :
        if (alarmSettingH == alarmHMin) {
          alarmSettingH = alarmHMax;
        } else {
          alarmSettingH--;
        }
        renderSetAlarmDisplay();
        break;
      case BUTTON_C :
        alarmSettingPart = alarmSettingPartM;
        renderSetAlarmDisplay();
        break;
      case BUTTON_D :
        //timeoutCounter = 0;
        exitScreensRoutine();
        //renderMainDisplay();
        break;
      case BUTTON_F :
        disableRTCAlarm();
        startAlarmSetting = false;
        //timeoutCounter = 0;
        exitScreensRoutine();
        //renderMainDisplay();
        break;
    }
    return;
  } else if (alarmSettingPart == alarmSettingPartM) {
    switch (button)
    {
      case BUTTON_A :
        if (alarmSettingM == 59) {
          alarmSettingM = 0;
        } else {
          alarmSettingM++;
        }
        renderSetAlarmDisplay();
        break;
      case BUTTON_B :
        if (alarmSettingM == 0) {
          alarmSettingM = 59;
        } else {
          alarmSettingM--;
        }
        renderSetAlarmDisplay();
        break;
      case BUTTON_C :

        if (twentyFourHourVal) {
          startAlarmSetting = false;
          setRTCAlarm(alarmSettingH, alarmSettingM, alarmSettingPM);
          exitScreensRoutine();
        } else {
          alarmSettingPart = alarmSettingPartPM;
          renderSetAlarmDisplay();
        }

        break;
      case BUTTON_D :
        alarmSettingPart = alarmSettingPartH;
        renderSetAlarmDisplay();
        break;
      case BUTTON_F :
        disableRTCAlarm();
        startAlarmSetting = false;
        //timeoutCounter = 0;
        exitScreensRoutine();
        //renderMainDisplay();
        break;
    }
    return;
  } else if (alarmSettingPart == alarmSettingPartPM) {
    switch (button)
    {
      case BUTTON_A :
        alarmSettingPM = !alarmSettingPM;
        renderSetAlarmDisplay();
        break;
      case BUTTON_B :
        alarmSettingPM = !alarmSettingPM;
        renderSetAlarmDisplay();
        break;
      case BUTTON_C :
        startAlarmSetting = false;
        setRTCAlarm(alarmSettingH, alarmSettingM, alarmSettingPM);
        //timeoutCounter = 0;
        exitScreensRoutine();
        //renderMainDisplay();
        break;
      case BUTTON_D :
        alarmSettingPart = alarmSettingPartM;
        renderSetAlarmDisplay();
        break;
      case BUTTON_F :
        disableRTCAlarm();
        startAlarmSetting = false;
        //timeoutCounter = 0;
        exitScreensRoutine();
        //renderMainDisplay();
        break;
    }
    return;
  }
}
