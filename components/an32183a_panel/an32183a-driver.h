//Library for the an32183a led driver

//Important: the internal pull-up resistors  for the I2C (TWI) have to be turned off in the Wire library. 
// The chessboard pcb has 2 pull-ups (4.7Kohm) already 
#ifndef __ANDRIVER_H
#define __ANDRIVER_H


#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/i2c/i2c.h"
#include <Arduino.h>
#include <Adafruit_GFX.h>

// i2c address -> datasheet p40
enum I2CAddress : uint8_t {
  I2CAddressLOW =   0b1011100,
  I2CAddressHIGH =  0b1011101,
  I2CAddressSCL =   0b1011110,
  I2CAddressSDA =   0b1011111,
};

enum Register : uint8_t {
  // Registers -> datasheet p15
  RST = 0x02,          // 'RAM' reset or 'soft reset' TODO can't understand what these resets are actually resetting
  POWERCNT = 0x02,     // Set internal oscilator
  OPTION = 0x04,       // Ghost Image Prevention, External Melody Input, Internal clock output, Internal/external synchronous clock
  MTXON = 0x05,        // Maximum led current
  PWMEN1 = 0x06,       // PWM enable PWMEN1 - PWMEN11 0x06 - 0x10
  MLDEN1 = 0x11,       // MLDEN1 - MLDEN11 0x11 - 0x1B
  MLDMODE1 = 0x2A,     
  THOLD = 0x2B,
  CONSTX6_1 = 0x2C,
  CONSTX10_7 = 0x2D,   // Constant current settings for matrix row X9 -> X7
  CONSTY6_1 = 0x2E,    // Constant current settings for matrix column Y6 -> Y1
  CONSTY9_7 = 0x2F,    // Constant current settings for matrix column Y6 -> Y1
  MASKY6_1 = 0x30,     // Constant current mask setting for matrix column X6 -> X1
  MASKY9_7 = 0x31,
  SLPTIME = 0x32,      // Fade in-out settings
  MLDCOM = 0x33,
  SCANSET = 0x36,      // ??? (Not used)
  DTA1 = 0x40,         // Pwm duty control. The next registers are the same until 0x90 (81 registers)
  DTA2 = 0x41,
  DTA3 = 0x42,
  DTA4 = 0x43,
  DTA5 = 0x44,
  DTA6 = 0x45,
  DTA7 = 0x46,
  DTA8 = 0x47,
  DTA9 = 0x48,
  DTB1 = 0x49,
  DTB2 = 0x4a,
  DTB3 = 0x4b,
  DTB4 = 0x4c,
  DTB5 = 0x4d,
  DTB6 = 0x4e,
  DTB7 = 0x4f,
  DTB8 = 0x50,
  DTB9 = 0x51,
  DTC1 = 0x52,
  DTC2 = 0x53,
  DTC3 = 0x54,
  DTC4 = 0x55,
  DTC5 = 0x56,
  DTC6 = 0x57,
  DTC7 = 0x58,
  DTC8 = 0x59,
  DTC9 = 0x5a,
  DTD1 = 0x5b,
  DTD2 = 0x5c,
  DTD3 = 0x5d,
  DTD4 = 0x5e,
  DTD5 = 0x5f,
  DTD6 = 0x60,
  DTD7 = 0x61,
  DTD8 = 0x62,
  DTD9 = 0x63,
  DTE1 = 0x64,
  DTE2 = 0x65,
  DTE3 = 0x66,
  DTE4 = 0x67,
  DTE5 = 0x68,
  DTE6 = 0x69,
  DTE7 = 0x6a,
  DTE8 = 0x6b,
  DTE9 = 0x6c,
  DTF1 = 0x6d,
  DTF2 = 0x6e,
  DTF3 = 0x6f,
  DTF4 = 0x70,
  DTF5 = 0x71,
  DTF6 = 0x72,
  DTF7 = 0x73,
  DTF8 = 0x74,
  DTF9 = 0x75,
  DTG1 = 0x76,
  DTG2 = 0x77,
  DTG3 = 0x78,
  DTG4 = 0x79,
  DTG5 = 0x7a,
  DTG6 = 0x7b,
  DTG7 = 0x7c,
  DTG8 = 0x7d,
  DTG9 = 0x7e,
  DTH1 = 0x7f,
  DTH2 = 0x80,
  DTH3 = 0x81,
  DTH4 = 0x82,
  DTH5 = 0x83,
  DTH6 = 0x84,
  DTH7 = 0x85,
  DTH8 = 0x86,
  DTH9 = 0x87,
  DTI1 = 0x88,
  DTI2 = 0x89,
  DTI3 = 0x8a,
  DTI4 = 0x8b,
  DTI5 = 0x8c,
  DTI6 = 0x8d,
  DTI7 = 0x8e,
  DTI8 = 0x8f,
  DTI9 = 0x90,
  LED_A1 = 0x91,       // Luminance + fading setup.  The next registers are the same until 0xE1 (81 registers)
  LED_A2 = 0x92,
  LED_A3 = 0x93,
  LED_A4 = 0x94,
  LED_A5 = 0x95,
  LED_A6 = 0x96,
  LED_A7 = 0x97,
  LED_A8 = 0x98,
  LED_A9 = 0x99,
  LED_B1 = 0x9a,
  LED_B2 = 0x9b,
  LED_B3 = 0x9c,
  LED_B4 = 0x9d,
  LED_B5 = 0x9e,
  LED_B6 = 0x9f,
  LED_B7 = 0xa0,
  LED_B8 = 0xa1,
  LED_B9 = 0xa2,
  LED_C1 = 0xa3,
  LED_C2 = 0xa4,
  LED_C3 = 0xa5,
  LED_C4 = 0xa6,
  LED_C5 = 0xa7,
  LED_C6 = 0xa8,
  LED_C7 = 0xa9,
  LED_C8 = 0xaa,
  LED_C9 = 0xab,
  LED_D1 = 0xac,
  LED_D2 = 0xad,
  LED_D3 = 0xae,
  LED_D4 = 0xaf,
  LED_D5 = 0xb0,
  LED_D6 = 0xb1,
  LED_D7 = 0xb2,
  LED_D8 = 0xb3,
  LED_D9 = 0xb4,
  LED_E1 = 0xb5,
  LED_E2 = 0xb6,
  LED_E3 = 0xb7,
  LED_E4 = 0xb8,
  LED_E5 = 0xb9,
  LED_E6 = 0xba,
  LED_E7 = 0xbb,
  LED_E8 = 0xbc,
  LED_E9 = 0xbd,
  LED_F1 = 0xbe,
  LED_F2 = 0xbf,
  LED_F3 = 0xc0,
  LED_F4 = 0xc1,
  LED_F5 = 0xc2,
  LED_F6 = 0xc3,
  LED_F7 = 0xc4,
  LED_F8 = 0xc5,
  LED_F9 = 0xc6,
  LED_G1 = 0xc7,
  LED_G2 = 0xc8,
  LED_G3 = 0xc9,
  LED_G4 = 0xca,
  LED_G5 = 0xcb,
  LED_G6 = 0xcc,
  LED_G7 = 0xcd,
  LED_G8 = 0xce,
  LED_G9 = 0xcf,
  LED_H1 = 0xd0,
  LED_H2 = 0xd1,
  LED_H3 = 0xd2,
  LED_H4 = 0xd3,
  LED_H5 = 0xd4,
  LED_H6 = 0xd5,
  LED_H7 = 0xd6,
  LED_H8 = 0xd7,
  LED_H9 = 0xd8,
  LED_I1 = 0xd9,
  LED_I2 = 0xda,
  LED_I3 = 0xdb,
  LED_I4 = 0xdc,
  LED_I5 = 0xdd,
  LED_I6 = 0xde,
  LED_I7 = 0xdf,
  LED_I8 = 0xe0,
  LED_I9 = 0xe1
};

