boolean isHourlyDisplay = true;
int weatherDisplayPage = 1;
boolean isCurrentDisplay = true;

void renderHourlyForecast() {

  int xStart = 10;
  int yStart = 110;
  int displayValues = 5;  //= 5;
  int xTab;
  int x = xStart;
  int y = yStart;
  int iconH = 0;

  xTab = ((displayWidth - xStart * 2) / (displayValues));
  x = xStart;

  File forecastJSON = SPIFFS.open("/hourlyWeather.json", FILE_READ);

  if (!forecastJSON) {
    Serial.println("The file does not exist");
    return;
  } else {

    size_t capacity = forecastJSON.size() * 4;
    if ( capacity == 0 ) {
      Serial.println("The file is empty");
      return;
    } else {
      Serial.println("Getting data");
      DynamicJsonDocument hourly(capacity);
      DeserializationError err = deserializeJson(hourly, forecastJSON);

      forecastJSON.close();
      if (err) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(err.c_str());
        return;
      }

      int startCounter = 0;

      boolean twentyFourHourVal = isTwentyFourHour();

      if (weatherDisplayPage == 2 && isHourlyDisplay) {
        startCounter = 5;
        displayValues = 10;
      }

      for (int i = startCounter; i < displayValues; i++) {

        JsonObject hourlyObj = hourly[i];
        const char* hr = hourlyObj["h"]; // "11"
        const char* am = hourlyObj["am"]; // "PM"
        const char* hrTF = hourlyObj["htf"]; // "23"
        int temp = hourlyObj["t"][unitSetting]; // 46
        int precip = hourlyObj["p"]; // 0
        const char* icon = hourlyObj["i"]; // "C"

        y = yStart;

        //hour --------
        char hramChr[5];

        if (twentyFourHourVal) {
          strcpy(hramChr, hrTF);
          strcat(hramChr, ":00");
        } else {
          strcpy(hramChr, hr);
          strcat(hramChr, am);
        }

        display.setFont(&FreeSansBold9pt7b);
        display.getTextBounds(hramChr, 0, 0, &xP, &yP, &w, &h);
        display.setCursor(x + xTab / 2 - w / 2, y);


        if (twentyFourHourVal) {
          display.print(hrTF);
          display.print(":00");
        } else {
          display.print(hr);
          display.setFont(&FreeSansBold7pt7b);
          display.print(am);
        }


        y += 30;

        //precipitation chance--------

        if (precip > 0) {

          char precipPercChar[5];
          char precipChar[3];
          itoa(precip, precipChar, 10);
          strcpy(precipPercChar, precipChar);
          strcat(precipPercChar, "%");

          display.setFont(&FreeSans9pt7b);

          display.getTextBounds(precipPercChar, 0, 0, &xP, &yP, &w, &h);
          display.setCursor(x + xTab / 2 - w / 2, y);
          display.print(precipPercChar);

        }

        //icon--------

        display.setFont(&slateWeather14pt7b);
        display.getTextBounds(icon, 0, 0, &xP, &yP, &w, &h);
        int iconCenter = x + (xTab / 2) - (w / 2);
        iconH = h;

        y += iconH + 10; //58 is the approx height of the icon

        display.setCursor(iconCenter, y);
        display.print(icon);

        //temperature--------
        char tempChar[4];
        itoa(temp, tempChar, 10);

        display.setFont(&FreeSansBold9pt7b);
        display.getTextBounds(tempChar, 0, 0, &xP, &yP, &w, &h);

        y += h + 9;//changed from 5 to 9

        int tempX = x + (xTab / 2) - (w / 2);
        display.setCursor(tempX, y);
        display.print(tempChar);
        display.fillCircle(tempX + w + 6, y - (h * .8), 4, TEXTCOLOR);
        display.fillCircle(tempX + w + 6, y - (h * .8), 1, BGCOLOR);

        x += xTab;

      }
    }
  }

}

