boolean isWifiInited() {
  if (EEPROM.read(WIFI_ENABLED_ADDR) != 1) {
    return false;
  }
  return true;
}

void noteWifiInited() {
  EEPROM.write(WIFI_ENABLED_ADDR, 1);
  EEPROM.commit();
}

void disconnectWifi() {
  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void checkReadyToSleepArray() {
  int checkSum = 0;
  for (int i = 0; i < readyToSleepCount; i++) {
    checkSum += readyToSleep[i];
  }

  if (checkSum < 1 && currentDisplay == HOME) {
    timeoutCounter = 0;
  } else if (currentDisplay == HOME) {
    renderMainDisplay();
  }

}

void setCurrentWeather(long epoch, int t, int offsetVal, const char* i) {
  showWeather = true;
  //currWeather = t;
  strcpy(currWeatherIcon, i);
}

void setQuote(const char* q, const char* a) {
  showQuote = true;
  strcpy(currQuote, q);
  strcpy(currAuthor, a);
}

void setOblique(const char* o) {
  strcpy(currOblique, o);
  Serial.println(o);
}

void setRiddle(const char* r, const char* a, const char* at, int cat) {
  strcpy(dailyRiddle, r);
  strcpy(dailyRiddleAnswer, a);
  strcpy(dailyRiddleAttr, at);
  riddleCategoryNo = cat;
}

void sendRequest(const char* request) {

  Serial.print("making request for: "); Serial.println(request);

  StaticJsonDocument<300> doc;

  doc["deviceID"] = uniqId;
  doc["userID"] = "N/A";
  doc["request"] = request;
  doc["majorV"] = SWVERSION_MAJOR;
  doc["minorV"] = SWVERSION_MINOR;
  doc["patchV"] = SWVERSION_PATCH;

  char buf[512];
  serializeJson(doc, buf);
  client.publish(mqtt_pub_topic, buf);
}

void messageReceived(String &topic, String &payload) {
  Serial.print("Received. topic=");
  Serial.println(topic);
  Serial.print("Received. payload=");
  Serial.println(payload);

  if (payload.length()<1){
    return;
  }

  if (topic.indexOf("quote") > 0) {
    Serial.println("We got the message");
    DynamicJsonDocument doc(128);
    deserializeJson(doc, payload);

    const char* q = doc["q"];
    const char* a = doc["a"];

    setQuote((const char *) q, (const char *) a);
    readyToSleep[sleepCheckQuote] = 0;
    checkReadyToSleepArray();

  }

  if (topic.indexOf("obliques") > 0) {

    StaticJsonDocument<250> doc;
    deserializeJson(doc, payload);

    const char* o = doc["o"];

    setOblique((const char *) o);
    readyToSleep[sleepCheckOblique] = 0;
    checkReadyToSleepArray();

  }

  if (topic.indexOf("riddle") > 0) {

    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload);

    int no = doc["no"]; // 21
    const char* cd = doc["cd"]; // "Brain Teaser"
    int cno = doc["cno"]; // 2
    const char* R = doc["R"]; // "You're escaping a maze, and there are three doors in front of you. The ...
    const char* A = doc["A"]; // "The door on the right. A lion that hasn't eaten in three months would be ...
    const char* Attr = doc["Attr"]; // "Reader's Digest"

    setRiddle( (const char*) R, (const char*)  A, (const char*)  Attr, cno);


    readyToSleep[sleepCheckRiddle] = 0;
    checkReadyToSleepArray();

  }

  if (topic.indexOf("currentWeather") > 0) {
    StaticJsonDocument<600> doc;

    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    strcpy(currWeatherIcon, doc["i"]);
    currentTemp[0] = doc["t"][0];
    currentTemp[1] = doc["t"][1];
    currentFeelsTemp[0] = doc["fl"][0];
    currentFeelsTemp[1] = doc["fl"][1];
    currHumidity = doc["h"];
    currUV = doc["uv"];

    showWeather = true;

    SPIFFS.remove("/currentWeather.json");

    File currentWeatherJSON = SPIFFS.open("/currentWeather.json", FILE_WRITE);
    if (!currentWeatherJSON) {
      Serial.println("There was an error opening the file for writing");
      return;
    }

    if (serializeJson(doc, currentWeatherJSON) == 0) {
      Serial.println(F("Failed to write to file"));
    }

    currentWeatherJSON.close();

    readyToSleep[sleepCheckCWeather] = 0;
    checkReadyToSleepArray();
  }

  if (topic.indexOf("dailyWeather") > 0) {

    StaticJsonDocument<2048> doc;

    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    int dayCounter = 0;

    for (JsonObject item : doc.as<JsonArray>()) {

      JsonArray ss = item["ss"];
      JsonArray sr = item["sr"];

      if (dayCounter < 1) {
        dayCounter++;
        currDayNo = item["dno"];
        strcpy(todayWDay, item["day"]);

        strcpy(dailyWeatherIcon[0], item["i"]);

        todayHiTemp[0] = item["tmax"][0];
        todayHiTemp[1] = item["tmax"][1];
        todayLoTemp[0] = item["tmin"][0];
        todayLoTemp[1] = item["tmin"][1];
        strcpy(todaySunrise[0], sr[0]);
        strcpy(todaySunrise[1], sr[1]);
        strcpy(todaySunrise[2], sr[2]);
        strcpy(todaySunset[0], ss[0]);
        strcpy(todaySunset[1], ss[1]);
        strcpy(todaySunset[2], ss[2]);
      } else if (dayCounter < 2) {
        dayCounter++;

        strcpy(dailyWeatherIcon[1], item["i"]);

        strcpy(tomWDay, item["day"]);
        tomHiTemp[0] = item["tmax"][0];
        tomHiTemp[1] = item["tmax"][1];
        tomLoTemp[0] = item["tmin"][0];
        tomLoTemp[1] = item["tmin"][1];
        strcpy(tomSunrise[0], sr[0]);
        strcpy(tomSunrise[1], sr[1]);
        strcpy(tomSunrise[2], sr[2]);
        strcpy(tomSunset[0], ss[0]);
        strcpy(tomSunset[1], ss[1]);
        strcpy(tomSunset[2], ss[2]);

      }

    }

    showDailyWeather = true;

    SPIFFS.remove("/dailyWeather.json");

    File dailyWeatherJSON = SPIFFS.open("/dailyWeather.json", FILE_WRITE);
    if (!dailyWeatherJSON) {
      Serial.println("There was an error opening the file for daily weather writing");
      return;
    }

    if (serializeJson(doc, dailyWeatherJSON) == 0) {
      Serial.println(F("Failed to write daily weather to file"));
    }

    dailyWeatherJSON.close();

    readyToSleep[sleepCheckDWeather] = 0;
    checkReadyToSleepArray();

  }

  if (topic.indexOf("hourlyWeather") > 0) {

    StaticJsonDocument<2048> doc;

    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return;
    }

    SPIFFS.remove("/hourlyWeather.json");

    File hourlyWeatherJSON = SPIFFS.open("/hourlyWeather.json", FILE_WRITE);
    if (!hourlyWeatherJSON) {
      Serial.println("There was an error opening the file for hourly weather writing");
      return;
    }

    if (serializeJson(doc, hourlyWeatherJSON) == 0) {
      Serial.println(F("Failed to write hourly weather to file"));
    }

    hourlyWeatherJSON.close();

    readyToSleep[sleepCheckHWeather] = 0;
    checkReadyToSleepArray();

  }

  if (topic.indexOf("airQuality") > 0) {
    Serial.println(F("Received AQ"));
    StaticJsonDocument<1200> doc;
    deserializeJson(doc, payload);

    JsonObject current = doc["current"];
    currAQ = current["aqi"]; // 9

    SPIFFS.remove("/airquality.json");

    File aqJSON = SPIFFS.open("/airquality.json", FILE_WRITE);
    if (!aqJSON) {
      Serial.println("There was an error opening the file for writing");
      return;
    }

    if (serializeJson(doc, aqJSON) == 0) {
      Serial.println(F("Failed to write to file"));
    }

    Serial.println(F("Wrote to AQ file"));

    aqJSON.close();

    readyToSleep[sleepCheckAQI] = 0;
    checkReadyToSleepArray();
  }


  /*if (topic.indexOf("todo") > 0) {

    DynamicJsonDocument doc(4000);
    deserializeJson(doc, payload);

    SPIFFS.remove("/todolist.json");

    File todoJSON = SPIFFS.open("/todolist.json", FILE_WRITE);
    if (!todoJSON) {
      Serial.println("There was an error opening the file for writing");
      return;
    }

    if (serializeJson(doc, todoJSON) == 0) {
      Serial.println(F("Failed to write to file"));
    }

    todoJSON.close();
    updatedTodos = true;

    }*/

}

