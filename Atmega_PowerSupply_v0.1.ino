class Channel {
  public:
    double controlVoltage;                              //knob voltage
    double controlCurrent;                              //knob current
    bool limitingPreference;                            //bool 0 current / 1 voltage
    double measuredVoltage;                             //measured adc value
    double measuredCurrent;                             //measured adc value
    bool voltageLimiting;                               //bool indicating voltage limiting
    bool currentLimiting;                               //bool indicating voltage limiting
};

/* Function Prototypes */
void initializeESP();
void readSerial();
void writeSerial();

/* Set up two output channels */
Channel channel1;
Channel channel2;

/* Globals (terrible practice I know) */
bool first = true;
int i = 0;  //Temporary  

void setup() {
  Serial.begin(115200);
  delay(10);

  /* Mimic values from components */
  channel1.measuredVoltage = 15.6;
  channel1.measuredCurrent = .3;
  channel1.voltageLimiting = 1;
  channel1.currentLimiting = 0;
  channel2.measuredVoltage = 12.6;
  channel2.measuredCurrent = .4;
  channel2.voltageLimiting = 0;
  channel2.currentLimiting = 1;

  Serial.println("Arduino Begin");
}

void loop() {
  /* Prepare for ESP communication */
  if (Serial.available() > 0 && first == true){
    delay(150);
    initializeESP();
  }
  
  /* Write values to ESP when available */
  if (i == 2000){    // To be replaced with if (new values read from SPI)
    writeSerial();
    i = 0;
  }
  delay(1);
  i++;

  /* Read values from ESP when available */
  if (Serial.available() > 0){
    readSerial();
  }
}

/* determines when ESP has booted and is ready for communication */
void initializeESP(){
  Serial.readStringUntil('ESP Ready');
  Serial.print("Initializing");
  delay(1000);
  while(Serial.available() > 0){
    delay(150);
    Serial.read();
    Serial.print('.');
  }
  delay(100);
  Serial.println();
  Serial.println("ESP Begin");
  first = false;
}

/* reads values from ESP */
void readSerial(){
  String variable = Serial.readStringUntil(':');
  double value = Serial.parseFloat();
  if (variable.indexOf("channel1") != -1){
    if(variable.indexOf("controlVoltage") != -1){                           //controlVoltage
      channel1.controlVoltage = value;
      //Serial.print("channel1.controlVoltage: ");
      //Serial.println(channel1.controlVoltage);
    } else if(variable.indexOf("controlCurrent") != -1){                    //controlCurrent
      channel1.controlCurrent = value;
      //Serial.print("channel1.controlCurrent: ");
      //Serial.println(channel1.controlCurrent);
    } else if(variable.indexOf("limitingPreference") != -1){                 //limitingPreference
      channel1.limitingPreference = value;
      //Serial.print("channel1.limitingPreference: ");
      //Serial.println(channel1.limitingPreference);
    }
  } else if (variable.indexOf("channel2") != -1){
    if(variable.indexOf("controlVoltage") != -1){                           //controlVoltage
      channel2.controlVoltage = value;
      //Serial.print("channel2.controlVoltage: ");
      //Serial.println(channel2.controlVoltage);
    } else if(variable.indexOf("controlCurrent") != -1){                    //controlCurrent
      channel2.controlCurrent = value;
      //Serial.print("channel2.controlCurrent: ");
      //Serial.println(channel2.controlCurrent);
    } else if(variable.indexOf("limitingPreference") != -1){                 //limitingPreference
      channel2.limitingPreference = value;
      //Serial.print("channel2.limitingPreference: ");
      //Serial.println(channel2.limitingPreference);
    }
  }
}

/* Writes values from SPI components */
void writeSerial(){
  Serial.print("channel1.measuredVoltage: ");
  Serial.println(channel1.measuredVoltage);
  delay(10);
  Serial.print("channel1.measuredCurrent: ");
  Serial.println(channel1.measuredCurrent);
  delay(10);
  Serial.print("channel1.voltageLimiting: ");
  Serial.println(channel1.voltageLimiting);
  delay(10);
  Serial.print("channel1.currentLimiting: ");
  Serial.println(channel1.currentLimiting);
  delay(10);

  Serial.print("channel2.measuredVoltage: ");
  Serial.println(channel2.measuredVoltage);
  delay(10);
  Serial.print("channel2.measuredCurrent: ");
  Serial.println(channel2.measuredCurrent);
  delay(10);
  Serial.print("channel2.voltageLimiting: ");
  Serial.println(channel2.voltageLimiting);
  delay(10);
  Serial.print("channel2.currentLimiting: ");
  Serial.println(channel2.currentLimiting);
  delay(10);
}
