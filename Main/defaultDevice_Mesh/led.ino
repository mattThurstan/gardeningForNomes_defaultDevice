/*----------------------------led----------------------------------*/

/*----------------------------led utils----------------------------*/

/*
 * Increment gHue by 0.1 every N milliseconds
 * Saved as a uint8_t (using 0-100) and converted to a float (0.0-1.0)
 */
void gHueRotate() {
  if (_modeCur == 1) {
    unsigned long _gHue2CurMillis = millis();
    if ( (unsigned long)(_gHue2CurMillis - _gHue2PrevMillis) >= _gHue2CycleMillis) {
      _gHue2PrevMillis = millis(); //re-initilize Timer
      _gHue2++;                                   // slowly cycle the "base color" through the rainbow
      checkAndSetColorHSL_H();
    }
  }
}

/*
 * Checks and resets _gHue2 bounds 0-100
 * Converts and sets main colour Hue
 */
void checkAndSetColorHSL_H() {
  if (_gHue2 > 100) { _gHue2 = 0; }           // rollover catch
  float tg = (float) _gHue2;
  tg = tg/100;
  _colorHSL.H = tg;
}

/*
 * Checks and resets LED rise speed bounds 0-255 (for the moment, change to something like 5-50)
 * Coverts and sets LED rise speed
 */
 void checkAndSetLedRiseSpeed() {
  if (_ledRiseSpeedSaved > 255) { _ledRiseSpeedSaved = 0; }
  if (_ledRiseSpeedSaved < 0) { _ledRiseSpeedSaved = 255; }
  _ledRiseSpeed = (unsigned long) _ledRiseSpeedSaved;
 }

/*
 * Checks and resets gHue2 saved cycle time 0-255
 * Converts and sets gHue2 cycle millis
 */
void checkAndSetGHue2CycleMillis() {
  if (_gHue2CycleSaved > 255) { _gHue2CycleSaved = 0; }
  if (_gHue2CycleSaved < 0) { _gHue2CycleSaved = 255; }
  _gHue2CycleMillis = (unsigned long) (_gHue2CycleSaved * _gHue2CycleMultiplier);
}

void setColorHSL(RgbColor rgb) {
  _colorHSL = rgb;
  _gHue2 = _colorHSL.H;
  //_gHue2saved = _colorHSL.H;
  
  if (DEBUG_GEN) { 
    Serial.print("setColorHSL - R ");
    Serial.print(rgb.R);
    Serial.print(", G ");
    Serial.print(rgb.G);
    Serial.print(", B ");
    Serial.println(rgb.B);
  }
}

/*
 * Color temperature utils
 */
void setColorTemp(int i) {
  _colorTempCur = i;
}
void cycleColorTemp() {
  _colorTempCur += 1;
  if (_colorTempCur >= _colorTempNum) 
  { _colorTempCur = 0; }  // rollover
}

/*
 * Global brightness utils
 */
void setGlobalBrightness(uint8_t gb) {
  //use this to achieve an override from the mesh, eg. to match levels
  _ledGlobalBrightnessCur = gb;
  brightnessRolloverCatch();
}
void increaseBrightness() {
  _ledGlobalBrightnessCur += _ledBrightnessIncDecAmount;
  brightnessRolloverCatch();
}
void decreaseBrightness() {
  _ledGlobalBrightnessCur -= _ledBrightnessIncDecAmount;
  brightnessRolloverCatch();
}
void brightnessRolloverCatch() {
  if(_ledGlobalBrightnessCur > 255) {
    _ledGlobalBrightnessCur = 255;
  } else if(_ledGlobalBrightnessCur < 0) {
    _ledGlobalBrightnessCur = 0;
  }
  #ifdef(USING_FASTLED) {
    FastLED.setBrightness(_ledGlobalBrightnessCur);      // set global brightness
  }
  #ifdef(USING_NEOPIXELBRIGHTNESSBUS) {
    strip.SetBrightness(_ledGlobalBrightnessCur);     // set global brightness
  }
}