void connect() {
  Serial.print("checking wifi...");
  wifiTimeoutCounter = millis() + MILLS_BEFORE_WIFI_TIMEOUT;

  wifiFailed = false;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    if (millis() > wifiTimeoutCounter) {
      wifiFailed = true;
      break;
    }
  }

  if (!wifiFailed) {

    Serial.print("\nconnecting...");
    while (!client.connect(uniqId, mqtt_user, mqtt_token)) {
      Serial.print(".");
      delay(1000);
    }

    Serial.println("\nconnected!");

    for (int i = 0; i < numOfTopics; i++) {
      char topic[60];
      strcpy(topic, uniqId);
      strcat(topic, "/");
      strcat(topic, topics[i]);
      client.subscribe(topic);
      Serial.println(topic);
    }

  } else {
    Serial.print("need an error message");
  }

}

void mqttLoop() {

  if (shouldBeConnected) {

    //do I need this if statement
    if (WiFi.status() != WL_CONNECTED) {

      WiFi.begin();

    }
    client.loop();
    delay(10);  // <- fixes some issues with WiFi stability

    if (!client.connected()) {
      connect();
    }
  }
}


void requestForecast() {
  Serial.println("Requesting Weather Forecast");
  sendRequest("forecast");
}

void requestTimeInfo() {
  Serial.println("Requesting Time Info");
  sendRequest("timeinfo");
}

