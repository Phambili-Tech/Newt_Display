#define TOUCH_CHANGE_CONFIG 0

const uint32_t touch_value_threshold_min = 16000;
uint32_t touch_value_threshold_max = 300000;

const uint32_t touch_value_threshold_min_F = 16000;
uint32_t touch_value_threshold_max_F = 300000;

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
            padPressed = true;
            selectedTouchpad = PAD_TWO;
          }
          break;
        case PAD_THREE_TOUCH_NAME:
          if (touch_value > padTHREE.thresholdVal) {
            padTHREE.pressed = true;
            padPressed = true;
            selectedTouchpad = PAD_THREE;
          }
          break;
        case PAD_FOUR_TOUCH_NAME:
          if (touch_value > padFOUR.thresholdVal) {
            padFOUR.pressed = true;
            padPressed = true;
            selectedTouchpad = PAD_FOUR;
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
