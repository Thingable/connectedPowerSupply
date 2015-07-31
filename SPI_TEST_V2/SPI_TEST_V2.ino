#include <SPI.h>

int dataPin = 4;
int clockPin = 13;
const int SHIFT_LATCH = 2;

// SPI Speed and mode settings
SPISettings mode0(4000000, MSBFIRST, SPI_MODE0);
SPISettings mode1(4000000, MSBFIRST, SPI_MODE1);
SPISettings mode2(4000000, MSBFIRST, SPI_MODE2);
SPISettings mode3(4000000, MSBFIRST, SPI_MODE3);

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);

  // Initialize SPI
  SPI.begin();
  
  Serial.begin(9600);
  delay(1000);
}

void loop() {
  //slaveRegister(3);
  //writeNegitiveDigitalPot(250);
  writeFreqGen(4000);
  delay(1000);
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
  
  digitalWrite(SHIFT_LATCH, HIGH);
  delayMicroseconds(10);
  digitalWrite(SHIFT_LATCH, LOW);

  SPI.begin();
  delay(10);
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
  
  SPI.beginTransaction(mode1);
  slaveRegister(1);               //Write SS 1 low
  SPI.transfer(value);
  Serial.println(value, BIN);
  slaveRegister(8);               //Write all bits high
  SPI.endTransaction();
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
  
  WriteRegisterAD9833(0x2100); // Write command register

  //Set the frequency==========================
  WriteRegisterAD9833(LSB); //lower 14 bits
  WriteRegisterAD9833(MSB); //upper 14 bits
  WriteRegisterAD9833(phase); //mid-low
 
  //Power it back up
  WriteRegisterAD9833(0x2020); //square
  //WriteRegisterAD9833(0x2000); //sin
  //WriteRegisterAD9833(0x2002); //triangle 

   slaveRegister(8);
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
