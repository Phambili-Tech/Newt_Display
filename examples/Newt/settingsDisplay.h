int selectedSettingIcon = 0;
int selectedSettingFunctionNumber;
int headerXStart;

#define UPGRADE_SETTING 901
//#define DEVICEDETAILS_SETTING 902
#define TIMEZONE_SETTING 903
#define LOCATION_SETTING 904
#define TIMEDISPLAY_SETTING 905
#define UNIT_SETTING 906
#define DARKMODE_SETTING 907
#define DEFAULTSCREEN_SETTING 908
#define RESETWIFI_SETTING 909
#define CLEARHABITS_SETTING 910
#define REINSTALL_SETTING 911
#define FACTORYRESET_SETTING 912
#define CONTACT_SETTING 9001
#define ATTRIBUTION_SETTING 9002
#define RELEASENOTES_SETTING 9003
#define DEVICEDETAILS_SETTING 9004
#define NEXT_SETTING 997
#define BACK_SETTING 998
#define SETTINGSDUMMY 999


struct SETTINGS_ICON {
  char displayName[20];
  char displayIcon[2];
  uint32_t functionNumber;
};

const byte numOfSettingsItems = 14;
SETTINGS_ICON settingsIconArray[numOfSettingsItems] = {
  {"INFO", infoG, DEVICEDETAILS_SETTING},
  {"WHAT'S NEW", paperG, RELEASENOTES_SETTING},
  {"LOCATION", locationG, LOCATION_SETTING},
  {"TIMEZONE", timezoneG, TIMEZONE_SETTING},
  {"TIME FORMAT", digiclockG, TIMEDISPLAY_SETTING},
  {"UNITS", unitsG, UNIT_SETTING},
  {"SCREEN MODE", darkmodeG, DARKMODE_SETTING},
  {"HOME SCREEN", displayG, DEFAULTSCREEN_SETTING},
  {"CLEAR HABITS", clearcalendarG, CLEARHABITS_SETTING},
  {"RESET WIFI", resetwifiG, RESETWIFI_SETTING},
  {"CONTACT INFO", phoneG, CONTACT_SETTING},
  {"ATTRIBUTION", attributeG, ATTRIBUTION_SETTING},
  {"FULL RESET", warningG, FACTORYRESET_SETTING},
  {"REINSTALL", downloadG, REINSTALL_SETTING}
};

