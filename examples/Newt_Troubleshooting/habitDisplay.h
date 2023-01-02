#define habitTrackerYear 139
#define habitTrackerAddressStart 140
int selectedDate = -1;
int selectedYear = -1;
int selectedMonth = -1;
boolean goForward = false;
boolean goBackward = true;

int dayAddress(unsigned int m, unsigned int d) {
  int dayCount[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}; // This assumes a leap year, which we can skip for non-leap years
  return (dayCount[(m - 1)] + d + (habitTrackerAddressStart - 1));
}

bool isLeapYear(int yr) {
  if (((yr % 4 == 0) && (yr % 100 != 0)) || (yr % 400 == 0))
    return true;
  return false;
}

int getDaysInMonth(int mo, int yr) {
  int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (mo != 2) {
    return daysInMonth[(mo - 1) % 12];
  }
  if (isLeapYear(yr)) {
    return 29;
  }
  return 28;
}

int getDayOfWeek(int yr, int mo, int dy) {
  uint16_t months[] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365
  };   // days until 1st of month

  uint32_t dys = yr * 365;        // days until year
  for (uint16_t i = 4; i < yr; i += 4) if (isLeapYear(i) ) dys++;     // adjust leap years, test only multiple of 4 of course

  dys += months[mo - 1] + dy;  // add the days of this year
  if ((mo > 2) && isLeapYear(yr)) dys++;  // adjust 1 if this year is a leap year, but only after febr

  // make Sunday 0
  dys--;
  if (dys < 0)
    dys += 7;
  return dys % 7;   // remove all multiples of 7
}

void clearChecks() {
  int endAddress = 366 + habitTrackerAddressStart + 1;
  for (int i = habitTrackerAddressStart; i < endAddress; i++) {
    EEPROM.write(i, 0);
  }
  EEPROM.commit();
}

