/*----------------------------Breathing----------------------------*/

/*
 * Breath : (noun) Refers to a full cycle of breathing. It can also refer to the air that is inhaled or exhaled.
 */
 
// would eventually like a simple timeline screen on an app where you can draw curves in and see the results in realtime
//const unsigned long _breathRiseFallStepIntervalMillis = 250;  //156  62  139  2 //breath rise/fall step interval in milliseconds
//const int _breathRiseFallSpacer = 4;  //just 4 at begin.. eg. 4 each end.. //8   //_breathMaxBrightness / (4*2)     //counts to pause/ignore at the bottom and top - 42
//const int _breathMaxBrightness = 16;  //32
//const unsigned long _breathRiseFallHoldIntervalMillis = 1;    //breath rise/fall hold interval in milliseconds
//unsigned long _breathRiseFallPrevMillis = 0;                  //previous time for reference
//int _breathRiseFallCounter = 0;                               //eg. 0-17
//boolean _breathRiseFallDirection = true;                      //direction true-rise, false-fall
//CRGB c;
//RgbColor c;

void loopBreathing() {
  if (_isBreathing == true) {
    breathRiseFall2();
  } else {
    #ifdef (USING_FASTLED) {
      for (int i = 0; i < _ledNumOfStrips; i++) {
        fadeToBlackBy( _leds[i], _ledNum, 30); 
      } 
    }
    #ifdef(USING_NEOPIXELBRIGHTNESSBUS) { FadeAll(4); }
  }
}

uint8_t bBpm = 12;
uint8_t bMax = ( (ledSegment[2].total / 4) - 1 );
uint8_t bPeak;

void breathRiseFall2() {

  if (_isBreathOverlaid == false) {
    #ifdef(USING_FASTLED) { }
    #ifdef(USING_NEOPIXELBRIGHTNESSBUS) { strip.ClearTo(_rgbBlack); }
  }
  bPeak = beatsin8( bBpm, 1, bMax); //bpm, min, max
  
  //fade bot to top
  //ledsLeft( 1, bPeak ).fill_gradient_RGB( CRGB::White, CRGB::Black );

  //strip.SetPixelColor(i, _rgbWhite);
  #ifdef(USING_FASTLED) { 
    //eg. ledsLeft( 1, bPeak ).fill_gradient_RGB( CRGB::White, CRGB::Black );
    }
  #ifdef(USING_NEOPIXELBRIGHTNESSBUS) {
    FillGradientRGB(ledSegment[2].first, bPeak, _rgbWhite, _rgbBlack);  // left
    //FillGradientRGB((ledSegment[4].first+bPeak), ledSegment[4].total, _rgbBlack, _rgbWhite);  // right
  }
}
