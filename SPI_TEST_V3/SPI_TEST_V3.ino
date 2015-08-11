#include <SPI.h>

// SPI Speed and mode settings
SPISettings mode0(1000000, MSBFIRST, SPI_MODE0);
SPISettings mode1(1000000, MSBFIRST, SPI_MODE1);
SPISettings mode2(1000000, MSBFIRST, SPI_MODE2);
SPISettings mode3(1000000, MSBFIRST, SPI_MODE3);

// Define SPI Slave Select Pins
const int SS_DAC_1 = 6;
const int SS_ADC_1 = 7;
const int SS_DAC_2 = 8;
const int SS_ADC_2 = 9;

const uint8_t DATA_PIN = 4;
const uint8_t SS_RESET = 0, SS_NEGPOT = 1, SS_FPOT = 2, SS_FGEN = 3;
const uint8_t SS_TFT1DC = 4, SS_SD = 5, SS_TFT2DC = 6, SS_EXTRA = 7;

void setup() {

  SPI.begin();   // Initialize SPI
  
  PORTB |= 0b00111011;  // sets PB5:0 to HIGH or LOW output
  DDRB |= 0b00111111;   // sets PB5:0 to OUTPUT, PB7:6(XTAL) are left alone
  PORTC |= 0b0110000;   // sets PC5:0 to HIGH or LOW output
  DDRC |= 0b0110000;    // sets PC3:0 to INPUT, PC5:4 to OUTPUT, PC6 is RESET pin and is left alone
  PORTD |= 0b11000000;  // sets PD7:0 to HIGH or LOW output
  DDRD |= 0b11111100;   // sets PD7:2 to OUTPUT and PD1(RX):0(TX) are left alone
  
  
  TCCR1A = _BV(COM1B0);              //toggle OC1B on compare match
  OCR1A = 7;                         //top value for counter (1 for 4.00396MHz, 7 for 1.000975MHz) 
  TCCR1B = _BV(WGM12) | _BV(CS10);   //CTC mode, prescaler clock/1
}

void loop() {
  for (int i=0; i<256; i++) {
    writeNegPot(i);
    delay(100);
  }
}


/*
 * Function Name:   slaveRegister(int slaveBit)
 * 
 * Description:
 *      Writes the proper slave bit
 * 
 * Params:
 *      int slaveBit - the pin that will go low on the slave register
 *      
 * Notes:
 * 
 */
void slaveRegister(uint8_t slaveBit) {
  SPI.end();
  uint8_t slaveByte = 0xFF;
  if(slaveBit == 1){
    slaveByte = 0xFE;
  }else if(slaveBit == 2){
    slaveByte = 0xFD;
  }else if(slaveBit == 3){
    slaveByte = 0xFB;
  }else if(slaveBit == 4){
    slaveByte = 0xF7;
  }else if(slaveBit == 5){
    slaveByte = 0xEF;
  }else if(slaveBit == 6){
    slaveByte = 0xDF;
  }else if(slaveBit == 7){
    slaveByte = 0xBF;
  }else if(slaveBit == 8){
    slaveByte = 0x7F;
  }else{
    slaveByte = 0xFF;
  }
  
  shiftOut(dataPin, SCK, MSBFIRST, slaveByte);
  PORTD |= _BV(2);   // toggle latchPin HIGH
  PORTD &= ~_BV(2);   // toggle latchPin LOW
  SPI.begin();
}

/*
 * Function Name:   writeNegitiveDigitalPot()
 * 
 * Description:
 *      Writes a value to the AD5290 digital Pot
 * 
 * Params:
 * 
 * Notes:
 * 
 */
void writeNegPot(uint8_t value){
  slaveRegister(ssNegPot);        // Write ssNegPot low
  PORTD |= _BV(4);           // toggle dataPin HIGH
  SPI.beginTransaction(mode0);
  SPI.transfer(value);
  SPI.endTransaction();
  PORTD |= _BV(2);   // toggle latchPin HIGH
  PORTD &= ~_BV(2);   // toggle latchPin LOW
  PORTD &= ~_BV(4);   // toggle dataPin LOW
  
}