void renderDailyForecast() {

  int xStart = 10;
  int yStart = 110;
  int displayValues = 5;  //= 5;
  int xTab;
  int x = xStart;
  int y = yStart;
  int iconH = 0;

  xTab = ((displayWidth - xStart * 2) / (displayValues));
  x = xStart;

  File forecastJSON = SPIFFS.open("/dailyWeather.json", FILE_READ);

  if (!forecastJSON) {
    Serial.println("The file does not exist");
    return;
  } else {

    size_t capacity = forecastJSON.size() * 4;
    if ( capacity == 0 ) {
      Serial.println("The file is empty");
      return;
    } else {
      Serial.println("Getting data");
      DynamicJsonDocument daily(capacity);
      DeserializationError err = deserializeJson(daily, forecastJSON);

      forecastJSON.close();
      if (err) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(err.c_str());
        return;
      }

      int startCounter = 0;

      for (int i = startCounter; i < displayValues; i++) {

        JsonObject dailyObj = daily[i];
        const char* wd = dailyObj["day"]; // "FRI"
        int tMin = dailyObj["tmin"][unitSetting]; // 40
        int tMax = dailyObj["tmax"][unitSetting]; // 56
        int dPrec = dailyObj["p"]; // 0
        const char* dIcon = dailyObj["i"]; // "c"

        y = yStart;

        display.setFont(&FreeSansBold9pt7b);
        display.getTextBounds(wd, 0, 0, &xP, &yP, &w, &h);
        display.setCursor(x + xTab / 2 - w / 2, y);
        display.print(wd);

        y += 22;

        if (dPrec > 0) {

          char precipPercChar[5];
          char precipChar[3];
          itoa(dPrec, precipChar, 10);
          strcpy(precipPercChar, precipChar);
          strcat(precipPercChar, "%");

          display.setFont(&FreeSans9pt7b);

          display.getTextBounds(precipPercChar, 0, 0, &xP, &yP, &w, &h);
          display.setCursor(x + xTab / 2 - w / 2, y);
          display.print(precipPercChar);

        }

        //icon------

        display.setFont(&slateWeather14pt7b);
        display.getTextBounds(dIcon, 0, 0, &xP, &yP, &w, &h);
        int iconCenter = x + (xTab / 2) - (w / 2);
        iconH = h;

        y += iconH + 10; //58 is the approx height of the icon

        display.setCursor(iconCenter, y);
        display.print(dIcon);

        //temperature--------
        char tempMaxChar[4];
        itoa(tMax, tempMaxChar, 10);
        char tempMinChar[4];
        itoa(tMin, tempMinChar, 10);

        //Max temp
        display.setFont(&FreeSansBold9pt7b);
        display.getTextBounds(tempMaxChar, 0, 0, &xP, &yP, &w, &h);

        y += h + 5;

        int tempX = x + (xTab / 2) - (w / 2);
        display.setCursor(tempX, y);
        display.print(tempMaxChar);
        display.fillCircle(tempX + w + 6, y - (h * .8), 4, TEXTCOLOR);
        display.fillCircle(tempX + w + 6, y - (h * .8), 1, BGCOLOR);

        //Min temp
        display.setFont(&FreeSans9pt7b);
        display.getTextBounds(tempMinChar, 0, 0, &xP, &yP, &w, &h);

        y += h + 8;

        tempX = x + (xTab / 2) - (w / 2);
        display.setCursor(tempX, y);
        display.print(tempMinChar);
        display.fillCircle(tempX + w + 6, y - (h * .8), 4, TEXTCOLOR);
        display.fillCircle(tempX + w + 6, y - (h * .8), 2, BGCOLOR);

        x += xTab;

      }

      display.refresh();
    }
  }

}

void renderCurrentInfo() {

  int xStart = 10;
  int yStart = 35;
  int x = xStart;
  int y = yStart;
  int xSummary = 0;
  int ySummary = 0;

  x = xStart;
  yStart += 5;

  display.setFont(&slateWeather22pt7b);
  display.getTextBounds(currWeatherIcon, 0, 0, &xP, &yP, &w, &h);
  int iconX = 20;

  yStart += h - 5; // + 5;

  display.setCursor(iconX, yStart);
  display.print(currWeatherIcon);

  ySummary = 70;
  xSummary+=w*2;

  display.setCursor(xSummary, ySummary);
  display.setFont(&FreeSansBold18pt7b);
  display.setFont(&FreeSansBold9pt7b);

  char currWeatherChar[4];
  itoa(currentTemp[unitSetting], currWeatherChar, 10);

  display.setFont(&FreeSansBold18pt7b);
  display.getTextBounds(currWeatherChar, 0, 0, &xP, &yP, &w, &h);
  
  display.print(currWeatherChar);

  int degreeAdd = 7;
  int outerDegreeRadius = 4;
  int innerDegreeRadius = 1;

  display.fillCircle(xSummary + w + degreeAdd, ySummary - (h * .8), outerDegreeRadius, TEXTCOLOR);
  display.fillCircle(xSummary + w + degreeAdd, ySummary - (h * .8), innerDegreeRadius, BGCOLOR);

  xSummary += w + degreeAdd + 20;
  ySummary -= h / 2;

  display.setCursor(xSummary, ySummary);
  display.setFont(&FreeSans9pt7b);
  display.print("Feels Like: ");
  display.getTextBounds("Feels Like: ", 0, 0, &xP, &yP, &w, &h);
  int feelText = w;

  display.setFont(&FreeSansBold9pt7b);
  char feelsWeatherChar[4];
  itoa(currentFeelsTemp[unitSetting], feelsWeatherChar, 10);
  display.getTextBounds(feelsWeatherChar, 0, 0, &xP, &yP, &w, &h);
  display.print(feelsWeatherChar);

  display.fillCircle(xSummary + feelText + w + degreeAdd + 5, ySummary - (h * .8), outerDegreeRadius, TEXTCOLOR);
  display.fillCircle(xSummary + feelText + w + degreeAdd + 5, ySummary - (h * .8), innerDegreeRadius, BGCOLOR);

  ySummary += 20;

  display.setCursor(xSummary, ySummary);
  display.setFont(&FreeSans9pt7b);
  display.print("Humidity: ");
  display.setFont(&FreeSansBold9pt7b);
  display.print(currHumidity);

  display.print("%");

}

