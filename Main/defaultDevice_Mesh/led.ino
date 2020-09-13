/*----------------------------led----------------------------------*/

/*----------------------------led utils----------------------------*/

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