void requestCurrent() {
  Serial.println("Requesting Current Info");
  sendRequest("current");
}

void requestNewQuote() {
  Serial.println("Requesting Quote Info");
  sendRequest("quote");
}

void requestTodoList() {
  Serial.println("Requesting Todo List");
  sendRequest("todo");
}

void requestDailyOblique() {
  Serial.println("Requesting Daily Oblique");
  sendRequest("obliques");
}

void requestDailyRiddle() {
  Serial.println("Requesting Daily Riddle");
  sendRequest("riddle");
}

void requestAQ() {
  Serial.println("Requesting Air Quality");
  sendRequest("airQuality");
}

void requestWeatherForecastsCall() {
  Serial.println("Requesting Weather Forecasts");
  sendRequest("allForecasts");
}




void setupWifiDisplay() {

  currentDisplay = WIFISETUP;

  display.fillScreen(BGCOLOR);
  display.setTextColor(TEXTCOLOR);
  delay(500);
  int startX = displayMarginW / 2;
  int startY = displayMarginH * 2 + 10;

  display.setCursor(startX, startY);
  display.setFont(&FreeSansBold12pt7b);
  display.print("Wi-Fi SETUP:");

  int spacer = 20;
  int majorSpacer = 30;
  int indent = 12;

  startY += majorSpacer;

  display.setCursor(startX, startY);
  display.setFont(&FreeSans9pt7b);
  display.println("-");
  display.setCursor(startX + indent, startY);
  display.print("Open the Wi-Fi settings on your phone");
  startY += spacer;
  display.setCursor(startX + indent, startY);
  display.print("and connect to this network:");
  startY += spacer;
  display.setCursor(startX + indent, startY);
  display.setFont(&FreeSansBold9pt7b);
  display.print(uniqId);
  display.setFont(&FreeSans9pt7b);

  startY += majorSpacer;
  display.setCursor(startX, startY);
  display.print("-");
  display.setCursor(startX + indent, startY);
  display.print("After you have connected, browse to");
  startY += spacer;
  display.setCursor(startX + indent, startY);
  display.print("to 192.168.4.1 (if needed)");


  startY += majorSpacer;
  display.setCursor(startX, startY);
  display.print("-");
  display.setCursor(startX + indent, startY);
  display.print("Click the 'Configure Wi-Fi' button");
  startY += spacer;
  display.setCursor(startX + indent, startY);
  display.print("and enter your Wi-Fi settings");

  display.refresh();

}

