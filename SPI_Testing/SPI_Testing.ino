#include <SPI.h>
#include <math.h>

// Define SPI Slave Select Pins
const int SS_DAC_1 = 6;
const int SS_ADC_1 = 7;
const int SS_DAC_2 = 8;
const int SS_ADC_2 = 9;

// SPI Speed and mode settings
SPISettings mode0(8000000, MSBFIRST, SPI_MODE0);
SPISettings mode1(8000000, MSBFIRST, SPI_MODE1);

void setup() {
  // Set SS to Output
  pinMode(SS_DAC_1, OUTPUT);
  pinMode(SS_ADC_1, OUTPUT);
  pinMode(SS_DAC_2, OUTPUT);
  pinMode(SS_ADC_2, OUTPUT);

  // Initialize SPI
  SPI.begin();

  //Initialize DACs
  initDACs();
  
  // Initialize Debug Serial
  Serial.begin(9600);
}

void loop() {
  //setDAC(4.8, 2); // Set DAC_2 to 4.8 Volts
  readADC();
  Serial.println();
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
 *      
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
  SPI.transfer(0b00000000);
  SPI.transfer(0b11100011);
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
  ain0Byte[0] = SPI.transfer(0b01100000);
  ain0Byte[1] = SPI.transfer(0b11100011);
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
  ain2Byte[1] = SPI.transfer(0b11110011);
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
  int ain0Voltage = ain0Value * 0.000076293945313;

  //Convert AIN2 binary to Voltage
  uint16_t ain2Value = 0x0000;
  ain2Value = ain2Byte[0] << 8;
  ain2Value |= ain2Byte[1];
  int ain2Voltage = ain2Value * 0.000076293945313;

  // Write AINO Data
  Serial.println("AIN0:");
  Serial.println(ain0Byte[0], BIN);
  Serial.println(ain0Byte[1], BIN);
  Serial.println(ain0Voltage, 4);
  // Write AIN2 Data
  Serial.println("AIN2");
  Serial.println(ain2Byte[0], BIN);
  Serial.println(ain2Byte[1], BIN);
  Serial.println(ain2Voltage, 4);
  // Write Temperature Data
  Serial.println("Temperature");
  Serial.println(temperatureByte[0], BIN);
  Serial.println(temperatureByte[1], BIN);
  Serial.println();
}

