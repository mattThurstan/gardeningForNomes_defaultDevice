
/*
    'defaultDevice' by Thurstan. Default light network device.
    Copyright (C) 2020 MTS Standish (Thurstan|mattKsp)
    
    https://github.com/mattThurstan/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be usefull,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    
    https://github.com/mattThurstan/
    
    WeMos D1 (R2 &) mini ESP8266, 80 MHz, 115200 baud, 4M, (1M SPIFFS)
    //Wemos D32 Pro (ESP32-WROVER)
*/

#define USING_FASTLED 1
#define USING_NEOPIXELBRIGHTNESSBUS 0

/*----------------------------libraries----------------------------*/
#include <MT_LightControlDefines.h>
#include <FS.h>                                   // file system
#include <FastLED.h>                              // WS2812B LED strip control and effects
#ifdef{USING_NEOPIXELBRIGHTNESSBUS) { #include <NeoPixelBrightnessBus.h> }                 // NeoPixelBrightnessBus (just for ESP8266)- for brightness functions (instead of NeoPixelBus.h)
#include <painlessMesh.h>                         // https://github.com/gmag11/painlessMesh

/*----------------------------system----------------------------*/
const String _progName = "defaultDevice";         // default device
const String _progVers = "0.012";                 // tweaks

uint8_t LOCKDOWN_SEVERITY = 0;                    // the severity of the lockdown
bool LOCKDOWN = false;                            // are we in lockdown?

boolean DEBUG_GEN = false;                        // realtime serial debugging output - general
boolean DEBUG_OVERLAY = false;                    // show debug overlay on leds (eg. show segment endpoints, center, etc.)
boolean DEBUG_MESHSYNC = false;                   // show painless mesh sync by flashing some leds (no = count of active mesh nodes) 
boolean DEBUG_COMMS = false;                      // realtime serial debugging output - comms
boolean DEBUG_INTERRUPT = false;                  // realtime serial debugging output - interrupt pins
boolean DEBUG_USERINPUT = false;                  // realtime serial debugging output - user input

boolean _firstTimeSetupDone = false;              // starts false //this is mainly to catch an interrupt trigger that happens during setup, but is usefull for other things
//volatile boolean _onOff = true; /    /flip _state // issues with mqtt and init false // this should init false, then get activated by input - on/off true/false
bool _dayMode = false;                            // whether or not to run if night or day. default to night just so it works in case something goes wrong.
bool _shouldSaveSettings = false;                 // flag for saving data
bool _runonce = true;                             // flag for sending states when first mesh conection

#ifdef(USING_FASTLED) {
  #define UPDATES_PER_SECOND 120                  // main loop FastLED show delay
}
#ifdef(USING_NEOPIXELBRIGHTNESSBUS) {
  const int _mainLoopDelay = 0;                   // just in case. Eg. 8 ~= 120 fps (1000/120=8.333)
}
bool _isBreathing = false;                        // toggle for breath
bool _isBreathOverlaid = false;                   // toggle for whether breath is overlaid on top of modes
bool _isBreathingSynced = false;                  // breath sync local or global

/*----------------------------pins----------------------------*/
#ifdef(USING_FASTLED) {
  const int _ledDOut0Pin = 4;                     // DOut 0 -> LED strip 0 DIn   - right (short)
  const int _ledDOut1Pin = 0;                     // DOut 1 -> LED strip 1 DIn   - middle (long)
  const int _ledDOut2Pin = 2;                     // DOut 2 -> LED strip 2 DIn   - left (long)
  //const int _ledDOut3Pin = 15;                    // DOut 3 -> LED strip 3 DIn   - SPARE ..for future use
  //const int _ledDOut4Pin = 27;                    // DOut 4 -> LED strip 4 DIn   - SPARE
  //const int _ledDOut4Pin = 14;                    // DOut 4 -> LED strip 4 DIn   - SPARE
  //const int _ledDOut4Pin = 12;                    // DOut 4 -> LED strip 4 DIn   - SPARE
  //const int _ledDOut4Pin = 13;                    // DOut 4 -> LED strip 4 DIn   - SPARE

}
#ifdef(USING_NEOPIXELBRIGHTNESSBUS) {
  // NeoPixelBrightnessBus - For Esp8266, only one Pin and it uses GPIO3 (RX) due to DMA hardware use. 
}

// Wemos D32 (and Pro).
//const int _i2cInterrupt1Pin = 36;                 // I2C interrupt pin 1 - DS3231 - Sunrise/Sunset
//const int _i2cSDApin = 21;                        // SDA (21) - interrupt pin
//const int _i2cSCLpin = 22;                        // SCL (22) - interrupt pin

//const byte _pirPin[2] = { 4, 5 }; // D2, D1       // 2 PIR sensors on interrupt pins (triggered on HIGH)