int getDeviceDetails() {

  int statusMsg = 0;

  const int httpsPort = 443;  //HTTPS= 443 and HTTP = 80
  WiFiClientSecure httpsClient;
  const size_t capacity = 2048;
  httpsClient.setInsecure();
  httpsClient.setTimeout(15000); // 15 Seconds
  delay(1000);

  Serial.println("HTTPS Connecting:");
  int r = 0; //retry counter
  while ((!httpsClient.connect(INIT_HOST, httpsPort)) && (r < 50)) {
    delay(100);
    Serial.print("-");
    r++;
  }
  StaticJsonDocument<50> doc;

  doc["request"] = "init";
  doc["deviceID"] = uniqId;

  String body;
  serializeJson(doc, body);
  int body_len = body.length();

  httpsClient.println("POST " + String(INIT_PATH) + " HTTP/1.1");
  //Headers
  httpsClient.print("Host: "); httpsClient.println(INIT_HOST);
  httpsClient.println("Content-Type: application/json");
  httpsClient.print("Content-Length: "); httpsClient.println(body_len);
  httpsClient.println("Connection: Close");
  httpsClient.println();

  httpsClient.println(body);
  httpsClient.println();

  Serial.println("request sent");


  // Check HTTP status
  char status[32] = {0};
  httpsClient.readBytesUntil('\r', status, sizeof(status));

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!httpsClient.find(endOfHeaders)) {
    Serial.println(F("Invalid response"));
    httpsClient.stop();
    return 1;
  }

  StaticJsonDocument<900> outdoc;

  DeserializationError error = deserializeJson(outdoc, httpsClient);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    httpsClient.stop();
    return 1;
  }

  // Extract values
  Serial.println(F("Response:"));

  const char* deviceID = outdoc["deviceID"];

  JsonObject location = outdoc["location"];
  lat = location["lat"];
  Serial.println(lat);

  lng = location["long"];
  Serial.println(lng);

  strcpy(city, location["city"]);
  Serial.println(city);

  strcpy(locale, location["localityName"]);
  Serial.println(locale);

  strcpy(timezone, outdoc["timezone"]);
  Serial.println(timezone);

  strcpy(tz_info, outdoc["tz_info"]);
  Serial.println(tz_info);

  strcpy(units, outdoc["units"]);
  Serial.println(units);

  if (strcmp(units, "imperial") == 0) {
    setDefaultMeasureOnWifi(IMPERIALMEASURE - 1);
  } else {
    setDefaultMeasureOnWifi(METRICMEASURE - 1);
  }

  strcpy(mqtt_server, outdoc["mqtt_server"]);
  Serial.println(mqtt_server);

  strcpy(mqtt_token, outdoc["mqtt_token"]);
  Serial.println(mqtt_token);

  strcpy(mqtt_user, outdoc["mqtt_user"]);
  Serial.println(mqtt_user);

  strcpy(mqtt_pub_topic, outdoc["mqtt_pub_topic"]);
  Serial.println(mqtt_pub_topic);

  latest_ver_major = outdoc["latest_ver_major"];
  latest_ver_minor = outdoc["latest_ver_minor"];
  latest_ver_patch = outdoc["latest_ver_patch"];

  checkLatestVersion();

  Serial.println(latest_ver_major);
  Serial.println(latest_ver_minor);
  Serial.println(latest_ver_patch);

  // Disconnect
  httpsClient.stop();

  return statusMsg;

}

// Variables to validate response from S3 for software upgrade
long contentLength = 0;
bool isValidContentType = false;
int upgradeStartX = 0;
int upgradeStartY = 0;

// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