void renderHabitDisplay() {

  currentDisplay = HABIT;
  display.fillScreen(BGCOLOR);
  display.setTextSize(1);

  while (!rtc.updateTime());

  int currMonth = rtc.getMonth();
  int currYear = rtc.getYear();
  int currDate = rtc.getDate();

  /*if (touchpad3hanged) {
    selectedDate = currDate;
    selectedYear = currYear;
    selectedMonth = currMonth;
    }*/

  if (selectedDate == -1) {
    selectedDate = currDate;
    selectedYear = currYear;
    selectedMonth = currMonth;
  }


  int habitYear = currYear - 2000;

  if (habitYear != EEPROM.read(habitTrackerYear)) {
    clearChecks();
    EEPROM.write(habitTrackerYear, habitYear);
  }

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold9pt7b);

  if ((selectedDate == 1) && (selectedMonth == 1)) {
    goBackward = false;
  } else {
    goBackward = true;

    display.getTextBounds("BACK", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("BACK");
  }

  if ((selectedDate == currDate) && (selectedMonth == currMonth)) {
    goForward = false;
  } else {
    goForward = true;
    display.getTextBounds("NEXT", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_D_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("NEXT");
  }

  int checked = EEPROM.read(dayAddress(selectedMonth, selectedDate));

  if (checked == 1) {
    display.getTextBounds("CLEAR", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_E_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("CLEAR");
  } else {
    display.getTextBounds("CHECK", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_E_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("CHECK");
  }

  display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("EXIT");

  char daysOfTheWeek[7][2] = {"S", "M", "T", "W", "T", "F", "S"};
  char monthsOfTheYear[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
  char monthsOfTheYearCAP[12][4] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

  char yearChar[5];
  itoa(selectedYear, yearChar, 10);

  int daysInMonth = getDaysInMonth(selectedMonth, selectedYear);
  int startDay = getDayOfWeek(selectedYear, selectedMonth, 1);
  int blockDay = 0;

  int xCalName = 60;
  int yCalName = 40;

  char monthChar[10];
  strcpy(monthChar, monthsOfTheYearCAP[selectedMonth - 1]);
  strcat(monthChar, " ");
  strcat(monthChar, yearChar);

  display.setFont(&FreeSansBold12pt7b);

  for (int j = 0; j < 8; j++) {
    display.getTextBounds("X", 0, 0, &xP, &yP, &w, &h);
    display.setCursor((xCalName - w) / 2, yCalName);
    display.print(monthChar[j]);

    yCalName += h + 4;
  }

  int xTab = 30;
  int yTab = 30;
  int rectR = 3;
  int circleR = 14;

  int xStart = displayWidth / 2 - (xTab * 3.5) + 10;
  int yStart = 20;
  int xPos = xStart;
  int yPos = yStart;
  int x;
  int y;

  display.setFont(&FreeSansBold9pt7b);
  display.getTextBounds("M", 0, 0, &xP, &yP, &w, &h);

  for (int d = 0; d < 7; d++) {
    x = xPos - w / 2;
    y = yPos + h / 2;
    display.setCursor(x - 2, y);
    display.print(daysOfTheWeek[d]);
    xPos += xTab;
  }

  xPos = xStart;
  yPos += yTab;

  while (blockDay != startDay) {
    xPos += xTab;
    blockDay++;
  }

  for (int i = 1; i < daysInMonth + 1; i++) {

    int r = rectR;
    if (i == selectedDate) {
      display.setFont(&FreeSansBold9pt7b);
      r = xTab / 2;
    } else {
      display.setFont(&FreeSans9pt7b);
    }

    char iDayChar[3];
    itoa(i, iDayChar, 10);
    display.getTextBounds(iDayChar, 0, 0, &xP, &yP, &w, &h);

    if ((blockDay % 7 == 0) && (i != 1)) {
      yPos += yTab;
      xPos = xStart;
    }

    x = xPos - w / 2;
    y = yPos + h / 2;
    display.setCursor(x - 2, y);

    int selected = EEPROM.read(dayAddress(selectedMonth, i));

    if (selected == 1) {
      display.fillRoundRect(xPos - xTab / 2, yPos - yTab / 2, xTab - 1, yTab - 1, r, TEXTCOLOR);
      display.setTextColor(BGCOLOR);
    } else {
      display.drawRoundRect(xPos - xTab / 2, yPos - yTab / 2, xTab - 1, yTab - 1, r, TEXTCOLOR);
      display.setTextColor(TEXTCOLOR);
    }

    display.print(i);
    xPos += xTab;
    blockDay++;
  }

  display.setTextColor(TEXTCOLOR);
  display.refresh();
}

void dateIncOrDec(boolean inc) {

  while (!rtc.updateTime());

  int currMonth = rtc.getMonth();
  int currYear = rtc.getYear();
  int currDate = rtc.getDate();

  if (!inc) {
    if (selectedDate == 1) {
      selectedMonth--;
      if (selectedMonth == 0) {
        selectedMonth = 12;
        selectedYear--;
      }
      selectedDate = getDaysInMonth(selectedMonth, selectedYear);
    } else {
      selectedDate--;
    }
  } else {
    if (selectedDate == getDaysInMonth(selectedMonth, selectedYear)) {
      selectedDate = 1;
      selectedMonth++;
      if (selectedMonth == 13) {
        selectedMonth = 1;
        selectedYear++;
      }
    } else {
      selectedDate++;
    }
  }
  renderHabitDisplay();

}

void recordHabit() {
  int int_adddress = dayAddress(selectedMonth, selectedDate);

  int val = EEPROM.read(int_adddress);

  if (val != 1) {
    EEPROM.write(int_adddress, 1);
  } else {
    EEPROM.write(int_adddress, 0);
  }
  EEPROM.commit();
  renderHabitDisplay();
}

void handleHabitCalendar(int button) {

  switch (button)
  {
    case BUTTON_C :
      if (goBackward) {
        dateIncOrDec(false);
      }
      break;
    case BUTTON_D :
      if (goForward) {
        dateIncOrDec(true);
      }
      break;
    case BUTTON_E :
      recordHabit();
      break;
    case BUTTON_F :
      //timeoutCounter = 0;
      exitScreensRoutine();
      //renderMainDisplay();
      break;
  }
  return;
}
