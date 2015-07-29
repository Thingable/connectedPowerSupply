#include <SPI.h>
#include <math.h>

// Define SPI Slave Select Pins
const int SS_DAC_1 = 6;
const int SS_ADC_1 = 7;
const int SS_DAC_2 = 8;
const int SS_ADC_2 = 9;

const int SHIFT_LATCH = 2;

// SPI Speed and mode settings
SPISettings mode0(8000000, MSBFIRST, SPI_MODE0);
SPISettings mode1(8000000, MSBFIRST, SPI_MODE1);
SPISettings mode2(8000000, MSBFIRST, SPI_MODE2);

void setup() {
  // Set SS to Output
  pinMode(SS_DAC_1, OUTPUT);
  pinMode(SS_ADC_1, OUTPUT);
  pinMode(SS_DAC_2, OUTPUT);
  pinMode(SS_ADC_2, OUTPUT);
  pinMode(SHIFT_LATCH, OUTPUT);

  // Initialize SPI
  SPI.begin();

  //Initialize DACs
  initDACs();
  
  // Initialize Debug Serial
  Serial.begin(9600);

  
  
  slaveRegister(8);
}

void loop() {
  
  //setDAC(4.8, 2); // Set DAC_2 to 4.8 Volts
  //readADC();
  //writeNegitiveDigitalPot();
  //writeFreqDigitalPot(150); // 8 bit value
  //writeFreqGen(frequency);  // 
  //Serial.println();
  //delay(5000);
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
    SPI.transfer(0b00111111);
    SPI.transfer(0b01011100);
    SPI.transfer(0b1111000);
    digitalWrite(SS_DAC_2, HIGH);
  }else{
    Serial.println("Nope Chuck Testa");
  }
  SPI.endTransaction();
  
}

/* 
 * Function Name:   readADC()
 * 
 * Description:
 *      Reads both ADC registers and temperature and prints values to the screen    
 *      
 * Params:
 * 
 * Notes:
 *      Math is wrong needs tweeking
 */
