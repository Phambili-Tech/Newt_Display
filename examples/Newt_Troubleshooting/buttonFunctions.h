#define TOUCH_CHANGE_CONFIG 0

const uint32_t touch_value_threshold_min = 16000;
uint32_t touch_value_threshold_max = 300000;

const uint32_t touch_value_threshold_min_F = 16000; //9000;
uint32_t touch_value_threshold_max_F = 300000; //70000;

struct Button {
  const uint8_t PIN;
  uint32_t numberKeyPresses;
  bool pressed;
  uint32_t thresholdVal;
};

boolean rtcIrq = false;
boolean usbIrq = false;

Button buttonA = {BUTTON_A, 0, false, 15000};
Button buttonB = {BUTTON_B, 0, false, 14000};
Button buttonC = {BUTTON_C, 0, false, 13000};
Button buttonD = {BUTTON_D, 0, false, 12000};
Button buttonE = {BUTTON_E, 0, false, 12000};
Button buttonF = {BUTTON_F, 0, false, 10000};

Button padONE =   {PAD_ONE, 0, false, 10000};
Button padTWO =   {PAD_TWO, 0, false, 10000};
Button padTHREE = {PAD_THREE, 0, false, 10000};
Button padFOUR =  {PAD_FOUR, 0, false, 11000};

int selectedTouchpad = 0;
boolean padPressed = false;

void IRAM_ATTR RTC_isr() {
  rtcIrq = true;
}

void IRAM_ATTR USB_isr() {
  usbIrq = true;
}

void beep() {
  digitalWrite(SPKR_CTRL, HIGH);
  ledcWriteTone(BUZZER_CHANNEL, BEEP_FREQUENCY);
}

void beepOff() {
  ledcWriteTone(BUZZER_CHANNEL, 0);
  digitalWrite(SPKR_CTRL, LOW);
}

void setupTouchPadForSleep(touch_pad_t tPad) {

  touch_pad_init();
  /* Only support one touch channel in sleep mode. */
  touch_pad_config(tPad);
  /* Filter setting */
  touch_filter_config_t filter_info = {
    .mode = TOUCH_PAD_FILTER_IIR_16,
    .debounce_cnt = 1,      // 1 time count.
    .noise_thr = 0,         // 50%
    .jitter_step = 4,       // use for jitter mode.
    .smh_lvl = TOUCH_PAD_SMOOTH_IIR_2,
  };
  touch_pad_filter_set_config(&filter_info);
  touch_pad_filter_enable();
  printf("touch pad filter init %d\n", TOUCH_PAD_FILTER_IIR_8);
  /* Set sleep touch pad. */
  touch_pad_sleep_channel_enable(tPad, true);
  touch_pad_sleep_channel_enable_proximity(tPad, false);
  /* Reducing the operating frequency can effectively reduce power consumption. */
  touch_pad_sleep_channel_set_work_time(10000, TOUCH_PAD_MEASURE_CYCLE_DEFAULT); //1000 //5000 works /10000 works
  /* Enable touch sensor clock. Work mode is "timer trigger". */
  touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
  touch_pad_fsm_start();
  vTaskDelay(100 / portTICK_RATE_MS);


  /* set touchpad wakeup threshold */
  //this whole section is original and working
  uint32_t touch_value, wake_threshold;
  touch_pad_sleep_channel_read_smooth(tPad, &touch_value); //this was working
  wake_threshold = touch_value * 0.1; // wakeup when touch sensor crosses 10% of background level
  touch_pad_sleep_set_threshold(tPad, wake_threshold);

  /*uint32_t wake_threshold;
    touch_pad_sleep_channel_read_smooth(tPad, &touch_value); //this was working
    wake_threshold = touch_value * 0.1; // wakeup when touch sensor crosses 10% of background level
    touch_pad_sleep_set_threshold(tPad, wake_threshold);*/
}

void setupTouchPadWhileAwake() {
  /* Initialize touch pad peripheral. */
  touch_pad_init();
  for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
    touch_pad_config(button[i]);
  }


  /* Enable touch sensor clock. Work mode is "timer trigger". */
  touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
  touch_pad_fsm_start();

}

