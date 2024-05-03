#include "an32183-panel.h"

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

void init(uint8_t p_nrst);


AN32183A::AN32183A(uint8_t nrstPin)
    : Adafruit_GFX(9, 9){
    _i2cAddress = I2CAddressLOW;
    _nrstPin = nrstPin;
    init(nrstPin);
}

/*
Flow:
check operation modes -> datasheet p38
MODES:
    * Pmw mode
    * Blinking mode
    * constant current mode
NOT IMPLEMENTED MODES:
    * Melody mode
    * Masks

START:
    AN32183A::begin
        RST: do full reset (??? TODO Not completely figured out how the reset works)
        POWERCNT: set to internal oscillator
        OPTION: set options
        THLOD -> THOLD: set treshold value
        MTXON -> MTXON: activate matrix
        MXTON -> IMAX: set led max luminence
        PWMEN -> PWMEN: enable pwm mode for each led if necesary
        SLPTIME -> SLPTIME: set fade options
        SCANSET -> SCANSET: set scanset

CONSTANT CURRENT MODE:
    set luminece setting for individual leds:
        AN32183A::setLedLuminance
            LED_A1 (+80) -> BRTA: set max luminance (relative to MXTON -> IMAX)

PWM MODE:
    when passing an OptionsSettings object to led.begin() -> set OptionsSettings.pwmMode = true
    set luminece setting for individual leds: same as in constant current mode
    control leds with AN32183A::setPwmDuty
        DTA1 (+80) -> DTA1: set pwm duty cycle -> datasheet p36

BLINKING MODE:
    make sure OptionsSettings.pwmMode = false
    set fadeTime setting:
        AN32183A::setLedFadeTime
            LED_A1 (+80) -> SDT*: set fade options -> datasheet p37
    set luminece setting for individual leds: same as in constant current mode
*/
void AN32183A::init(uint8_t nrstPin){
    Wire.begin();
    Wire.setClock(1000000);
    ChipSettings settings;
    settings.pwmMode = true; // Banana PWM mode
    settings.scanset = 8; // Monkey scan settings
    settings.slpTimeSettings.slowFadeout = true; // Monkey sleep time settings
    settings.slpTimeSettings.ledOnExtend = 1; // Monkey sleep time settings
    pinMode(_nrstPin, OUTPUT);
    digitalWrite(_nrstPin, HIGH);
    delayMicroseconds(5000); // Start up led driver and wait >4 ms (datasheet P14)
    reset();
    setInternalOscillator(settings.internalOscillator);
    setOptions(settings.optionsSettings);
    setTreshold(settings.treshold);
    toggleMatrix(true);
    setMaxLuminence(settings.maxLuminence);
    setPwmMode(settings.pwmMode);
    setFadeoptions(settings.slpTimeSettings);
    setScanset(settings.scanset);
    setMatrixLuminance(5); // Set monkey LED brightness
}

void AN32183A::clear(void) {
  uint8_t erasebuf[81];
  memset(erasebuf, 0, 81);
  uint8_t size = 81;
  multiWriteToRegister(DTA1, erasebuf, size);
}

// Full reset resets all registers. RAMRST only resets the pwm duty and led intensity settings. (datasheet -> p 22) 
void AN32183A::reset(bool ramrst, bool srst){
    uint8_t value = RST_DEFAULT || (ramrst << 1);
    value |= srst;
    writeToRegister(RST, value);  
}

void AN32183A::toggleMatrix(bool active){
    uint8_t value = readRegister(MTXON);
    active ? value |= 1 : value &= 0b00011110;
    writeToRegister(MTXON, value);
}

void AN32183A::setPwmDuty(Register ledNr, uint8_t value){
    if (ledNr < DTA1) return;
    if (ledNr > DTI9) return;
    writeToRegister(ledNr, value);
}

void AN32183A::writeLEDValues(uint8_t values[9][9]) {
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
      // Calculate the register address based on row and column
      Register registerAddr = static_cast<Register>(DTA1 + row * 9 + col);
      // Set the PWM duty cycle for the LED at current row and column
      setPwmDuty(registerAddr, values[row][col]);
    }
  }
}

void AN32183A::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if (color > 255)
    color = 255;
  if(y > 8 || y < 0) return;
  if(x > 8 || x < 0) return;
  Register registerAddr = static_cast<Register>(DTA1 + y * 9 + x);
  setPwmDuty(registerAddr, color);
}


