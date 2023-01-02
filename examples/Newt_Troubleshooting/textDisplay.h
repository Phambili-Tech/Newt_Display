
void renderObliqueDisplay() {
  currentDisplay = OBLIQUE;
  display.fillScreen(BGCOLOR);

  int xStart = 10;
  int yStart = 40;
  int displayValues;  //= 5;
  int xTab;
  int x = xStart;
  int y = yStart;

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("OBLIQUE STRATEGIES", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(x, displayMarginH + h);
  display.print("OBLIQUE STRATEGIES");

  display.setFont(&FreeSansBoldOblique12pt7b);
  display.getTextBounds("X", 0, 0, &xP, &yP, &w, &h);

  int lineH = h + 5;
  int quoteSpacer = 7;
  int obliqueY = 100;
  display.setCursor(displayMarginW, obliqueY);

  char oblique[1200];
  strcpy(oblique, currOblique);

  char delim[] = " ";
  int lineLimit = displayWidth - (displayMarginW * 4.5); //note: we are ok going a little past the self-imposed margins
  int lineLength = 0;
  int lineCount = 1;

  char *ptr = strtok(oblique, delim);

  while (ptr != NULL)
  {
    //printf("'%s'\n", ptr);
    display.getTextBounds(ptr, 0, 0, &xP, &yP, &w, &h);
    //Serial.println(w);

    if ((lineLength + w) > lineLimit) {
      lineLength = w;
      obliqueY = obliqueY + lineH;
      display.setCursor(displayMarginW, obliqueY);
      lineCount++;
    } else {
      lineLength = lineLength + w;
    }
    display.print(ptr);
    display.print(" ");

    ptr = strtok(NULL, delim);

  }

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold9pt7b);

  display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("EXIT");

  display.refresh();

}

void handleObliqueDisplay(int button) {

  switch (button)
  {
    case BUTTON_C :
      break;
    case BUTTON_D :
      break;
    case BUTTON_E :
      break;
    case BUTTON_F :
      exitScreensRoutine();
      break;
  }
  return;
}

void renderQuoteDisplay() {
  currentDisplay = QUOTE;
  display.fillScreen(BGCOLOR);

  int xStart = 10;
  int yStart = 40;
  int displayValues;  //= 5;
  int xTab;
  int x = xStart;
  int y = yStart;

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("QUOTE", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(x, displayMarginH + h);
  display.print("QUOTE");

  display.setFont(&FreeSansBoldOblique12pt7b);
  display.getTextBounds("X", 0, 0, &xP, &yP, &w, &h);

  int quoteY = 60;

  renderQuote(quoteY);

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold9pt7b);

  display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("EXIT");

  display.refresh();

}

void handleQuoteDisplay(int button) {

  switch (button)
  {
    case BUTTON_C :
      break;
    case BUTTON_D :
      break;
    case BUTTON_E :
      break;
    case BUTTON_F :
      exitScreensRoutine();
      break;
  }
  return;
}


void renderAlertsDisplay() {
  currentDisplay = ALERTS;
  display.fillScreen(BGCOLOR);

  int xStart = 10;
  int yStart = 40;
  int displayValues;  //= 5;
  int xTab;
  int x = xStart;
  int y = yStart;

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("ALERTS (BETA)", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(x, displayMarginH + h);
  display.print("ALERTS (BETA)");

  display.setFont(&FreeSansBoldOblique12pt7b);
  display.getTextBounds("X", 0, 0, &xP, &yP, &w, &h);


  char alert[1200];
  if (wifiFailed) {

    strcpy(alert, "Newt was unable to connect to WiFi. It will try again in less than 30 mins.");

  } else {

    strcpy(alert, "There are no alerts at this time.");
  }

  int lineH = h + 5;
  int textY = 100;
  display.setCursor(displayMarginW, textY);


  char delim[] = " ";
  int lineLimit = displayWidth - (displayMarginW * 6); //note: we are ok going a little past the self-imposed margins
  int lineLength = 0;
  int lineCount = 1;

  char *ptr = strtok(alert, delim);

  while (ptr != NULL)
  {
    //printf("'%s'\n", ptr);
    display.getTextBounds(ptr, 0, 0, &xP, &yP, &w, &h);
    //Serial.println(w);

    if ((lineLength + w) > lineLimit) {
      lineLength = w;
      textY = textY + lineH;
      display.setCursor(displayMarginW, textY);
      lineCount++;
    } else {
      lineLength = lineLength + w;
    }
    display.print(ptr);
    display.print(" ");

    ptr = strtok(NULL, delim);

  }

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold9pt7b);

  display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("EXIT");

  display.refresh();

}

void handleAlertsDisplay(int button) {

  switch (button)
  {
    case BUTTON_C :
      break;
    case BUTTON_D :
      break;
    case BUTTON_E :
      break;
    case BUTTON_F :
      exitScreensRoutine();
      break;
  }
  return;
}


void renderRiddle(int startY, int option) {

  display.setFont(&FreeSansBold9pt7b);

  display.getTextBounds("X", 0, 0, &xP, &yP, &w, &h);

  int lineH = h + 5;
  display.setCursor(displayMarginW, startY);

  char text[300];

  switch (option)
  {
    case 0 :
      strcpy(text, dailyRiddle);
      break;
    case 1 :
      strcpy(text, dailyRiddleAnswer);
      break;
    case 2 :
      strcpy(text, dailyRiddleAttr);
      break;
  }


  char delim[] = " ";
  int lineLimit = displayWidth - (displayMarginW * 6); //note: we are ok going a little past the self-imposed margins
  int lineLength = 0;
  int lineCount = 1;

  char *ptr = strtok(text, delim);

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
}

void renderRiddleDisplay(int option = 0) {
  currentDisplay = RIDDLE;
  display.fillScreen(BGCOLOR);

  int xStart = 10;
  int yStart = 40;
  int displayValues;  //= 5;
  int xTab;
  int x = xStart;
  int y = yStart;

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("RIDDLE (BETA)", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(x, displayMarginH + h);
  display.print("RIDDLE (BETA)");

  display.setFont(&FreeSansBoldOblique12pt7b);
  display.getTextBounds("X", 0, 0, &xP, &yP, &w, &h);

  int riddleY = 60;

  renderRiddle(riddleY, option);

  if (option == 0) {
    timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP;
  }


  if (riddleCategoryNo == 1 && option == 0) { //add note about daily puzzle
    display.setFont(&FreeSans9pt7b);
    display.getTextBounds("Each clue describes a famous pair (ex: Yin & Yang)", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y - (h * 2));
  }


  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold9pt7b);


  display.getTextBounds("RIDDLE", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("RIDDLE");

  display.getTextBounds("ANSWER", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_D_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("ANSWER");

  display.getTextBounds("INFO", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_E_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("INFO");

  display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("EXIT");

  display.refresh();

}

void handleRiddleDisplay(int button) {

  switch (button)
  {
    case BUTTON_C :
      renderRiddleDisplay(0);
      break;
    case BUTTON_D :
      renderRiddleDisplay(1);
      break;
    case BUTTON_E :
      renderRiddleDisplay(2);
      break;
    case BUTTON_F :
      exitScreensRoutine();
      break;
  }
  return;
}
