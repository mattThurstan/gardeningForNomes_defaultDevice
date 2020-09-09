/* 
 * WeMos D1 (R2 &) mini, 80 MHz, 115200 baud, 4M, (1M SPIFFS)
 * 
 * Pinout Wemos D1 Mini (ESP-8266)
 * RX  3 (GPIO3) [ NeoPixelBus - For Esp8266, the Pin is omitted and it uses GPIO3 (RX) due to DMA hardware use.  ]
 * TX  1
 * A0  Analog input, max 3.2V input  A0
 * D0  16  IO  GPIO16   - no PWM or I2C or interrupt
 * D1  5  IO, SCL GPIO5  I2C
 * D2  4  IO, SDA GPIO4  I2C
 * D3  0  IO, 10k Pull-up GPIO0
 * D4  2  IO, 10k Pull-up, BUILTIN_LED  GPIO2
 * D5  14  IO, SCK GPIO14
 * D6  12  IO, MISO  GPIO12
 * D7  13  IO, MOSI  GPIO13
 * D8  15  IO, 10k Pull-down, SS GPIO15
 * 
 * All have external interrupts except GPIO16 (D0)
 *
 * 
 * http://wiki.keyestudio.com/index.php/Ks0052_keyestudio_PIR_Motion_Sensor
 * 
 * LED strips
 * 
 */
 
/*
 * Wemos D32 (and Pro), 80 MHz (..says 240 on the box?), 4M
 * https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
 * 
 * Pinout Wemos D32 (ESP32-WROOM)
 * 
 * 36 - VP (RTC_GPIO0 interrupt)
 * 39 - VN
 * 34 - ADC1_CH6
 * 32 - ADC1_CH4 / TFT_LED
 * 33 - ADC1_CH5 / TFT_RST
 * 25 - DAC_1 (Analog Output)
 * 26 - DAC_2 (Analog Output)
 * 27 - TFT_DC
 * 14 - TFT_CS
 * 12 - TS_CS
 * 13
 * 
 * 23 - MOSI
 * 22 - SCL
 * 1 - TX
 * 3 - RX
 * 21 - SDA
 * 19 - MISO
 * 18 - SCK
 * 5 - SS
 * 17 - NC
 * 16 - NC
 * 4
 * 0
 * 2
 * 15
 * 
 * Pinout Wemos D32 Pro (ESP32-WROVER)
 * 
 * 36 - VP (RTC_GPIO0 interrupt)
 * 39 - VN
 * 34 - ADC1_CH6
 * 32 - ADC1_CH4 / TFT_LED
 * 33 - ADC1_CH5 / TFT_RST
 * 25 - DAC_1 (Analog Output)
 * 26 - DAC_2 (Analog Output)
 * 27 - TFT_DC
 * 14 - TFT_CS
 * 12 - TS_CS
 * 13
 * 
 * 23 - MOSI
 * 22 - SCL
 * 1 - TX
 * 3 - RX
 * 21 - SDA
 * 19 - MISO
 * 18 - SCK
 * 5 - SS
 * NC
 * NC
 * 4
 * 0
 * 2
 * 15
 * 
 */

/*
 * Home Assistant config example.
 * 
 *   light:
 *     - platform: mqtt
 *       name: 'Stairs lights'
 *       state_topic: 'house/stairs/lights/light/status'
 *       command_topic: 'house/stairs/lights/light/switch'
 *       state_value_template: "{{ value_json.state }}"
 *       brightness_state_topic: 'house/stairs/lights/brightness/status'
 *       brightness_command_topic: 'house/stairs/lights/brightness/set'
 *       brightness_value_template: "{{ value_json.brightness }}"
 *       hue_state_topic: 'house/stairs/lights/hue/status'
 *       hue_command_topic: 'house/stairs/lights/hue/set'
 *       rgb_state_topic: 'house/stairs/lights/rgb/status'
 *       rgb_command_topic: 'house/stairs/lights/rgb/set'
 *       rgb_value_template: "{{ value_json.rgb | join(,) }}"
 *       qos: 0
 *       payload_on: "ON"
 *       payload_off: "OFF"
 *       optimistic: false
 *       
 */
 
