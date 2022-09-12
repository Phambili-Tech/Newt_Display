#define defaultTimerSettingH 0
#define defaultTimerSettingM 10
boolean volatile startTimerSetting = false;
volatile byte timerSettingH = defaultTimerSettingH;
volatile byte timerSettingM = defaultTimerSettingM;
volatile byte timerSettingPart = 0; //0 - hour; 1 - min;
volatile bool timerSettingFirstLoad = true;
#define timerSettingPartH 0
#define timerSettingPartM 1


void renderSetTimerDisplay() {

  currentDisplay = TIMER;
  display.fillScreen(BGCOLOR);
  display.setTextSize(1);

  int x = 10;
  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("SET TIMER", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(x, displayMarginH + h);

  if (pomodoroEnabled) {
    currentDisplay = CANCEL;
    display.print("TIMER");

    display.setTextColor(TEXTCOLOR);
    display.setFont(&FreeSansBold9pt7b);

    display.setCursor(x, displayMarginH + h * 3);
    display.println("You must cancel the pomodoro timer first.");

    display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("EXIT");

    display.refresh();
    return;
  }

  display.print("SET TIMER");

  if (!startTimerSetting) {

    if (!timerEnabled) {
      //Serial.println("We need to set up a new timer");
      timerSettingH = 0;
      timerSettingM = 0;
      timerSettingPart = 0;
      startTimerSetting = true;
    } else {
      timerSettingH = timerH;
      timerSettingM = timerM;
      if (timerM == 0 && timerS > 0) {
        timerSettingM = 1;
      }
      timerSettingPart = 0;
      startTimerSetting = true;
    }
  }

  char hr[3];
  sprintf(hr, "%02d", timerSettingH);
  char mi[3];
  sprintf(mi, "%02d", timerSettingM);

  display.setTextColor(TEXTCOLOR);

  display.setFont(&slateIcons14pt7b);
  display.getTextBounds(stopwatchG, 0, 0, &xP, &yP, &w, &h);
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
  if (timerSettingPart == 1) {
    display.setFont(&FreeSansBold60pt7b);
  } else {
    display.setFont(&FreeSansBold50pt7b);
  }

  display.getTextBounds("00", 0, 0, &xP, &yP, &w, &h);
  int minX = displayWidth / 2 ;

  int timeY =  displayHeight / 2 + h / 2 ;
  display.setCursor(minX + displayMarginW / 2, timeY);
  display.print(mi);

  display.setFont(&FreeSansBold12pt7b);
  display.print("m");

  display.getTextBounds("h", 0, 0, &xP, &yP, &w, &h);
  int h_AbrX = (minX - w) - displayMarginW / 2;
  display.setCursor(h_AbrX, timeY);
  display.print("h");


  if (timerSettingPart == 0) {
    display.setFont(&FreeSansBold60pt7b);
  } else {
    display.setFont(&FreeSansBold50pt7b);
  }
  display.getTextBounds(hr, 0, 0, &xP, &yP, &w, &h);

  int hX = h_AbrX - (w) - displayMarginW / 2;

  display.setCursor(hX , timeY);
  display.print(hr);
  display.refresh();

  if (timerSettingFirstLoad && timerSettingPart == 0) {
    delay(150);
    for (int i = 0; i < 4; i++) {

      if (i % 2 == 0) {
        display.setTextColor(BGCOLOR);
      } else {
        display.setTextColor(TEXTCOLOR);
      }
      display.setCursor(hX , timeY);
      display.print(hr);
      delay(150);
      display.refresh();
    }
    timerSettingFirstLoad = false;
    display.setTextColor(TEXTCOLOR);
  }

}




void incrementTimerDisplay(int button) {

  if (timerSettingPart == timerSettingPartH) {
    switch (button)
    {
      case BUTTON_A :
        if (timerSettingH == 6) {
          timerSettingH = 0;
        } else {
          timerSettingH++;
        }
        renderSetTimerDisplay();
        break;
      case BUTTON_B :
        if (timerSettingH == 0) {
          timerSettingH = 6;
        } else {
          timerSettingH--;
        }
        renderSetTimerDisplay();
        break;
      case BUTTON_C :
        timerSettingPart = timerSettingPartM;
        renderSetTimerDisplay();
        break;
      case BUTTON_D :
        startTimerSetting = false;
        //timeoutCounter = 0;
        exitScreensRoutine();
        break;
      case BUTTON_F :
        startTimerSetting = false;
        disableRTCTimer();
        //timeoutCounter = 0;
        exitScreensRoutine();
        break;
    }
    return;
  } else if (timerSettingPart == timerSettingPartM) {
    switch (button)
    {
      case BUTTON_A :
        if (timerSettingM == 59) {
          timerSettingM = 0;
        } else {
          timerSettingM++;
        }
        renderSetTimerDisplay();
        break;
      case BUTTON_B :
        if (timerSettingM == 0) {
          timerSettingM = 59;
        } else {
          timerSettingM--;
        }
        renderSetTimerDisplay();
        break;
      case BUTTON_C :
        startTimerSetting = false;

        saveTimerHMS(hmsToSeconds(timerSettingH, timerSettingM, 0, 1));
        timerOrigH = timerSettingH;
        timerOrigM = timerSettingM;


        timerEnabled = true;
        timerSubMin = false;
        while (!rtc.updateTime());
        lastEpoch = rtc.getUNIX();
        setRTCTimer(60);

        exitScreensRoutine();

        break;
      case BUTTON_D :
        timerSettingPart = timerSettingPartH;
        renderSetTimerDisplay();
        break;
      case BUTTON_F :
        startTimerSetting = false;
        disableRTCTimer();
        exitScreensRoutine();
        break;
    }
    return;
  }
}


void savePomodoroHMS(int32_t t) {
  pausedPomodoroS = t % 60;

  t = (t - pausedPomodoroS) / 60;
  pausedPomodoroM = t % 60;

  t = (t - pausedPomodoroM) / 60;
}

void savePomodoroTimerValues(int M = pomodoroWorkMinues, int S = 0, int decrement = 0) {

  saveTimerHMS(hmsToSeconds(0, M, S, decrement));
  timerOrigH = timerSettingH;
  timerOrigM = timerSettingM;

  timerEnabled = true;
  timerSubMin = false;
  while (!rtc.updateTime());
  lastEpoch = rtc.getUNIX();

  if (S == 0) {
    setRTCTimer(60);
    pomodoroNeedInterrupt = false;
  } else {
    setRTCTimer(1);
    pomodoroNeedInterrupt = true;
  }
}

void incrementPomodoroTimer() {
  pomodoroPaused = false;
  pomodoroSaved = false;
  showPomodoroMenu = true;
  pomodoroEnabled = true;
  pomodoroItem++;

  if (pomodoroItem > 8) {
    pomodoroItem = 1;
  }

  if (pomodoroItem % 2 == 0) { //if even, then break
    if (pomodoroItem == 8) { //long break
      savePomodoroTimerValues(pomodoroLongBreakMinues, 0, 1);
    } else { //short break
      savePomodoroTimerValues(pomodoroShortBreakMinues, 0, 1);
    }
  } else {
    savePomodoroTimerValues(pomodoroWorkMinues, 0, 1);
  }

}

void handlePomodoroDetails(int button) {

  switch (button)
  {
    case BUTTON_C :
      showPomodoroMenu = true;
      pomodoroEnabled = true;

      if (!pomodoroInitiated) {
        pomodoroInitiated = true;

        pomodoroItem = 1;
        savePomodoroTimerValues();

      } else {
        pomodoroPaused = !pomodoroPaused;

        if (pomodoroPaused) {
          pomodoroSaved = false;
        } else {
          savePomodoroTimerValues(timerM, timerS, 1);
          pomodoroSaved = false;
        }

      }

      renderMainDisplay();
      break;
    case BUTTON_D :

      if (pomodoroInitiated) {
        incrementPomodoroTimer();
        renderMainDisplay();
      }

      break;
    case BUTTON_E :

      if (pomodoroInitiated) {
        pomodoroItem = 0;
        startTimerSetting = false;
        pomodoroPaused = false;
        pomodoroSaved = false;
        pomodoroEnabled = false;
        pomodoroInitiated = false;
        disableRTCTimer();
        showPomodoroMenu = true;
        renderMainDisplay();
      }

      break;
    case BUTTON_F :
      showPomodoroMenu = false;
      //exitScreensRoutine(false);
      //note - we can't use "exitScreensRoutine" because it incorrectly changes the menu
      timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP_QUICK;
      renderMainDisplay();
      delay(DEBOUNCE); //adding a second debounce when we go back to the main screen

      break;
  }
  return;
}

void handlePomodoroAlarm() {

  if (pomodoroEnabled) {
    if (pomodoroItem < 8) {
      incrementPomodoroTimer();
      //pomodoroPaused = true;
      //showSeconds = true;
      //showMenu = true;
      //showPomodoroMenu = true;
      //menuStart = 0;
      //timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP_SHORT;
      //renderMainDisplay();
      //timeoutCounter = 0;
    } else {
      startTimerSetting = false;
      pomodoroPaused = false;
      pomodoroSaved = false;
      showPomodoroMenu = false;
      pomodoroEnabled = false;
      pomodoroInitiated = false;
      pomodoroItem = 0;
    }
  }
}



void renderPomodoroInfoDisplay() {

  currentDisplay = CANCEL;
  display.fillScreen(BGCOLOR);
  display.setTextSize(1);

  int x = 10;
  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("SET TIMER", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(x, displayMarginH + h);

  display.print("POMODORO");

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold9pt7b);

  display.setCursor(x, displayMarginH + h * 3);
  display.println("You must cancel the timer first.");

  display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("EXIT");

  display.refresh();
  return;
}
