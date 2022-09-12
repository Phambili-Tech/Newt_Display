int selectedTodo = 0;
int todoMax = 0;

void renderTodoDisplay(bool syncInProgress = false) {

  Serial.print("selected todo number: ");
  Serial.println(selectedTodo);

  currentDisplay = TODO;
  display.fillScreen(BGCOLOR);

  display.setTextColor(TEXTCOLOR);
  display.setFont(&FreeSansBold12pt7b);
  display.getTextBounds("TODO LIST", 0, 0, &xP, &yP, &w, &h);
  display.setCursor(displayMarginW, displayMarginH + h);
  display.print("TODO LIST");

  File todoJSON = SPIFFS.open("/todolist.json", FILE_READ);
  int totalItems = -1;

  if (!todoJSON) {
    Serial.println("The file does not exist");
    // return;
  } else {
    size_t capacity = todoJSON.size() * 3;
    if ( capacity == 0 ) {
      Serial.println("The file is empty");
      //return;
    } else {
      Serial.println("Getting data");
      DynamicJsonDocument doc(capacity);
      DeserializationError err = deserializeJson(doc, todoJSON);

      todoJSON.close();
      //serializeJson(doc, Serial);

      if (err) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(err.c_str());
        return;
      }

      JsonArray todos =  doc.as<JsonArray>();
      int startCounter = 0;
      int limit = 5;

      int x = displayMarginW * 1.5;
      int y = 50;
      int yTab = 15;
      //int totalItems = -1;
      int startShowNum = 0;

      if (selectedTodo < limit) {
        limit = limit * 1;
      } else if (selectedTodo < (limit * 2)) {
        startShowNum = limit;
        limit = limit * 2;
      } else if (selectedTodo <= (limit * 3)) {
        startShowNum = limit * 2;
        limit = limit * 3;
      } else {
        startShowNum = limit * 3;
        limit = limit * 4;
      }

      Serial.print("limit: ");
      Serial.println(limit);

      Serial.print("startShowNum: ");
      Serial.println(startShowNum);

      Serial.print("startCounter: ");
      Serial.println(startCounter);

      for (JsonObject item : doc.as<JsonArray>()) {

        //long updated = item["updated"]; // 1630763022, 1630762815, 1630762551, 1630762302, 1630762231, ...
        const char* todo = item["value"]; // "Feel", "Star", "Heatwave", "Detective", "Test", "Central", ...
        //const char* id = item["id"]; // "25edb112-61d2-4bdf-ae5c-0ec2cd498f76", ...

        if (startCounter >= startShowNum && startCounter < limit) {
          display.setFont(&FreeSans9pt7b);
          display.getTextBounds("X", 0, 0, &xP, &yP, &w, &h);
          y = y + h;
          display.setCursor(x, y);
          display.setFont(&slateIcons9pt7b);
          display.print(checkboxuncheckedG);

          if (startCounter == selectedTodo) {
            display.setFont(&FreeSansBold9pt7b);
            Serial.print("Selected: ");
            Serial.println(todo);
          } else {
            display.setFont(&FreeSans9pt7b);
          }

          display.print(" ");
          display.print(todo);
          y = y + yTab;
        }

        startCounter++;
        totalItems++;

      }

      todoMax = totalItems;

      // Handle Menu
    }
  }

  if (syncInProgress) { //maybe this needs to be in the upper right....
    display.setFont(&slateIcons9pt7b);
    display.getTextBounds(refreshG, 0, 0, &xP, &yP, &w, &h);
    display.setCursor(displayMarginW, BUTTON_CDEF_PIN_Y - h);
    display.print(refreshG);
    display.setFont(&FreeSansBold9pt7b);
    display.print(" Synicng TODO List");

    timeoutCounter = millis() + MILLS_BEFORE_DEEP_SLEEP;
    if (syncInProgress) {
      shouldConnect = true;
      requestTodos = true;
    }


  } else {

    display.setTextColor(TEXTCOLOR);

    //display.setFont(&slateIcons14pt7b);
    display.setFont(&FreeSansBold9pt7b);
    display.getTextBounds(stopwatchG, 0, 0, &xP, &yP, &w, &h);

    if (selectedTodo > 0) {
      display.setCursor(BUTTON_C_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
      display.print("BACK");
    }

    if (selectedTodo != totalItems) {
      display.setCursor(BUTTON_D_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
      display.print("NEXT");
    }

    display.setCursor(BUTTON_E_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("CHECK");

    display.setFont(&FreeSansBold9pt7b);

    //display.getTextBounds("EXIT", 0, 0, &xP, &yP, &w, &h);
    display.setCursor(BUTTON_F_PIN_X - (w / 2), BUTTON_CDEF_PIN_Y);
    display.print("EXIT");

  }

  display.refresh();
}

void handleTodoList(int button) {

  switch (button)
  {
    case BUTTON_C :
      if (selectedTodo > 0) {
        selectedTodo--;
        renderTodoDisplay();
      }
      break;
    case BUTTON_D :
      if (selectedTodo < todoMax) {
        selectedTodo++;
        renderTodoDisplay();
      }
      break;
    case BUTTON_E :
      //recordHabit();
      break;
    case BUTTON_F :
      //timeoutCounter = 0;
      exitScreensRoutine();
      //renderMainDisplay();
      break;
  }
  return;
}