void upgradeSoftwareDisplay(int message = 0) {

  currentDisplay = UPGRADE;
  int spacer = 20;
  int majorSpacer = 20;
  int indent = 10;
  boolean restart = false;

  switch (message) {
    case 0:
      display.fillScreen(BGCOLOR);
      display.setTextColor(TEXTCOLOR);
      delay(500);
      upgradeStartX = displayMarginW / 2;
      upgradeStartY = displayMarginH * 2 + 10;

      display.setCursor(upgradeStartX, upgradeStartY);
      display.setFont(&FreeSansBold12pt7b);
      display.print("UPGRADE NEWT:");

      upgradeStartY += majorSpacer;

      display.setCursor(upgradeStartX, upgradeStartY);
      display.setFont(&FreeSans9pt7b);
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Upgrading from "); display.print(SWVERSION_MAJOR); display.print("."); display.print(SWVERSION_MINOR); display.print("."); display.print(SWVERSION_PATCH);

      if (RC){
        display.print("-RC"); 
        display.print(SWVERSION_RC); 
      }
      
      display.print(" to "); display.print(latest_ver_major); display.print("."); display.print(latest_ver_minor); display.print("."); display.print(latest_ver_patch);
      display.refresh();
      break;
    case 1:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Attempting to connect to network");
      display.refresh();
      break;
    case 2:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Connection established");
      display.refresh();
      break;
    case 3:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Looking for latest update");
      display.refresh();
      break;
    case 4:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Download failed - timeout");
      delay(5000);
      display.refresh();

      restart = true;
      break;

    case 5:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Download failed - bad connection");
      delay(5000);
      display.refresh();

      restart = true;
      break;

    case 6:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Download failed - dropped connection");
      delay(5000);
      display.refresh();

      restart = true;
      break;

    case 7:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Download failed - missing data");
      delay(5000);
      display.refresh();

      restart = true;
      break;

    case 8:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Starting download (2-5 min wait)");
      display.refresh();
      break;

    case 9:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Update started");
      display.refresh();
      break;

    case 10:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Update failed - missing data");
      delay(5000);
      display.refresh();

      restart = true;
      break;

    case 11:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Update complete");
      delay(5000);
      display.refresh();

      restart = true;
      break;

    case 12:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Update failed - unknown");
      delay(5000);
      display.refresh();

      restart = true;
      break;

    case 13:
      display.setFont(&FreeSans9pt7b);
      upgradeStartY += majorSpacer;
      display.setCursor(upgradeStartX + indent, upgradeStartY);
      display.print("Update failed - no space");
      delay(5000);
      display.refresh();

      restart = true;
      break;
    default:
      break;
  }

  if (restart) {
    upgradeStartY += majorSpacer;
    display.setCursor(upgradeStartX + indent, upgradeStartY);
    display.print("Restarting device");
    delay(3000);
    display.refresh();
    ESP.restart();
  }

}

// OTA Logic
void execOTA() {

  upgradeSoftwareDisplay(3);
  if (client_upgrade.connect(UPGRADE_HOST, UPGRADE_PORT)) {
    // Connection Succeed. Now fecthing the bin
    client_upgrade.print(String("GET ") + UPGRADE_PATH + " HTTP/1.1\r\n" +
                         "Host: " + UPGRADE_HOST + "\r\n" +
                         "Cache-Control: no-cache\r\n" +
                         "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client_upgrade.available() == 0) {
      if (millis() - timeout > 9000) {
        client_upgrade.stop();
        upgradeSoftwareDisplay(4);
        return;
      }
    }
    // Once the response is available, check file

    while (client_upgrade.available()) {
      // read line till /n
      String line = client_upgrade.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();

      // if the the line is empty, this is end of headers break the while and feed the remaining `client_upgrade` to the Update.writeStream();
      if (!line.length()) {
        //headers ended
        break; // and get the OTA started
      }

      // Check if the HTTP Response is 200 else break and Exit Update
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          upgradeSoftwareDisplay(5);
          break;
        }
      }

      // extract headers here. Start with content length.
      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
      }

      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {
    // Connect to S3 failed.// Probably a choppy network?
    upgradeSoftwareDisplay(6);
    return;

  }

  // check contentLength and content type
  if (contentLength && isValidContentType) {
    // Check if there is enough to OTA Update
    bool canBegin = Update.begin(contentLength);

    // If yes, begin
    if (canBegin) {
      upgradeSoftwareDisplay(8);
      size_t written = Update.writeStream(client_upgrade);

      if (written == contentLength) { //successful
        upgradeSoftwareDisplay(9);
      } else { //failed
        upgradeSoftwareDisplay(10);
      }

      if (Update.end()) { //OTA complete
        if (Update.isFinished()) {
          upgradeSoftwareDisplay(11);
        } else {
          upgradeSoftwareDisplay(12);
        }
      } else {
        upgradeSoftwareDisplay(12);
      }
    } else {
      // not enough space to begin OTA. Understand the partitions and space availability
      client_upgrade.flush();
      upgradeSoftwareDisplay(13);
    }
  } else { //not enough content in the response
    client_upgrade.flush();
    upgradeSoftwareDisplay(7);
  }
}

void updateVersion() {

  upgradeSoftwareDisplay(0);

  WiFiClient client_upgrade;

  timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP_SETUP;

  upgradeSoftwareDisplay(1);
  WiFi.begin();
  Serial.println("attempting to connect to wifi for upgrades");
  // Wait for connection to establish
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }

  // Connection Succeed
  upgradeSoftwareDisplay(2);

  // Execute OTA Update
  execOTA();

}