static void touchpad_check_value(void *pvParameter)
{
  uint32_t touch_value;

  /* Wait touch sensor init done */
  vTaskDelay(100 / portTICK_RATE_MS);

  while (1) {
    for (int i = 0; i < TOUCH_BUTTON_NUM; i++) {
      touch_pad_read_raw_data(button[i], &touch_value);    // read raw data.
      // if ( ((touch_value > touch_value_threshold_min) && (touch_value < touch_value_threshold_max) && i != 5) ||
      //      ((touch_value > touch_value_threshold_min_F) && (touch_value < touch_value_threshold_max_F) && i == 5)) {
      //printf("T%d: [%4d] ", button[i], touch_value);
      //printf("\n");
      switch (button[i]) {
        case BUTTON_A_TOUCH_NAME:
          if (touch_value > buttonA.thresholdVal) {
            buttonA.pressed = true;
            padPressed = true;
            selectedTouchpad = BUTTON_A;
          }
          break;
        case BUTTON_B_TOUCH_NAME:
          if (touch_value > buttonB.thresholdVal) {
            buttonB.pressed = true;
            padPressed = true;
            selectedTouchpad = BUTTON_B;
          }
          break;
        case BUTTON_C_TOUCH_NAME:
          if (touch_value > buttonC.thresholdVal) {
            buttonC.pressed = true;
            padPressed = true;
            selectedTouchpad = BUTTON_C;
          }
          break;
        case BUTTON_D_TOUCH_NAME:
          if (touch_value > buttonD.thresholdVal) {
            buttonD.pressed = true;
            padPressed = true;
            selectedTouchpad = BUTTON_D;
          }
          break;
        case BUTTON_E_TOUCH_NAME:
          if (touch_value > buttonE.thresholdVal) {
            buttonE.pressed = true;
            padPressed = true;
            selectedTouchpad = BUTTON_E;
          }
          break;
        case BUTTON_F_TOUCH_NAME:
          if (touch_value > buttonF.thresholdVal) {
            buttonF.pressed = true;
            padPressed = true;
            selectedTouchpad = BUTTON_F;
          }
          break;
        case PAD_ONE_TOUCH_NAME:
          if (touch_value > padONE.thresholdVal) {
            padONE.pressed = true;
            padPressed = true;
            selectedTouchpad = PAD_ONE;
          }
          break;
        case PAD_TWO_TOUCH_NAME:
          if (touch_value > padTWO.thresholdVal) {
            padTWO.pressed = true;
          }
          break;
        case PAD_THREE_TOUCH_NAME:
          if (touch_value > padTHREE.thresholdVal) {
            padTHREE.pressed = true;
          }
          break;
        case PAD_FOUR_TOUCH_NAME:
          if (touch_value > padFOUR.thresholdVal) {
            padFOUR.pressed = true;
          }
          break;
        default:
          break;
      }
      //}
    }
    vTaskDelay(200 / portTICK_PERIOD_MS);
  }
}

void IRAM_ATTR isr(void* arg) {
  Button* s = static_cast<Button*>(arg);
  s->numberKeyPresses += 1;
  s->pressed = true;
}

void selectFunction() {

  if (selectedTouchpad == PAD_ONE) {

    if (alarmActive || timerActive) {//the user is turning off the buzzer
      timeoutCounter = 0;
      if (timerActive) {
        disableRTCTimer();
        handlePomodoroAlarm();
      }
      if (alarmActive) {
        disableRTCAlarm();
      }
      return;
    }

    showMenu = true; //we need this in case we are attempting to change the menu while in sync

    if (currentDisplay == HOME) {//change menu on main display
      Serial.println("change menu");
      renderMainDisplay();
    } else { //return to main display from another view
      keepMenu();
    }
    timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP_SHORT;
    return;
  }

  //pad one not pressed, so check other touchpads
  for (int j = 0; j < numOfMenuItems; j++) {

    if (currentDisplay == HOME && !showPomodoroMenu) {
      if (selectedTouchpad == menuIconArray[j].assignedTouchpad) {
        switch (menuIconArray[j].functionNumber) {
          case ALARM :
            renderSetAlarmDisplay();
            break;
          case TIMER :
            renderSetTimerDisplay();
            break;
          case WEATHER :
            renderWeatherDisplay();
            break;
          case HABIT :
            renderHabitDisplay();
            break;
          case TODO :
            renderTodoDisplay(true); //need to sync todo list
            break;
          case SETTINGS :
            renderSettingsDisplay();
            break;
          case SETTINGSINFO :
            renderSettingsInfoDisplay();
            break;
          case AIRQUALITY :
            renderAQDisplay();
            break;
          case UV :
            renderUVDisplay();
            break;
          case OBLIQUE :
            renderObliqueDisplay();
            break;
          case QUOTE :
            renderQuoteDisplay();
            break;
          case RIDDLE :
            renderRiddleDisplay();
            break;
          case ALERTS :
            renderAlertsDisplay();
            break;
          case POMODORO :
            if (timerEnabled && !pomodoroEnabled) {
              renderPomodoroInfoDisplay();
            } else {
              showPomodoroMenu = true;
              renderMainDisplay();
            }
            break;
          default : Serial.println("No valid button selection"); break;
        }
        timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP_SHORT;
        delay(DEBOUNCE); //adding a second debounce when we to a function screen
        return;
      }
    } else {
      switch (currentDisplay)
      {
        case ALARM :
          incrementAlarmDisplay(selectedTouchpad);
          break;
        case TIMER :
          incrementTimerDisplay(selectedTouchpad);
          break;
        case WEATHER :
          toggleWeatherDisplay(selectedTouchpad);
          break;
        case HABIT :
          handleHabitCalendar(selectedTouchpad);
          break;
        case TODO :
          handleTodoList(selectedTouchpad);
          break;
        case SETTINGS :
          handleSettingsDetails(selectedTouchpad);
          break;
        case SETTINGSINFO :
          handleSettingsInfoDetails(selectedTouchpad);
          break;
        case AIRQUALITY :
          handleAQDisplay(selectedTouchpad);
          break;
        case UV :
          handleUVDisplay(selectedTouchpad);
          break;
        case OBLIQUE :
          handleObliqueDisplay(selectedTouchpad);
          break;
        case QUOTE :
          handleQuoteDisplay(selectedTouchpad);
          break;
        case RIDDLE :
          handleRiddleDisplay(selectedTouchpad);
          break;
        case ALERTS :
          handleAlertsDisplay(selectedTouchpad);
          break;
        case CANCEL :
          handleObliqueDisplay(selectedTouchpad); //can be used for an exit/cancel
          break;
        case HOME :
          if (showPomodoroMenu) {
            handlePomodoroDetails(selectedTouchpad);
            delay(DEBOUNCE);
          }
          break;
        default : Serial.println("No valid button selection"); break;
      }
      timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP_SHORT;
      return;
    }
  }
}