/*----------------------------modes----------------------------*/
const int _modeNum = 2;                           // normal, cycle (gHue)
//const int _modePresetSlotNum = 7;
//int _modePreset[_modePresetSlotNum] = { 0, 2, 3, 4, 5, 7, 8 }; // test basic, tap bt to cycle around a few mode slots   //expand to array or struct later for more presets
volatile int _modeCur = 0;                        // current mode in use
//int _modePresetSlotCur = 3;                       // the current array pos (slot) in the current preset, as opposed to..      //+/- by userInput
const String _modeName[_modeNum] = { "Normal", "Cycle" };
//String _modeName[_modeNum] = { "Glow", "Sunrise", "Morning", "Day", "Working", "Evening", "Sunset", "Night", "Effect" };

const int _colorTempNum = 3;                      // 3 color temperature sub-modes for now
int _colorTempCur = 1;                            // current colour temperature
const String _colorTempName[_colorTempNum] = { "Warm", "Standard", "CoolWhite" }; // color temperature sub-mode names for the main "Working" mode.

/*----------------------------PIR----------------------------*/
/* 
  const unsigned long _pirHoldInterval = 10000; //150000; // 15000=15 sec. 30000=30 sec. 150000=2.5 mins.
  volatile byte _state = 0;                         // 0-Off, 1-Fade On, 2-On, 3-Fade Off
  volatile byte _stateSave = 0;                     // temp save state for inside for-loops
  //direction for fade on/off is determined by last pir triggered
  volatile unsigned long _pirHoldPrevMillis = 0;
  volatile byte _pirLastTriggered = 255;            // last PIR sensor triggered (0=top or 1=bottom)
  volatile boolean _PIRtriggeredTimerRunning = false;           // is the hold timer in use?
  volatile byte _fadeOnDirection = 255;             // direction for fade on loop. 0=fade down the stairs (top to bot), 1=fade up the stairs (bot to top).
  // crash at boot with ISR not in IRAM error
  void ICACHE_RAM_ATTR pirInterrupt0();
  void ICACHE_RAM_ATTR pirInterrupt1();
*/

/*----------------------------buttons----------------------------*/
/*----------------------------touch sensors----------------------------*/

/*----------------------------LED----------------------------*/
const uint16_t _ledNum = 101;                     // 1 + 100 LEDs
#ifdef(USING_FASTLED) {
  #define MAX_POWER_DRAW 900 // limit power draw to 0.9A at 5v (with 1A power supply this gives us a bit of head room for board, lights etc.)
}
#ifdef(USING_NEOPIXELBRIGHTNESSBUS) {
  NeoPixelBrightnessBus<NeoGrbFeature, Neo800KbpsMethod> strip(_ledNum);
}

typedef struct {
  byte first;
  byte last;
  byte total;                                     // using a byte here is ok as we haven't got more than 256 LEDs in a segment
} LED_SEGMENT;

const byte _segmentTotal = 2;                     // (1 + 1) runs down stair banister from top to bottom
LED_SEGMENT ledSegment[_segmentTotal] = {
  { 0, 0, 1 },  // sacrificial level changer
  { 1, 101, 100 }
};

#ifdef(USING_FASTLED) {
  CRGB _leds[_ledNum];                             // global RGB array
  //CRGB _leds[_ledNumOfStrips][_ledNumPerStrip];    // global RGB array matrix
  
  //CRGBArray<_ledNum> _leds;                       // master array - CRGBArray means can do multiple '_leds(0, 2).fadeToBlackBy(40);' as well as single '_leds[0].fadeToBlackBy(40);'
  //CRGBSet sacrificeLed( _leds(ledSegment[0].first, ledSegment[0].last) );
  //CRGBSet nextLeds( _leds(ledSegment[1].first, ledSegment[1].last) );
}

const int _ledGlobalBrightness = 255;             // global brightness - use this to cap the brightness
uint8_t _ledGlobalBrightnessCur = 255;            // current global brightness - adjust this
uint8_t _ledBrightnessIncDecAmount = 10;          // the brightness amount to increase or decrease
unsigned long _ledRiseSpeed = 30;                 // speed at which the LEDs turn on (runs backwards)
uint8_t _ledRiseSpeedSaved = 30;                  // cos of saving / casting unsigned long issues - use 0-255 via mqtt
uint8_t _gHue2 = 0;                               // incremental cycling "base color", 0-100, converted to 0-1
uint8_t _gHue2saved = 0;                          // used to revert color when going back to 'Normal' mode
unsigned long _gHue2CycleMillis = 200UL;          // gHue loop update time (millis)
uint8_t _gHue2CycleSaved = 100;                   // 0-255 mapped to millis range
uint8_t _gHue2CycleMultiplier = 4;                // (__gHue2CycleSaved * _gHue2CycleMultiplier) = (unsigned long) _gHue2CycleMillis
unsigned long _gHue2PrevMillis;                   // gHue loop previous time (millis)