void renderSetting () {
  int startX = displayMarginW / 2;
  int xScreenDetailsStart = startX;
  int yScreenDetailsStart = 55;
  int yScreenDetailsStartRowTwoTab = 17;
  char buttonLabel[10];
  char settingValue[120];
  char settingAddl[120];

  strcpy(settingValue, "Setting Value: ");
  strcpy(settingAddl, "");

  switch (selectedSettingFunctionNumber) {

    case DEVICEDETAILS_SETTING:
      strcpy(settingValue, "Select for device details");
      strcpy(buttonLabel, "SEE INFO");

      char availpatchLevel[10];
      sprintf(availpatchLevel, " %d.%d.%d", latest_ver_major, latest_ver_minor, latest_ver_patch);

      display.setTextColor(TEXTCOLOR);

      display.setFont(&FreeSansBold9pt7b);

      if (versionUpdateAvailable) {
        //strcpy(buttonLabel, "UPGRADE");
        strcpy(settingAddl, "New Version Available - ");
        strcat(settingAddl, availpatchLevel);
      }

      break;

    /*case DEVICEDETAILS_SETTING:

      strcpy(buttonLabel, "");

      strcpy(settingValue, "ID: ");
      strcat(settingValue, uniqId);
      strcat(settingValue, "   Ver: ");

      char patchLevel[16];

      if (!RC) {
        sprintf(patchLevel, " %d.%d.%d", SWVERSION_MAJOR, SWVERSION_MINOR, SWVERSION_PATCH);
      } else {
        sprintf(patchLevel, " %d.%d.%d-RC%d", SWVERSION_MAJOR, SWVERSION_MINOR, SWVERSION_PATCH, SWVERSION_RC);
      }

      strcat(settingValue, patchLevel);

      char availpatchLevel[10];
      sprintf(availpatchLevel, " %d.%d.%d", latest_ver_major, latest_ver_minor, latest_ver_patch);

      display.setTextColor(TEXTCOLOR);

      display.setFont(&FreeSansBold9pt7b);

      if (versionUpdateAvailable) {
        strcpy(buttonLabel, "UPGRADE");
        strcpy(settingAddl, "New Version Available - ");
        strcat(settingAddl, availpatchLevel);
      }

      break;*/

    case TIMEZONE_SETTING:

      strcpy(buttonLabel, "");

      strcpy(settingValue, "Timezone: ");
      strcat(settingValue, timezone);

      break;

    case LOCATION_SETTING:

      strcpy(buttonLabel, "");

      strcpy(settingValue, "Location: ");
      strcat(settingValue, city);
      strcat(settingValue, " - ");
      strcat(settingValue, locale);

      break;

    case TIMEDISPLAY_SETTING:
      strcpy(buttonLabel, "CHANGE");

      if (isTwentyFourHour()) {
        strcat(settingValue, "24 HOUR");
      } else {
        strcat(settingValue, "12 HOUR");
      }
      break;

    case UNIT_SETTING:
      strcpy(buttonLabel, "CHANGE");

      switch (getSystemMeasure()) {
        case IMPERIALMEASURE:
          strcat(settingValue, "IMPERIAL");
          break;
        case METRICMEASURE:
          strcat(settingValue, "METRIC");
          break;
        case AUTOMEASURE:
          strcat(settingValue, "AUTOMATIC");
          strcpy(settingAddl, "Set based on location");
          break;
      }
      break;

    case DARKMODE_SETTING:
      strcpy(buttonLabel, "CHANGE");

      switch (getLightMode()) {
        case AUTOMODE:
          strcat(settingValue, "AUTO MODE");
          strcpy(settingAddl, "Dark Mode between 11PM - 5AM");
          break;
        case LIGHTMODE :
          strcat(settingValue, "LIGHT MODE");
          break;
        case DARKMODE :
          strcat(settingValue, "DARK MODE");
          break;
      }
      break;

    case DEFAULTSCREEN_SETTING:
      strcpy(buttonLabel, "CHANGE");

      switch (getDefaultScreen()) {
        case QUOTESCREEN:
          strcat(settingValue, "QUOTES");
          break;
        case WEATHERSCREEN:
          strcat(settingValue, "WEATHER");
          break;
        case AUTOSCREEN:
          strcat(settingValue, "ROTATES");
          strcpy(settingAddl, "Changes every 3 min");
          break;
      }
      break;

    case CLEARHABITS_SETTING:
      strcpy(settingValue, "Clear the Habit Calendar");
      strcpy(buttonLabel, "CLEAR");
      break;

    case RESETWIFI_SETTING:
      strcpy(settingValue, "Clear saved values and ask for new info");
      strcpy(buttonLabel, "RESET");
      break;

    case FACTORYRESET_SETTING:
      strcpy(settingValue, "Restore Newt to factory settings");
      strcpy(buttonLabel, "ERASE");
      break;

    case ATTRIBUTION_SETTING:
      strcpy(settingValue, "List of contributing libraries");
      strcpy(buttonLabel, "SEE LIST");
      break;

    case RELEASENOTES_SETTING:
      strcpy(settingValue, "See the release notes for this version");
      strcpy(buttonLabel, "SEE LIST");
      break;

    case CONTACT_SETTING:
      strcpy(settingValue, "Select for website and email");
      strcpy(buttonLabel, "SEE INFO");
      break;

    case REINSTALL_SETTING:
      strcpy(settingValue, "Force download of the latest firmware");
      strcpy(buttonLabel, "DOWNLOAD");
      break;

    default:
      // statements
      break;
  }

  display.setTextColor(TEXTCOLOR);

  display.setFont(&FreeSansBold9pt7b);
  display.getTextBounds(settingValue, 0, 0, &xP, &yP, &w, &h);

  display.setCursor(xScreenDetailsStart, yScreenDetailsStart);
  display.print(settingValue);

  display.setCursor(xScreenDetailsStart, yScreenDetailsStart + yScreenDetailsStartRowTwoTab);
  display.print(settingAddl);

  display.setFont(&FreeSansBold9pt7b);
  display.getTextBounds(buttonLabel, 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_E_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print(buttonLabel);

}


void renderSettingsInfoDisplay(int msg = 0) {

  currentDisplay = SETTINGSINFO;

  display.fillScreen(BGCOLOR);
  display.setTextColor(TEXTCOLOR);
  int startX = displayMarginW / 2;
  int startY = displayMarginH * 2 + 10;

  display.setCursor(startX, startY);
  display.setFont(&FreeSansBold12pt7b);

  if (msg == 0) {
    renderSettingsInfoDisplay();
    return;
  }


  if (msg == ATTRIBUTION_SETTING) {

    display.print("ATTRIBUTION");

    startY = startY + 10;

    for (int i = 0; i < attributionItems; i++) {
      startY = startY + 20;
      display.setFont(&FreeSansBold9pt7b);
      display.setCursor(startX, startY);
      display.print(entities[i]);
      display.print(": ");
      display.setFont(&FreeSans9pt7b);
      display.print(contributions[i]);

    }

  }

  if (msg == CONTACT_SETTING) {

    display.print("CONTACT INFO");

    int spacer = 20;
    int tab = 110;

    startY += 30;

    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(startX, startY);
    display.print("WEBSITE");
    display.print(": ");
    display.setCursor(startX + tab, startY);
    display.setFont(&FreeSansBold9pt7b);
    display.print("https://phambili.tech/newt");

    startY += spacer;

    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(startX, startY);
    display.print("EMAIL");
    display.print(": ");
    display.setCursor(startX + tab, startY);
    display.setFont(&FreeSansBold9pt7b);
    display.print("info AT phambili.tech");

    startY += spacer;

  }

  if (msg == DEVICEDETAILS_SETTING) {

    char patchLevel[16];

    if (!RC) {
      sprintf(patchLevel, " %d.%d.%d", SWVERSION_MAJOR, SWVERSION_MINOR, SWVERSION_PATCH);
    } else {
      sprintf(patchLevel, " %d.%d.%d-RC%d", SWVERSION_MAJOR, SWVERSION_MINOR, SWVERSION_PATCH, SWVERSION_RC);
    }

    char availpatchLevel[10];
    sprintf(availpatchLevel, " %d.%d.%d", latest_ver_major, latest_ver_minor, latest_ver_patch);

    display.print("DEVICE INFO");

    int spacer = 20;
    int tab = 190;

    startY += 30;

    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(startX, startY);
    display.print("Device ID");
    display.print(": ");
    display.setCursor(startX + tab, startY);
    display.setFont(&FreeSansBold9pt7b);
    display.print(uniqId);

    startY += spacer;

    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(startX, startY);
    display.print("Version");
    display.print(": ");
    display.setCursor(startX + tab, startY);
    display.setFont(&FreeSansBold9pt7b);
    display.print(patchLevel);

    startY += spacer;

    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(startX, startY);
    display.print("Available Version");
    display.print(": ");
    display.setCursor(startX + tab, startY);
    display.setFont(&FreeSansBold9pt7b);
    display.print(availpatchLevel);

    startY += spacer;

    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(startX, startY);
    display.print("Network ID");
    display.print(": ");
    display.setCursor(startX + tab, startY);
    display.setFont(&FreeSansBold9pt7b);
    display.print(savedSSID);

    startY += spacer;

    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(startX, startY);
    display.print("IP Address");
    display.print(": ");
    display.setCursor(startX + tab, startY);
    display.setFont(&FreeSansBold9pt7b);
    display.print(savedIP);

  }

  if (msg == RELEASENOTES_SETTING) {
    display.print("RELEASE NOTES");

    int spacer = 20;
    int indent = 30;

    startY += 30;

    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(startX, startY);
    display.print("1. Updated Icons");

    startY += spacer;

    display.setFont(&FreeSansBold9pt7b);
    display.setCursor(startX, startY);
    display.print("2. New Settings Options & Info:");

    startY += spacer;

    display.setFont(&FreeSans9pt7b);
    display.setCursor(startX + indent, startY);
    display.print("24 or 12 hour time");

    startY += spacer;

    display.setFont(&FreeSans9pt7b);
    display.setCursor(startX + indent, startY);
    display.print("Dark/Light and Auto Screen Mode");

    startY += spacer;

    display.setFont(&FreeSans9pt7b);
    display.setCursor(startX + indent, startY);
    display.print("Default Quote or Weather Main Display");

    startY += spacer;

    display.setFont(&FreeSans9pt7b);
    display.setCursor(startX + indent, startY);
    display.print("Imperial or Metric units");

    startY += spacer;

    display.setFont(&FreeSans9pt7b);
    display.setCursor(startX + indent, startY);
    display.print("Factory Reset");

    startY += spacer;

    display.setFont(&FreeSans9pt7b);
    display.setCursor(startX + indent, startY);
    display.print("Release Notes");

    startY += spacer;


  }


  int minorSpacer = 4;
  display.setTextSize(1);
  display.setFont(&FreeSansBold9pt7b);


  display.getTextBounds("MORE", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_D_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("");


  display.getTextBounds("BACK", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("BACK");

  display.refresh();

}


void renderSettingsDisplay(int msg = 0) {

  currentDisplay = SETTINGS;

  display.fillScreen(BGCOLOR);
  display.setTextColor(TEXTCOLOR);


  if (msg == 1) {

    display.setFont(&FreeSansBold12pt7b);
    display.getTextBounds("Clearing Habit Calendar....DONE", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(displayWidth / 2 - w / 2, displayHeight / 2 - h / 2);
    display.print("Clearing Habit Calendar....");
    display.refresh();
    clearChecks();
    delay(2000);
    display.print("DONE");
    display.refresh();
    delay(3000);
  }

  if (msg == 2) {

    display.setFont(&FreeSansBold12pt7b);
    display.getTextBounds("Clearing WIFI SETTINGS....DONE", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(displayWidth / 2 - w / 2, displayHeight / 2 - h / 2);
    display.print("Clearing WIFI SETTINGS....");
    display.refresh();

    WiFi.mode(WIFI_STA);
    WiFiManager wm;
    wm.resetSettings();

    delay(2000);
    display.print("DONE");
    display.refresh();
    delay(3000);
    display.fillScreen(BGCOLOR);
    display.setTextColor(TEXTCOLOR);
    display.getTextBounds("RESTARTING DEVICE", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(displayWidth / 2 - w / 2, displayHeight / 2 - h / 2);
    display.print("RESTARTING DEVICE");
    display.refresh();
    delay(2000);
    ESP.restart();
  }

  if (msg == 3) {

    display.setFont(&FreeSansBold12pt7b);
    display.getTextBounds("Clearing Saved Values....DONE", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(displayWidth / 2 - w / 2, displayHeight / 2 - h / 2);
    display.print("Clearing Saved Values....");
    display.refresh();
    factoryErase();
    delay(2000);
    display.print("DONE");
    display.refresh();
    delay(3000);

    display.fillScreen(BGCOLOR);
    display.setTextColor(TEXTCOLOR);
    display.refresh();

    display.setFont(&FreeSansBold12pt7b);
    display.getTextBounds("Clearing WIFI SETTINGS....DONE", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(displayWidth / 2 - w / 2, displayHeight / 2 - h / 2);
    display.print("Clearing WIFI SETTINGS....");
    display.refresh();

    WiFi.mode(WIFI_STA);
    WiFiManager wm;
    wm.resetSettings();

    delay(2000);
    display.print("DONE");
    display.refresh();
    delay(3000);
    display.fillScreen(BGCOLOR);
    display.setTextColor(TEXTCOLOR);
    display.getTextBounds("RESTARTING DEVICE", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(displayWidth / 2 - w / 2, displayHeight / 2 - h / 2);
    display.print("RESTARTING DEVICE");
    display.refresh();
    delay(2000);
    ESP.restart();
  }


  display.fillScreen(BGCOLOR);
  display.setTextColor(TEXTCOLOR);

  int startX = displayMarginW / 2;
  int startY = displayMarginH * 2 + 10;

  display.setCursor(startX, startY);
  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("SETTINGS", 0, 0, &xP, &yP, &w, &h);
  display.print("SETTINGS");

  headerXStart = startX + w;

  int xStart = 10;
  int yStart = 85;

  int displayValues = 5;
  int displayRows = 2;
  int xTab;
  int yTab = 10;
  int x = xStart;
  int y = yStart;
  int iconW = 50;
  int iconH = 50;
  int iconR = 4;
  int xIconBox;
  int xIcon;
  int yIcon;
  int iconNum = 0;

  for (int r = 0; r < displayRows; r++) {

    xTab = ((displayWidth - xStart * 2) / (displayValues));
    x = xStart;

    if (selectedSettingIcon + 1 > displayValues * 2) {
      iconNum = (displayValues * 2);
      displayValues = min(displayValues, int(numOfSettingsItems) - displayValues * 2);
    }

    for (int i = 0; i < displayValues; i++) {
      xIconBox = x + xTab / 2 - iconW / 2;

      if (selectedSettingIcon == iconNum) {

        display.setFont(&FreeSans9pt7b);
        display.getTextBounds(" (0/00)", 0, 0, &xP, &yP, &w, &h);
        display.setCursor(displayWidth - (displayMarginW + w) , displayMarginH * 2 );
        display.print(" (");
        display.print(iconNum + 1);
        display.print("/");
        display.print(int(numOfSettingsItems));
        display.print(")");

        selectedSettingFunctionNumber = settingsIconArray[iconNum].functionNumber;
        display.setFont(&FreeSansBold12pt7b);
        display.setCursor(headerXStart, startY);
        display.print(": ");
        display.print(settingsIconArray[iconNum].displayName);

        display.fillRoundRect(xIconBox, y, iconW, iconH, iconR, TEXTCOLOR);

        renderSetting();
        display.setTextColor(BGCOLOR);

      } else {
        display.drawRoundRect(xIconBox, y, iconW, iconH, iconR, TEXTCOLOR);
      }

      display.setFont(&slateIcons14pt7b);
      display.getTextBounds(settingsIconArray[iconNum].displayIcon, 0, 0, &xP, &yP, &w, &h);
      xIcon = xIconBox + iconW / 2 - w / 2;
      yIcon = y + iconH / 2 + h / 3;
      display.setCursor(xIcon , yIcon);
      display.print(settingsIconArray[iconNum].displayIcon);

      display.setTextColor(TEXTCOLOR);

      x += xTab;
      iconNum++;

    }

    if (iconNum == numOfSettingsItems) {
      break;
    }
    y += iconH + yTab;

  }


  display.setFont(&slateIcons14pt7b);

  if (selectedSettingIcon > 0) {
    display.getTextBounds(lefttoggleG, 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print(lefttoggleG);
  } else if (selectedSettingFunctionNumber == DEVICEDETAILS_SETTING && versionUpdateAvailable) {
    display.setFont(&FreeSansBold9pt7b);
    display.getTextBounds("UPGRADE", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("UPGRADE");
  }

  display.setFont(&slateIcons14pt7b);
  if (selectedSettingIcon < numOfSettingsItems - 1) {
    display.getTextBounds(righttoggleG, 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_D_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print(righttoggleG);
  }

  display.setFont(&FreeSansBold9pt7b);

  display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("EXIT");

  display.refresh();

}

void handleSetting () {

  switch (selectedSettingFunctionNumber) {
    //case DEVICEDETAILS_SETTING:
    //if (versionUpdateAvailable) {
    //  updateVersion();
    //}
    //break;
    case TIMEDISPLAY_SETTING:
      toggleClockFormat();
      break;
    case UNIT_SETTING:
      toggleSystemMeasure();
      break;
    case DARKMODE_SETTING:
      toggleLightMode();
      break;
    case DEFAULTSCREEN_SETTING:
      toggleDefaultScreen();
      break;
    case CLEARHABITS_SETTING:
      renderSettingsDisplay(1);
      break;
    case RESETWIFI_SETTING:
      renderSettingsDisplay(2);
      break;
    case FACTORYRESET_SETTING:
      renderSettingsDisplay(3);
      break;
    case REINSTALL_SETTING:
      updateVersion();
      break;
    default:
      renderSettingsInfoDisplay(selectedSettingFunctionNumber);
      break;
  }

}


void handleSettingsDetails(int button) {

  switch (button)
  {
    case BUTTON_C :
      if (selectedSettingIcon > 0) {
        selectedSettingIcon--;
        renderSettingsDisplay(0);
      } else if (selectedSettingFunctionNumber == DEVICEDETAILS_SETTING && versionUpdateAvailable) {
        updateVersion();
      }
      break;
    case BUTTON_D :
      if (selectedSettingIcon < numOfSettingsItems - 1) {
        selectedSettingIcon++;
        renderSettingsDisplay(0);
      }
      break;
    case BUTTON_E :
      handleSetting();
      if (selectedSettingFunctionNumber < 9000) {
        renderSettingsDisplay(0);
      }
      break;
    case BUTTON_F :
      selectedSettingIcon = 0;
      exitScreensRoutine();
      timeoutCounter = 0; //force sleep
      break;
  }
  return;
}

void handleSettingsInfoDetails(int button) {

  switch (button)
  {
    case BUTTON_C :
      break;
    case BUTTON_D :
      break;
    case BUTTON_E :
      break;
    case BUTTON_F :
      renderSettingsDisplay();
      break;
  }
  return;
}
