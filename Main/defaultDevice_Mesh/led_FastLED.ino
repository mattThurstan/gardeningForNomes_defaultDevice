/*----------------------------FastLED------------------------------*/
#ifdef(USING_FASTLED) {
/*----------------------------setup FastLED------------------------*/
void setupLEDs() {
  
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_DRAW);

  FastLED.addLeds<WS2812B, _ledDOutPin0, GRB>(_leds, 0, _ledNum).setCorrection( TypicalSMD5050 );  //all lights 1 loop
  //FastLED.addLeds<WS2812B, _ledDOut0Pin, GRB>(_leds[0], _ledNumPerStrip).setCorrection( TypicalSMD5050 );
  //FastLED.addLeds<WS2812B, _ledDOut0Pin, GRB>(_leds, ledSegment[0].first, ledSegment[1].last).setCorrection( TypicalSMD5050 );
  
  _ledGlobalBrightnessCur = _ledGlobalBrightness;
  
  FastLED.setBrightness(_ledGlobalBrightnessCur);       //set global brightness
  FastLED.setTemperature(UncorrectedTemperature);       //set first temperature

  checkAndSetLedRiseSpeed();
  _gHue2saved = _gHue2;
  checkAndSetGHue2CycleMillis();
}

/*----------------------------FastLED utils-----------------------*/
void flashLED(byte led) {
  if (led == 0) {
    //
  } else if (led == 1) {
    _leds[1] = CRGB::Yellow;
  } else if (led == 2) {
    _leds[2] = CRGB::Fuchsia;
  } else if (led == 3) {
    _leds[3] = CRGB::Orange;
  } else if (led == 4) {
    _leds[4] = CRGB::Violet;
  } else if (led == 5) {
    _leds[5] = CRGB::Teal;
  } else if (led == 6) {
    _leds[6] = CRGB::Pink;
  } else if (led == 7) {
    _leds[7] = CRGB::Green;
  }
  FastLED.show();
  delay(400);
}

/*
 * Increment gHue by 0.1 every N milliseconds
 * Saved as a uint8_t (using 0-100) and converted to a float (0.0-1.0)
 */
void gHueRotate() {
  if (_modeCur == 8) {
    unsigned long _gHue2CurMillis = millis();
    if ( (unsigned long)(_gHue2CurMillis - _gHue2PrevMillis) >= _gHue2CycleMillis) {
      _gHue2PrevMillis = millis(); //re-initilize Timer
      _gHue2++;                                   // slowly cycle the "base color" through the rainbow
      checkAndSetColorHSV_H();
    }
  }
}

/*
 * Checks and resets _gHue2 bounds 0-100
 * Converts and sets main colour Hue
 */
void checkAndSetColorHSV_H() {
  if (_gHue2 > 100) { _gHue2 = 0; }           // rollover catch
  float tg = (float) _gHue2;
  tg = tg/100;
  _colorHSV.H = tg;
}

/*
 * Set HSV colour
 */
void setColorHSV(CRGB rgb) {
  _colorHSV = rgb;
  _gHue2 = _colorHSV.H;
  //_gHue2saved = _colorHSV.H;
  
  if (DEBUG_GEN && Serial) { 
    Serial.print("setColorHSV - R ");
    Serial.print(rgb.R);
    Serial.print(", G ");
    Serial.print(rgb.G);
    Serial.print(", B ");
    Serial.println(rgb.B);
  }
}

/*
//called from 'pir'
void fadeShowLEDs(byte low, byte high) {
  uint16_t l = low;
  uint16_t h = high;
  h += 1;
  
  for (uint16_t i = l; i < h; i++)
  {
    strip.SetPixelColor(i, _colorHSL);
  }
  strip.Show();
  delay(_ledRiseSpeed);
}
*/

/*
 Check Segment Endpoints
 usage | showSegmentEndpoints();
 overlay/place near end of chain
 This places different coloured lights at the ends of the LED strip(s) segments for quick visual feedback of calculations
 */
void showSegmentEndpoints() {
  if (DEBUG_OVERLAY) {
    _leds[ledSegment[1].first]= CRGB::Red;
    _leds[ledSegment[1].last]= CRGB::Red;
    //
    //
  } else {
    _led[0] = _rgbBlack;          // modes are responsible for all other leds 
  }
}

} // end ifdef