#ifdef(USING_FASTLED) {
  CHSV  _colorHSV(0.25f, 0.5f, 0.5f);
}
#ifdef(USING_NEOPIXELBRIGHTNESSBUS) {
  HslColor _colorHSL(0.25f, 0.5f, 0.5f);
}

RgbColor _glowColor(32, 32, 32);
RgbColor _morningColor(255, 255, 0);
RgbColor _dayColor(135,206,235);
RgbColor _eveningColor(128, 64, 64);
RgbColor _nightColor(64, 64, 64);

// RGB colours for "Working" colour temperature sub-mode
RgbColor _rgbWarmFluorescent(255, 244, 229);      // WarmFluorescent = 0xFFF4E5 - 0 K, 255, 244, 229
RgbColor _rgbStandardFluorescent(244, 255, 250);  // StandardFluorescent = 0xF4FFFA - 0 K, 244, 255, 250
RgbColor _rgbCoolWhiteFluorescent(212, 235, 255); // CoolWhiteFluorescent = 0xD4EBFF - 0 K, 212, 235, 255
//RgbColor _rgbColorTempCur(_rgbStandardFluorescent); // use this one in day-to-day operations

/*----------------------------Mesh----------------------------*/
painlessMesh  mesh;                               // initialise
uint32_t id_bridge1 = DEVICE_ID_BRIDGE1;

void receivedCallback(uint32_t from, String &msg ) {
  if (DEBUG_COMMS && Serial) { Serial.printf("defaultDevice_Mesh: Received from %u msg=%s\n", from, msg.c_str()); }
  receiveMessage(from, msg);
}

void newConnectionCallback(uint32_t nodeId) {
  //if (_runonce == true) {
    publishStatusAll(false);
    //_runonce = false;
  //}
  if (DEBUG_COMMS && Serial) { Serial.printf("--> defaultDevice_Mesh: New Connection, nodeId = %u\n", nodeId); }
}

void changedConnectionCallback() {
  //publish..
  //publishStatusAll(false);
  if (DEBUG_COMMS && Serial) { Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str()); }
}

void nodeTimeAdjustedCallback(int32_t offset) {
  if (DEBUG_COMMS && Serial) { Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset); }
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  if (DEBUG_COMMS && Serial) { Serial.printf("Delay to node %u is %d us\n", from, delay); }
}


/*----------------------------MAIN----------------------------*/
void setup() {
  #ifdef(USING_NEOPIXELBRIGHTNESSBUS) {
    // LED strip - Wemos D1 - GPIO 3 (RX) - swap the pin from serial to a GPIO.
    pinMode(3, FUNCTION_3); // FUNCTION_0 = swap back
  }
  
  // start serial regardless but control debug output from mqtt
  Serial.begin(115200);
  
  Serial.println();
  Serial.print( F("Welcome to Gardening For Nomes part ") );
  Serial.print(_progVers);
  Serial.print( F("!") ); 
  Serial.println();
  Serial.print( F("Your host for todays proceedings is.. ") );
  Serial.print(_progName);
  Serial.println();
  Serial.print("..");
  Serial.println();
  
  delay(3000);                                    // give the power, LED strip, etc. a couple of secs to stabilise
  
  loadSettings();
  
  setupLEDs();
  flashLED(0);
/*  
  setupPIR();
  flashLED(1);
*/  
  setupMesh();
  flashLED(2);

  //everything done? ok then..
  Serial.print(F("Setup done"));
  Serial.println("-----");
  Serial.print(F("Device Node ID is "));
  String s = String(mesh.getNodeId());
  Serial.println(s);
  Serial.println("-----");
  Serial.println("");

  flashLED(7);
  delay(1500);
  #ifdef(USING_FASTLED) {
    FastLED.clear();
  }
  #ifdef(USING_NEOPIXELBRIGHTNESSBUS) {
    strip.ClearTo(_rgbBlack);
  }
}

void loop() {
  
  mesh.update();
  
  if(_firstTimeSetupDone == false) {
    _firstTimeSetupDone = true;                   
    publishStatusAll(false);
    if (DEBUG_GEN && Serial) { Serial.print(F("firstTimeSetupDone  = true")); }
  }

  //loopUserInputs();                               // loop direct user input
  //loopPir();
  loopModes();
  loopBreathing();                                // overlaid on top, ..cos
  gHueRotate();
  
  showSegmentEndpoints();                         // overlay debug
  if (DEBUG_MESHSYNC) { }
 
  EVERY_N_SECONDS(60) {                           // too much ???
    if (_shouldSaveSettings == true)
    { 
      saveSettings(); 
      _shouldSaveSettings = false; 
    }
  }
  //factoryReset();              //TODO             // Press and hold the button to reset to factory defaults

  #ifdef(USING_FASTLED) {
    FastLED.show();                               // send all the data to the strips
    FastLED.delay(1000 / UPDATES_PER_SECOND);
  }
  #ifdef(USING_NEOPIXELBRIGHTNESSBUS) {
    strip.Show();
    delay(_mainLoopDelay); 
  }
}
