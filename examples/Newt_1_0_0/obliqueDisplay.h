
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
