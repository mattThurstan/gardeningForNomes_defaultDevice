/*----------------------------memory----------------------------*/
void loadSettings()
{
    //read configuration from FS json
    if (DEBUG_GEN && Serial) { Serial.println("mounting FS..."); }

    if (!SPIFFS.begin()) {
        if (DEBUG_GEN && Serial) { Serial.println("failed to mount FS"); }
        return;
    }
    
    if (DEBUG_GEN && Serial) { Serial.println("mounted file system"); }

    if (!SPIFFS.exists("/settings.json")) {
      setDefaults();
      _shouldSaveSettings = true;
    }
    else {
      //file exists, reading and loading
      if (DEBUG_GEN && Serial) { Serial.println("reading user settings file"); }
      File configFile = SPIFFS.open("/settings.json", "r");
      DynamicJsonDocument jsonDoc(512);
      const size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);

      DeserializationError error = deserializeJson(jsonDoc, buf.get());
      if (DEBUG_GEN && Serial) { serializeJson(jsonDoc, Serial); /* print output */ }
      if (error) {
        if (DEBUG_GEN) { Serial.println("failed to load json user settings (using defaults)"); }
        setDefaults();
        _shouldSaveSettings = true;
      } 
      else { 
        if (DEBUG_GEN && Serial) { Serial.println("loaded user settings file"); }
        if (DEBUG_GEN && Serial) { Serial.println("\nparsed json"); }

        _ledGlobalBrightnessCur = jsonDoc["gBrightnessCur"];
        _ledRiseSpeedSaved = jsonDoc["ledRiseSpeedSaved"];
        checkAndSetLedRiseSpeed();
        _gHue2CycleSaved = jsonDoc["gHue2CycleSaved"];
        checkAndSetGHue2CycleMillis();
        _colorHSL.H = jsonDoc["colorHSL_H"];
        _colorHSL.S = jsonDoc["colorHSL_S"];
        _colorHSL.L = jsonDoc["colorHSL_L"];
      }
      
      configFile.close();
    }
    
}

void saveSettings()
{
  if (DEBUG_GEN && Serial) { Serial.println("saving user settings"); }
  
  if (!SPIFFS.begin()) {
      if (DEBUG_GEN && Serial) { Serial.println("failed to mount FS"); }
      return;
  }

  SPIFFS.remove("/settings.json");
  
  File settingsFile = SPIFFS.open("/settings.json", "w");
  //File settingsFile = SPIFFS.open("/settings.json", FILE_WRITE);
  
  if (!settingsFile) { 
    if (DEBUG_GEN && Serial) { Serial.println("Error opening user settings file for writing"); }
  }

  DynamicJsonDocument jsonDoc(512);
  JsonObject json = jsonDoc.to<JsonObject>();
  
  //_pirHoldInterval
  json["gBrightnessCur"] = _ledGlobalBrightnessCur;
  json["ledRiseSpeedSaved"] = _ledRiseSpeedSaved;
  json["gHue2CycleSaved"] = _gHue2CycleSaved;
  json["colorHSL_H"] = _colorHSL.H;
  json["colorHSL_S"] = _colorHSL.S;
  json["colorHSL_L"] = _colorHSL.L;

  if (DEBUG_GEN && Serial) { 
    serializeJson(jsonDoc, Serial);       // output to serial
  }
  
  serializeJson(jsonDoc, settingsFile);   // output to file
  settingsFile.close();
}

void resetDefaults() {
  setDefaults();
  saveSettings();
}

void setDefaults() 
{
  DEBUG_GEN = false;
  DEBUG_OVERLAY = false;
  DEBUG_MESHSYNC = false;
  DEBUG_COMMS = false;
  DEBUG_INTERRUPT = false;
  DEBUG_USERINPUT = false;
  
  _isBreathing = false;
  _isBreathOverlaid = false;
  _isBreathingSynced = false; 

  _dayMode = false; 
  
  _modeCur = 0;
  _state = 0;

  _ledGlobalBrightnessCur = 255; 
  _ledBrightnessIncDecAmount = 10;
  _ledRiseSpeedSaved = 25;
  _gHue2CycleSaved = 50;
  _colorHSL.H = 0.25f;
  _colorHSL.S = 0.5f;
  _colorHSL.L = 0.5f;
}
