/*----------------------------NeoPixelBrightnessBus----------------*/
#ifdef(USING_NEOPIXELBRIGHTNESSBUS) {
/*----------------------------setup NeoPixelBrightnessBus----------*/
void setupLEDs() {
  
  strip.Begin();                                  // NeoPixelBus
  strip.Show();                                   // NeoPixelBus

  _ledGlobalBrightnessCur = _ledGlobalBrightness;
  
  //uint8_t oldBrightness = strip.GetBrightness();
  strip.SetBrightness(_ledGlobalBrightnessCur);
  
  checkAndSetLedRiseSpeed();
  _gHue2saved = _gHue2;
  checkAndSetGHue2CycleMillis();
}

/*----------------------------NeoPixelBrightnessBus utils---------*/
void flashLED(byte led) {
  
  if (led == 0) {
    //
  } else if (led == 1) {
    strip.SetPixelColor(1, _rgbYellow);
  } else if (led == 2) {
    strip.SetPixelColor(1, _rgbFuchsia);
  } else if (led == 3) {
    strip.SetPixelColor(1, _rgbOrange);
  } else if (led == 4) {
    strip.SetPixelColor(1, _rgbViolet);
  } else if (led == 5) {
    strip.SetPixelColor(1, _rgbTeal);
  } else if (led == 6) {
    strip.SetPixelColor(1, _rgbPink);
  } else if (led == 7) {
    strip.SetPixelColor(1, _rgbGreen);
  }
  strip.Show();
  delay(400);
}

/*
 Fade all leds except for the sacrificial one in the box
 LED '0' is used for debug porpoises
 */
void FadeAll(uint8_t darkenBy) {
    RgbColor color;
    for (uint16_t i = 1; i < strip.PixelCount(); i++)
    {
        color = strip.GetPixelColor(i);
        if (color.R > 0 && color.G > 0 && color.B > 0) {
          color.Darken(darkenBy);
        } else {
          color = _rgbBlack;
        }
        strip.SetPixelColor(i, color);
    }
}

/*
 A gradient of colour from one end of a strip to the other
 */
void FillGradientRGB(byte first, byte total, RgbColor colA, RgbColor colB) {
  for (uint16_t indexPixel = 0; indexPixel < total; indexPixel++)
    {
      //map number of used pixels to 0-100
      //mapf();
      int ti = map(indexPixel, 0, (total+1), 0, 100);
      
      //divide by 100 for 0.0-1.0
      float tf = ti/100.0;
      
      RgbColor updatedColor = RgbColor::LinearBlend(colA, colB, tf);
      
      strip.SetPixelColor( (indexPixel + first), updatedColor);
      
      if (DEBUG_GEN && Serial) { 
        Serial.print("SetGradient - i "); 
        Serial.print(indexPixel); 
        Serial.print(", ti - "); 
        Serial.print(ti); 
        Serial.print(", tf - "); 
        Serial.print(tf); 
        Serial.print(", col - "); 
        Serial.print(updatedColor.R); 
        Serial.print(", "); 
        Serial.print(updatedColor.G); 
        Serial.print(", "); 
        Serial.print(updatedColor.B); 
        Serial.println(); 
      }
    }
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
 * Set HSL colour
 */
void setColorHSL(RgbColor rgb) {
  _colorHSL = rgb;
  _gHue2 = _colorHSL.H;
  //_gHue2saved = _colorHSL.H;
  
  if (DEBUG_GEN && Serial) { 
    Serial.print("setColorHSL - R ");
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
    strip.ClearTo(_rgbBlack);
  
    strip.SetPixelColor(ledSegment[1].first, _rgbRed);
    strip.SetPixelColor(ledSegment[1].last, _rgbRed);
    //
    //
  } 
  else {
    strip.SetPixelColor(0, _rgbBlack);          // modes are responsible for all other leds
  }
}

} // end ifdef