enum RegisterDefaults : uint8_t {
  RST_DEFAULT = 0x00,
  POWERCNT_DEFAULT = 0x00,
  OPTION_DEFAULT = 0x00,
  MTXON_DEFAULT = 0x1E,   // 11110 -> IMAX: 60mA, MTXON: 0
  PWMEN_DEFAULT = 0x00,
  MLDEN_DEFAULT = 0x00,
  MLDMODE1_DEFAULT = 0x00,
  THOLD_DEFAULT = 0x00,
  CONSTX6_1_DEFAULT = 0x00,
  CONSTX10_7_DEFAULT = 0x00,
  CONSTY6_1_DEFAULT = 0x00,
  CONSTY9_7_DEFAULT = 0x00,
  MASKY6_1_DEFAULT = 0x00,
  MASKY9_7_DEFAULT = 0x00,
  SLPTIME_DEFAULT = 0x00,
  MLDCOM_DEFAULT = 0x03,    // 11 -> MLDCOM: 5.8µs
  SCANSET_DEFAULT = 0x08,   // 1000 -> scanset: Scan all columns/Fade delay multiplier = 0.999 -> datasheet p37
  DT_DEFAULT = 0x00,
  LED_DEFAULT = 0x00
};

struct Led{
  uint8_t A1 = 0;
};