void renderWeatherDisplay() {

  currentDisplay = WEATHER;
  display.fillScreen(BGCOLOR);

  int xStart = 10;
  int yStart = 40;
  int displayValues;  //= 5;
  int xTab;
  int x = xStart;
  int y = yStart;
  int yTopLine = 87;
  int yBottomLine = 210;

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("WEATHER", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(x, displayMarginH + h);

  display.print("WEATHER");

  renderCurrentInfo();

  display.drawLine(xStart, yTopLine, displayWidth - xStart * 2, yTopLine, TEXTCOLOR);

  display.setFont(&FreeSansBold9pt7b);

  if (isHourlyDisplay) {
    //displayValues = 5;
    display.getTextBounds("HOURLY - 2 of 2", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(displayWidth - (w + displayMarginH), displayMarginH + h);
    display.print("HOURLY - ");
    display.print(weatherDisplayPage);
    display.print(" of 2");
    renderHourlyForecast();
  }
  else {
    //display.print("DAILY FORECAST");
    display.getTextBounds("DAILY", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(displayWidth - (w + displayMarginH), displayMarginH + h);
    display.print("DAILY");
    renderDailyForecast();
  }

  display.drawLine(xStart, yBottomLine, displayWidth - xStart * 2, yBottomLine, TEXTCOLOR);

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold9pt7b);

  if (isHourlyDisplay) {
    display.getTextBounds("DAILY", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_D_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("DAILY");
  } else {
    display.getTextBounds("HOURLY", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_D_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("HOURLY");
  }

  if (isHourlyDisplay) {
    if (weatherDisplayPage == 1) {
      display.getTextBounds("PAGE 2", 0, 0, &xP, &yP, &w, &h);
      display.setCursor(BUTTON_E_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
      display.print("PAGE 2");
    } else {
      display.getTextBounds("PAGE 1", 0, 0, &xP, &yP, &w, &h);
      display.setCursor(BUTTON_E_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
      display.print("PAGE 1");
    }
  }

  display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("EXIT");

  display.refresh();



}

void toggleWeatherDisplay(int button) {

  switch (button)
  {
    case BUTTON_D :
      isHourlyDisplay = !isHourlyDisplay;
      weatherDisplayPage = 1;
      renderWeatherDisplay();
      break;
    case BUTTON_E :
      if (isHourlyDisplay) {
        weatherDisplayPage++;
        if (weatherDisplayPage == 3) {
          weatherDisplayPage = 1;
        }
        renderWeatherDisplay();
      }
      break;
    case BUTTON_C :
      break;
    case BUTTON_F :
      exitScreensRoutine();
      break;
  }
  return;
}

boolean showAQReadings = true;

void renderAQDisplay() {

  currentDisplay = AIRQUALITY;
  display.fillScreen(BGCOLOR);

  int xStart = 10;
  int yStart = 40;
  int xTab;
  int x = xStart;
  int y = yStart;

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("AIR QUALITY", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(x, displayMarginH + h);
  display.print("AIR QUALITY");

  //display.setCursor(x, displayMarginH + h + 20);

  File aqJSON = SPIFFS.open("/airquality.json", FILE_READ);

  if (!aqJSON) {
    Serial.println("The file does not exist");
    ///display.print("no file");
    // return;
  } else {

    StaticJsonDocument<1200> doc;
    DeserializationError err = deserializeJson(doc, aqJSON);

    if (err) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(err.c_str());
      //display.print("json error");
    } else {
      Serial.println("Getting data");
      //display.print("got data - ");
      aqJSON.close();

      if (showAQReadings) {
        //Display currrent

        int current_aqi = doc["current"]["aqi"];
        const char* current_icon = doc["current"]["icon"];
        const char* current_desc = doc["current"]["desc"];

        x = 45;

        display.setFont(&slateIcons30pt7b);
        display.getTextBounds(current_icon, 0, 0, &xP, &yP, &w, &h);
        int iconX = x + (xTab / 2) - (w / 2);

        y += h + 5;
        int textX = iconX + w;

        display.setCursor(iconX, y);
        display.print(current_icon);

        int textY = y - h;

        //Currently
        display.setFont(&FreeSansBold9pt7b);
        display.getTextBounds("NOW", 0, 0, &xP, &yP, &w, &h);

        textX += 10;
        textY += h;
        display.setCursor(textX, textY);
        display.print("NOW");

        textY += h + 10;

        //Current AQI--------
        char currAQI[4];
        itoa(current_aqi, currAQI, 10);
        display.setFont(&FreeSansBold9pt7b);
        display.getTextBounds(currAQI, 0, 0, &xP, &yP, &w, &h);

        display.setCursor(textX, textY);
        display.print("AQI: ");
        display.print(currAQI);

        textY += h + 10;


        //Current STATUS--------
        display.setCursor(textX, textY);
        display.print("STATUS: ");
        display.print(current_desc);


        //Display Forecast
        int displayValues = 4;
        int displayCounter = 0;
        xTab = ((displayWidth - xStart * 2) / (displayValues));
        x = xStart;

        yStart += 95;

        for (JsonObject forecast_item : doc["forecast"].as<JsonArray>()) {

          y = yStart;

          int forecast_item_avg = forecast_item["avg"]; // 75, 51, 50, 52, 33
          int forecast_item_max = forecast_item["max"]; // 104, 59, 55, 64, 40
          int forecast_item_min = forecast_item["min"]; // 35, 35, 34, 36, 25
          const char* forecast_item_icon = forecast_item["icon"]; // "y", "y", "g", "y", "g"
          const char* forecast_item_d = forecast_item["d"]; // "FRI", "SAT", "SUN", "MON", "TUE"

          display.setFont(&FreeSansBold9pt7b);
          display.getTextBounds(forecast_item_d, 0, 0, &xP, &yP, &w, &h);
          display.setCursor(x + xTab / 2 - w / 2, y);
          display.print(forecast_item_d);

          display.setFont(&slateIcons14pt7b);
          display.getTextBounds(forecast_item_icon, 0, 0, &xP, &yP, &w, &h);
          int iconCenter = x + (xTab / 2) - (w / 2);

          y += h + 5;

          display.setCursor(iconCenter, y);
          display.print(forecast_item_icon);


          //Avg AQI Desc--------;
          char * aq = aqDesc (forecast_item_avg);

          display.setFont(&FreeSans9pt7b);
          display.getTextBounds(aq, 0, 0, &xP, &yP, &w, &h);

          y += h + 5;

          int valueX = x + (xTab / 2) - (w / 2);
          display.setCursor(valueX, y);
          display.print(aq);
          free (aq);

          //Avg AQI--------
          char avgAQI[4];
          itoa(forecast_item_avg, avgAQI, 10);
          display.setFont(&FreeSans9pt7b);
          display.getTextBounds(avgAQI, 0, 0, &xP, &yP, &w, &h);

          y += h + 5;

          valueX = x + (xTab / 2) - (w / 2);
          display.setCursor(valueX, y);
          display.print(avgAQI);

          x += xTab;

          displayCounter++;

          if (displayCounter == displayValues) {
            break;
          }
        }
      } else {

        int y = 60;

        display.setFont(&FreeSansBold9pt7b);


        display.setCursor(x, y);
        display.print("Attribution");

        display.setFont(&FreeSans9pt7b);
        JsonArray attributions = doc["attributions"];
        for (JsonVariant credit : attributions) {
          y += 20;
          display.setCursor(x, y);
          display.print(credit.as<const char*>());
        }
      }
    }
  }

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold9pt7b);

  display.getTextBounds("INFO", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_E_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);

  if (showAQReadings) {
    display.print("INFO");
  } else {
    display.print("BACK");
  }

  display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("EXIT");

  display.refresh();

}

void handleAQDisplay(int button) {

  switch (button)
  {
    case BUTTON_C :
      break;
    case BUTTON_D :
      break;
    case BUTTON_E :
      showAQReadings = !showAQReadings;
      renderAQDisplay();
      break;
    case BUTTON_F :
      showAQReadings = true;
      exitScreensRoutine();
      break;
  }
  return;
}

boolean showUVReadings = true;

void renderUVDisplay() {

  currentDisplay = UV;
  display.fillScreen(BGCOLOR);

  int xStart = 10;
  int yStart = 40;
  int xTab;
  int x = xStart;
  int y = yStart;

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("UV INDEX", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(x, displayMarginH + h);
  display.print("UV INDEX");

  //display.setCursor(x, displayMarginH + h + 20);

  File uvJSON = SPIFFS.open("/dailyWeather.json", FILE_READ);

  if (!uvJSON) {
    Serial.println("The file does not exist");
    ///display.print("no file");
    // return;
  } else {

    size_t capacity = uvJSON.size() * 4;
    if ( capacity == 0 ) {
      Serial.println("The file is empty");
      return;
    } else {
      Serial.println("Getting data");
      DynamicJsonDocument daily(capacity);
      DeserializationError err = deserializeJson(daily, uvJSON);

      if (err) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(err.c_str());
      } else {
        Serial.println("Getting data");
        uvJSON.close();


        //Display currrent

        x = 45;

        char * uvIconNow = uvIcon (currUV);

        display.setFont(&slateIcons30pt7b);
        display.getTextBounds(uvIconNow, 0, 0, &xP, &yP, &w, &h);
        int iconX = x + (xTab / 2) - (w / 2);

        y += h + 5;
        int textX = iconX + w;

        display.setCursor(iconX, y);
        display.print(uvIconNow);

        int textY = y - h;

        //Currently
        display.setFont(&FreeSansBold9pt7b);
        display.getTextBounds("NOW", 0, 0, &xP, &yP, &w, &h);

        textX += 10;
        textY += h;
        display.setCursor(textX, textY);
        display.print("NOW");

        textY += h + 10;

        //Current UV--------
        char currUVtemp[4];
        itoa(currUV, currUVtemp, 10);

        display.setFont(&FreeSansBold9pt7b);
        display.getTextBounds(currUVtemp, 0, 0, &xP, &yP, &w, &h);

        display.setCursor(textX, textY);
        display.print("UV Index: ");
        display.print(currUVtemp);

        textY += h + 10;

        //Current STATUS--------
        char * uvDescCurr = uvDesc (currUV);
        display.setCursor(textX, textY);
        display.print("STATUS: ");
        display.print(uvDescCurr);
        free (uvDescCurr);


        //Display UV Forecast
        int displayValues = 4;
        int displayCounter = 0;
        xTab = ((displayWidth - xStart * 2) / (displayValues));
        x = xStart;

        yStart += 95;

        for (JsonObject item : daily.as<JsonArray>()) {

          y = yStart;

          const char* forecast_item_d = item["day"]; // "SUN", "MON", "TUE", "WED", "THU"
          int uv = item["uv"]; // 9, 8, 8, 8, 8

          //Day

          display.setFont(&FreeSansBold9pt7b);
          display.getTextBounds(forecast_item_d, 0, 0, &xP, &yP, &w, &h);
          display.setCursor(x + xTab / 2 - w / 2, y);
          display.print(forecast_item_d);

          //UV Icon

          char * uvIconVal = uvIcon (uv);

          display.setFont(&slateIcons14pt7b);
          display.getTextBounds(uvIconVal, 0, 0, &xP, &yP, &w, &h);
          int iconCenter = x + (xTab / 2) - (w / 2);

          y += h + 5;

          display.setCursor(iconCenter, y);
          display.print(uvIconVal);


          //UV Desc--------;
          char * uvDescVal = uvDesc (uv);

          display.setFont(&FreeSans9pt7b);
          display.getTextBounds(uvDescVal, 0, 0, &xP, &yP, &w, &h);

          y += h + 5;

          int valueX = x + (xTab / 2) - (w / 2);
          display.setCursor(valueX, y);
          display.print(uvDescVal);
          free (uvDescVal);

          //UV Reading --------
          char uvVal[4];
          itoa(uv, uvVal, 10);
          display.setFont(&FreeSans9pt7b);
          display.getTextBounds(uvVal, 0, 0, &xP, &yP, &w, &h);

          y += h + 5;

          valueX = x + (xTab / 2) - (w / 2);
          display.setCursor(valueX, y);
          display.print(uvVal);

          x += xTab;

          displayCounter++;

          if (displayCounter == displayValues) {
            break;
          }
        }
      }
    }
  }

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold9pt7b);


  display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
  display.print("EXIT");

  display.refresh();

}

void handleUVDisplay(int button) {

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
