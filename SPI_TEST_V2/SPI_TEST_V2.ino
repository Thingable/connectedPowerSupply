#include <SPI.h>
//#include "TimerOne.h"
//#include <avr/io.h>
//#include <avr/interrupt.h>

// Define SPI Slave Select Pins
const int SS_DAC_1 = 6;
const int SS_ADC_1 = 7;
const int SS_DAC_2 = 8;
const int SS_ADC_2 = 9;

const int dataPin = 4;
const int clockPin = 13;
const int SHIFT_LATCH = 2;


// SPI Speed and mode settings
SPISettings mode0(1000000, MSBFIRST, SPI_MODE0);
SPISettings mode1(1000000, MSBFIRST, SPI_MODE1);
SPISettings mode2(1000000, MSBFIRST, SPI_MODE2);
SPISettings mode3(1000000, MSBFIRST, SPI_MODE3);

void setup() {
  //Timer1.initialize(1);
  //Timer1.pwm(10, 512);
  


  
  // Set SS to Output
  pinMode(SS_DAC_1, OUTPUT);
  pinMode(SS_ADC_1, OUTPUT);
  pinMode(SS_DAC_2, OUTPUT);
  pinMode(SS_ADC_2, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(10, OUTPUT);

  analogWrite(10, 127);

  // Initialize SPI
  SPI.begin();

  //Initialize DACs
  initDACs();

  //Initialize Freq Gen
  writeFreqGen(4000);

  writeFreqDigitalPot(100);
  
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  //slaveRegister(3);
  //latch();

  //setDAC(1, 2); // Set DAC_2 to 4.8 Volts
  
  /*
  for(int i = 0; i <= 255; i++){
    //writeNegitiveDigitalPot(i);
    writeFreqDigitalPot(i);
    Serial.println(i);
    delay(100);
  }
  */
  
  //writeFreqDigitalPot(8);

  writeFreqGen(2000);

  
  //Serial.println();
  delay(1000);
  
}

/*
 * Function Name:   initDACs()
 * 
 * Description:
 *      Initializes both DACs by writing the proper values to command register
 * 
 * Params:
 * 
 * Notes:
 * 
 */
void initDACs(){
  SPI.beginTransaction(mode1);
  digitalWrite(SS_DAC_1, LOW);
  SPI.transfer(0b01000000);
  SPI.transfer(0b10000000);
  SPI.transfer(0b0000000);
  digitalWrite(SS_DAC_1, HIGH);
  SPI.endTransaction();
  
  SPI.beginTransaction(mode1);
  digitalWrite(SS_DAC_2, LOW);
  SPI.transfer(0b01000000);
  SPI.transfer(0b10000000);
  SPI.transfer(0b0000000);
  digitalWrite(SS_DAC_2, HIGH);
  SPI.endTransaction();
}

/*
 * Function Name:  setDAC()
 *      
 * Description:
 *      Sets the specified voltage for the specified channel
 *      
 * Params:
 *      voltage -     the voltage value to be set
 *      DAC_Number -  the channel number of the DAC of interest
 *      
 * Notes:
 *      Currently does not write the specified voltage, only writes 4.8 Volts
 */
void setDAC(double voltage, int DAC_Number){
  // Convert voltage to 16 bit DAC value (1 bit = 0.000076293945313 Volts)
  float floatingDAC_Value = (voltage / 0.000076293945313) + 0.5; // Approximate voltage 
  uint32_t DAC_Value = floatingDAC_Value;
 
  // DAC command bits
  uint32_t writeInput=     0b00010000; // Write input register
  uint32_t updateDAC =     0b00100000; // Update DAC
  uint32_t writeDACInput = 0x00030000; // Write both DAC and input register
  uint32_t writeControl =  0x00040000; // Write control register

  // Combine command and DAC value and break up 24 bits into 3 bytes
  //uint32_t SPI_Command = writeDACInput << 8; // Write Command data and shift 12 bits
  //Serial.println(SPI_Command);
  uint32_t SPI_Command = writeDACInput | DAC_Value;                   // Bitwise Or DAC value
  SPI_Command <<= 4;

  uint8_t Byte[3] = {0x00, 0x00, 0x00};       // Make byte array initialize to 0
  Byte[0] = (SPI_Command >> 16);  // Write first byte
  Byte[1] = (SPI_Command >> 8);   // Write second byte
  Byte[2] = (SPI_Command);        // Write third byte

  SPI.beginTransaction(mode1);
  if(DAC_Number == 1){
    digitalWrite(SS_DAC_1, LOW);
    //SPI.transfer(Byte[0]);
    //SPI.transfer(Byte[2]);
    //SPI.transfer(Byte[3]);
    SPI.transfer(0b00111111);
    SPI.transfer(0b01011100);
    SPI.transfer(0b1111000);
    digitalWrite(SS_DAC_1, HIGH);
  }else if(DAC_Number == 2){
    digitalWrite(SS_DAC_2, LOW);
    //SPI.transfer(Byte[0]);
    //SPI.transfer(Byte[2]);
    //SPI.transfer(Byte[3]);
    SPI.transfer(0b00111000);
    SPI.transfer(0b00000000);
    SPI.transfer(0b00001000);
    digitalWrite(SS_DAC_2, HIGH);
  }else{
    Serial.println("Nope Chuck Testa");
  }
  SPI.endTransaction();
  
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
void slaveRegister(int slaveBit){
  SPI.end();
  
  delay(10);
  uint8_t slaveByte = 0xFF;
  if(slaveBit == 0){
    slaveByte = 0xFE;
  }else if(slaveBit == 1){
    slaveByte = 0xFD;
  }else if(slaveBit == 2){
    slaveByte = 0xFB;
  }else if(slaveBit == 3){
    slaveByte = 0xF7;
  }else if(slaveBit == 4){
    slaveByte = 0xFE;
  }else if(slaveBit == 5){
    slaveByte = 0xDF;
  }else if(slaveBit == 6){
    slaveByte = 0xBF;
  }else if(slaveBit == 7){
    slaveByte = 0x7F;
  }else{
    slaveByte = 0xFF;
  }
  Serial.println(slaveByte, BIN);
  digitalWrite(SHIFT_LATCH, LOW);
  
  shiftOut(dataPin, clockPin, MSBFIRST, slaveByte);

  SPI.begin();
}

/*
 * Function Name:   latch()
 * 
 * Description:
 *      latches the shift register 
 * 
 * Params:
 * 
 * Notes:
 *      this is required because the shift register and mosi devices share a clock pin
 */
void latch(){
  digitalWrite(SHIFT_LATCH, HIGH);
  
  digitalWrite(SHIFT_LATCH, LOW);
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
void writeNegitiveDigitalPot(uint8_t value){
  slaveRegister(1);               //Write SS 1 low
  latch();
  digitalWrite(dataPin, HIGH);
  SPI.beginTransaction(mode0);
  SPI.transfer(value);
  SPI.endTransaction();
  latch();
  digitalWrite(dataPin, LOW);
  
}

/*
 * Function Name:   writeFreqGen()
 * 
 * Description:
 *      Writes frequency and phase to the AD9833
 * 
 * Params:
 *      
 * Notes:
 * 
 */
void writeFreqGen(long frequency){
  int MSB;
  int LSB;
  int phase = 0;

  //We can't just send the actual frequency, we have to calculate the "frequency word".
  //This amounts to ((desired frequency)/(reference frequency)) x 0x10000000.
  //calculated_freq_word will hold the calculated result.
  long calculated_freq_word;
  float AD9833Val = 0.00000000;

  AD9833Val = (((float)(frequency))/16000000);
  calculated_freq_word = AD9833Val*0x10000000;
  Serial.println(frequency);
  Serial.println(calculated_freq_word);  

  //Once we've got that, we split it up into separate bytes.
  MSB = (int)((calculated_freq_word & 0xFFFC000)>>14); //14 bits
  LSB = (int)(calculated_freq_word & 0x3FFF);

  //Set control bits DB15 ande DB14 to 0 and one, respectively, for frequency register 0
  LSB |= 0x4000;
  MSB |= 0x4000;
 
  phase &= 0xC000;
  
  slaveRegister(3);
  latch();
  
  WriteRegisterAD9833(0x2100); // Write command register

  //Set the frequency==========================
  WriteRegisterAD9833(LSB); //lower 14 bits
  WriteRegisterAD9833(MSB); //upper 14 bits
  WriteRegisterAD9833(phase); //mid-low

  digitalWrite(dataPin, HIGH);
  
  //Power it back up
  //WriteRegisterAD9833(0x2020); //square
  //WriteRegisterAD9833(0x2000); //sin
  WriteRegisterAD9833(0x2002); //triangle 

  latch();
  digitalWrite(dataPin, LOW);
}

/*
 * Function Name:   WriteRegisterAD9833()
 * 
 * Description:
 *      Writes data to the AD9833 Frequency Generator
 * 
 * Params:
 *     int dat - the data to be written 
 *     
 * Notes:
 *     
 */
void WriteRegisterAD9833(int dat){
  SPI.beginTransaction(mode2);
  SPI.transfer(highByte(dat));
  SPI.transfer(lowByte(dat));
  SPI.endTransaction();
}  

/*
 * Function Name:   writeFreqDigitalPot()
 * 
 * Description:
 *      Writes a value to the MCP4151 digital Pot for frequency amplitude
 * 
 * Params:
 *      int level - an 8 bit number to set the resistance
 *      
 * Notes:
 * 
 */
void writeFreqDigitalPot(int level){
  slaveRegister(2);
  latch();
  digitalWrite(dataPin, HIGH);
  SPI.beginTransaction(mode0);
  SPI.transfer(0);      //Choose the register to write to
  SPI.transfer(level);  //Set the level (0-255)
  SPI.endTransaction();
  latch();
  digitalWrite(dataPin, LOW);
}
