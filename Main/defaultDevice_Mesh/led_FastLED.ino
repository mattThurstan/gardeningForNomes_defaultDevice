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
  }
}

} // end ifdef