void AN32183A::setMatrixLuminance(int luminance) {
  for (int row = 0; row < 9; row++) {
    for (int col = 0; col < 9; col++) {
      // Calculate the register address based on row and column
      Register registerAddr = static_cast<Register>(LED_A1 + row * 9 + col);
      // Set the luminance for the LED at current row and column
      Serial.print("Setting Register:");
      Serial.println(registerAddr,HEX);
      setLedLuminance(registerAddr, luminance);
    }
  }
}

void AN32183A::setLedLuminance(Register ledNr, uint8_t brta){
    if (ledNr < LED_A1) ledNr = LED_A1;
    if (ledNr > LED_I9) ledNr = LED_I9;
    if (brta > 15) brta = 15;
    uint8_t bitsOn = brta << 4;
    uint8_t bitsOff = brta << 4 | 0b00001111;  // OR with bitmask: set bits to be ignored to 1
    uint8_t value = readRegister(ledNr);
    value |= bitsOn;
    value &= bitsOff;
    writeToRegister(ledNr, value);
}

void AN32183A::setLedFadeTime(Register ledNr, uint8_t sdt){
    if (ledNr < LED_A1) ledNr = LED_A1;
    if (ledNr > LED_I9) ledNr = LED_I9;
    if (sdt > 7) sdt = 7;
    uint8_t bitsOn = sdt;
    uint8_t bitsOff = sdt | 0b11111000;  // OR with bitmask: set bits to be ignored to 1
    uint8_t value = readRegister(ledNr);
    value |= bitsOn;
    value &= bitsOff;
    writeToRegister(ledNr, value);
}

uint8_t AN32183A::getRegister(Register reg){
    return readRegister(reg);
}

// Private
void AN32183A::setInternalOscillator(bool oscen){
    writeToRegister(POWERCNT, oscen);
}

void AN32183A::setOptions(OptionsSettings settings){
    uint8_t value = settings.ghostPrevention << 3 | settings.melodyMode << 2 | settings.clkOut << 1 | settings.extClk;
    writeToRegister(OPTION, value);
}

void AN32183A::setMaxLuminence(uint8_t imax){
    if (imax > 7) imax = 7;  // -> datasheet p23 
    uint8_t bitsOn = imax << 1;
    uint8_t bitsOff = imax << 1 | 0b11110001;  // OR with bitmask: set bits to be ignored to 1
    uint8_t value = readRegister(MTXON);
    value |= bitsOn;
    value &= bitsOff;
    writeToRegister(MTXON, value);
}

void AN32183A::setPwmMode(bool pmwMode){
    uint8_t size = 11;
    uint8_t values[size] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    if (pmwMode == true){
        for (int i = 0; i < 11 - 1; i++){
            values[i] = 0b11111111;
        }
        values[10] = 0b00000001;  // last PMWEN register only uses one bit
    }

    multiWriteToRegister(PWMEN1, values, size);
}


void AN32183A::setTreshold(uint8_t treshold){
    // THOLD register can only hold one HIGH bit. All LOW = 'auto mode' (datasheet p27)
    uint8_t validOptions[9] = {0, 1, 2, 4, 8, 16, 32, 64, 128};
    for (int i = 0; i < 9; i++){
        if (validOptions[i] == treshold) writeToRegister(THOLD, treshold);
    } 
}

void AN32183A::setFadeoptions(SlpTimeSettings settings){
    if (settings.ledOnExtend > 3) settings.ledOnExtend = 3;
    if (settings.ledOffExtend > 3) settings.ledOffExtend = 3;
    uint8_t value = settings.slowFadeout << 4 | settings.ledOnExtend << 2 | settings.ledOffExtend;
    writeToRegister(SLPTIME, value);
}

void AN32183A::setScanset(uint8_t scanset){
    if (scanset > 8) scanset = 8;
    writeToRegister(SCANSET, scanset);
}

uint8_t AN32183A::readRegister(Register reg){
    Wire.beginTransmission(_i2cAddress);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom((int)_i2cAddress, 1);
    while (Wire.available() == 0);  // Wait for incoming data
    int incoming = Wire.read();
    return incoming;
}

void AN32183A::writeToRegister(Register reg, uint8_t value){
    Wire.beginTransmission(_i2cAddress);
    Wire.write(reg);
    Wire.write(value);
    Wire.endTransmission();
}

void AN32183A::multiWriteToRegister(Register firstReg, uint8_t values[], uint8_t arraySize){
    Wire.beginTransmission(_i2cAddress);
    Wire.write(firstReg);
    for (int i = 0; i < arraySize; i++){
        Wire.write(values[i]);
    }
    Wire.endTransmission();
}