void readADC(){
  //Create Byte Arrays
  uint8_t ain0Byte[2] = {0x00, 0x00};
  uint8_t ain2Byte[2] = {0x00, 0x00};
  uint8_t temperatureByte[2] = {0x00, 0x00};
  uint8_t commandByte[2] = {0x00, 0x00};

  // Initialize AIN0 Read
  SPI.beginTransaction(mode1);
  digitalWrite(SS_ADC_2, LOW);
  SPI.transfer(0b00010000);
  SPI.transfer(0b10000011);
  commandByte[0] = SPI.transfer(0);
  commandByte[1] = SPI.transfer(0);
  digitalWrite(SS_ADC_2, HIGH);
  SPI.endTransaction();

  // Should be the same as the above command^^
  Serial.println("Voltage Command");
  Serial.println(commandByte[0], BIN);
  Serial.println(commandByte[1], BIN);
  Serial.println();
  
  // Initialize AIN2 Read AIN0 and Command
  SPI.beginTransaction(mode1);
  digitalWrite(SS_ADC_2, LOW);
  ain0Byte[0] = SPI.transfer(0b00110110);
  ain0Byte[1] = SPI.transfer(0b10000011);
  commandByte[0] = SPI.transfer(0);
  commandByte[1] = SPI.transfer(0);
  digitalWrite(SS_ADC_2, HIGH);
  SPI.endTransaction();

  // Should be the same as the above command^^
  Serial.println("Current Command");
  Serial.println(commandByte[0], BIN);
  Serial.println(commandByte[1], BIN);
  Serial.println();
  
  // Initialize Temperature Read AIN2 and Command
  SPI.beginTransaction(mode1);
  digitalWrite(SS_ADC_2, LOW);
  ain2Byte[0] = SPI.transfer(0b00000000);
  ain2Byte[1] = SPI.transfer(0b00010011);
  commandByte[0] = SPI.transfer(0);
  commandByte[1] = SPI.transfer(0);
  digitalWrite(SS_ADC_2, HIGH);
  SPI.endTransaction();

  // Should be the same as the above command^^
  Serial.println("Temp Command");
  Serial.println(commandByte[0], BIN);
  Serial.println(commandByte[1], BIN);
  Serial.println();
  
  // Read Temperature
  SPI.beginTransaction(mode1);
  digitalWrite(SS_ADC_2, LOW);
  temperatureByte[0] = SPI.transfer(0);
  temperatureByte[1] = SPI.transfer(0);
  digitalWrite(SS_ADC_2, HIGH);
  SPI.endTransaction();

  //Convert AIN0 binary to Voltage
  uint16_t ain0Value = 0x0000;
  ain0Value = ain0Byte[0] << 8;
  ain0Value |= ain0Byte[1];
  double ain0Voltage = ain0Value * 0.000076293945313;

  //Convert AIN2 binary to Voltage
  uint16_t ain2Value = 0x0000;
  ain2Value = ain2Byte[0] << 8;
  ain2Value |= ain2Byte[1];
  double ain2Voltage = ain2Value * 0.000076293945313;

  //Convert Temperature to degrees C
  uint16_t temperatureValue = 0x0000;
  temperatureValue = temperatureByte[0] << 8;
  temperatureValue |= temperatureByte[1];
  if(temperatureValue > 0x8000){               //If 2's complement negitive (first bit = 1)
    temperatureValue -= 1;
    temperatureValue = ~temperatureValue;
    temperatureValue <<= 
    temperatureValue = temperatureValue * -0.03125;
  }else{                                      //Else not negitive
    temperatureValue = temperatureValue * 0.03125;
  }
  
  // Write AINO Data
  Serial.println("AIN0:");
  Serial.println(ain0Byte[0], BIN);
  Serial.println(ain0Byte[1], BIN);
  Serial.println(ain0Voltage);
  // Write AIN2 Data
  Serial.println("AIN2");
  Serial.println(ain2Byte[0], BIN);
  Serial.println(ain2Byte[1], BIN);
  Serial.println(ain2Voltage);
  // Write Temperature Data
  Serial.println("Temperature");
  Serial.println(temperatureByte[0], BIN);
  Serial.println(temperatureByte[1], BIN);
  Serial.println(temperatureValue);
  Serial.println();
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
  int dataPin = 4;
  int clockPin = 13;
  uint8_t slaveByte = 0x00;
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
    slaveByte = 0x00;
  }
  digitalWrite(SHIFT_LATCH, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, slaveByte);
  digitalWrite(SHIFT_LATCH, HIGH);
  delayMicroseconds(1);
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
void writeNegitiveDigitalPot(){
  SPI.beginTransaction(mode1);
  slaveRegister(1);               //Write SS 1 low
  SPI.transfer(0b11111111);
  slaveRegister(8);               //Write all bits high
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
  SPI.beginTransaction(mode0);
  slaveRegister(2);
  SPI.transfer(0);      //Choose the register to write to
  SPI.transfer(level);  //Set the level (0-255)
  slaveRegister(8);
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

  //Once we've got that, we split it up into separate bytes.
  MSB = (int)((calculated_freq_word & 0xFFFC000)>>14); //14 bits
  LSB = (int)(calculated_freq_word & 0x3FFF);

  //Set control bits DB15 ande DB14 to 0 and one, respectively, for frequency register 0
  LSB |= 0x4000;
  MSB |= 0x4000;
 
  phase &= 0xC000;
 
  WriteRegisterAD9833(0x2100); // Write command register

  //Set the frequency==========================
  WriteRegisterAD9833(LSB); //lower 14 bits
  WriteRegisterAD9833(MSB); //upper 14 bits
  WriteRegisterAD9833(phase); //mid-low
 
  //Power it back up
  //WriteRegisterAD9833(0x2020); //square
  WriteRegisterAD9833(0x2000); //sin
  //WriteRegisterAD9833(0x2002); //triangle 
}

/*
 * Function Name:   WriteRegisterAD9837()
 * 
 * Description:
 *      Writes data to the AD9837 Frequency Generator
 * 
 * Params:
 *     int dat - the data to be written 
 *     
 * Notes:
 *     
 */
void WriteRegisterAD9833(int dat){
  slaveRegister(3);
  SPI.beginTransaction(mode2);
  SPI.transfer(highByte(dat));
  SPI.transfer(lowByte(dat));
  slaveRegister(8);
}  