// Objects holding registry options. All settings have the same default value as the datasheet except for 'maxluminence' 
struct OptionsSettings{  // OPTION registers -> datasheet p23
  bool ghostPrevention = false;
  bool melodyMode = false;
  bool clkOut = false;
  bool extClk = false;
};

struct SlpTimeSettings{  // Control fade timings -> datasheet p34
  bool slowFadeout = false;
  uint8_t ledOnExtend = 0;  
  uint8_t ledOffExtend = 0;
};

struct ChipSettings{
  bool internalOscillator = true;
  OptionsSettings optionsSettings;
  uint8_t maxLuminence = 7;
  bool pwmMode = true;
  uint8_t treshold = 0;
  SlpTimeSettings slpTimeSettings;
  uint8_t scanset = 8;
};

// Fucntions are ordered the same way as in the datasheet
class AN32183A : public Adafruit_GFX{
  public:
      AN32183A(uint8_t nrstPin, esphome::i2c::I2CDevice *i2c_dev);
      void init(uint8_t nrstPin);
      void reset(bool ramrst = true, bool srst = true);
      void toggleMatrix(bool active);
      void setPwmDuty(Register ledNr, uint8_t value);
      void writeLEDValues(uint8_t values[9][9]);
      void setMatrixLuminance(int luminance);
      void setLedLuminance(Register ledNr, uint8_t brta);
      void setLedFadeTime(Register ledNr, uint8_t sdt);
      uint8_t getRegister(Register reg);
      void led_setup();
      void clear(void);
      void drawPixel(int16_t x, int16_t y, uint16_t color);
      void writeDisplay(void);

    private:
      void setInternalOscillator(bool oscen);
      void setOptions(OptionsSettings settings);
      void setMaxLuminence(uint8_t imax);
      void setPwmMode(bool pwmActive);
      void setTreshold(uint8_t treshold);
      void setFadeoptions(SlpTimeSettings settings);
      void setScanset(uint8_t scanset);
      uint8_t readRegister(Register reg);
      void writeToRegister(Register reg, uint8_t value);
      void writePwmDuty(Register reg, uint8_t value);
      void multiWriteToRegister(Register firstReg, uint8_t [], uint8_t arraySize);
      void writeToRegister(uint8_t reg, uint8_t value);
      
    private:
      esphome::i2c::I2CDevice *i2c_dev_;
      uint8_t _nrstPin;
};
#endif