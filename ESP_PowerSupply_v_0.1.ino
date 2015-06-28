#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

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
void matchRequest(String req);
void readSerial();
void writeSerial();
void buildResponse(int index);

/* Set up two output channels */
Channel channel1;
Channel channel2;

/* WiFi ssid & password */
const char* ssid = "SSID";
const char* password = "password";

/* JSON Buffer */
StaticJsonBuffer<2000> jsonBuffer;                 
JsonObject& ROOT = jsonBuffer.createObject();

/* Initialize Server Port */
WiFiServer server(80); 

void setup() {
  // Begin serial
  Serial.begin(115200);
  delay(10);
  // Begin Wifi
  WiFi.begin(ssid, password);
  delay(10);
  // Begin Server
  server.begin();
  delay(1000);

  /* Mimic values from client */
  channel1.controlVoltage = 15.5;
  channel1.controlCurrent = 2.51;
  channel1.limitingPreference = 1;
  channel2.controlVoltage = 12.5;
  channel2.controlCurrent = 2.52;
  channel2.limitingPreference = 0;
  
  Serial.println("ESP Ready");
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

 // Wait until the client sends some data
  while(!client.available()){
    delay(1);
  }

  //Read measured values if available
  if (Serial.available() > 0){
    readSerial();
  }
  
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  //Serial.print(request);
  client.flush();

  matchRequest(request);

  // Print to client
  ROOT.prettyPrintTo(client);
  client.stop();
}

/* Match Request */ 
void matchRequest(String request){
if (request.indexOf("GET") != -1){

}
if (request.indexOf("GET") != -1){ // Should be else if & POST but changed for testing 
  writeSerial();
}

if (request.indexOf("/v0.1/all/json") != -1){
    buildResponse(0);
  } else if (request.indexOf("/v0.1/channel1/json") != -1){
    buildResponse(1);
  } else if (request.indexOf("/v0.1/channel2/json") != -1){
    buildResponse(2);
  } else if (request.indexOf("/v0.1/negitiveChannel/json") != -1){
    buildResponse(3);
  } else if (request.indexOf("/v0.1/frequencyGen/json") != -1){
    buildResponse(4);
  } else if (request.indexOf("/v0.1/settings/json") != -1){
    buildResponse(5);
  } else {
    buildResponse(403);
  }
}

/* Read measured values from Atmega */ 
void readSerial(){
  String variable = Serial.readStringUntil(':');
  double value = Serial.parseFloat();
  if (variable.indexOf("channel1") != -1){
    if(variable.indexOf("measuredVoltage") != -1){                            //measuredVoltage
      channel1.measuredVoltage = value;
      //Serial.print("channel1.measuredVoltage: ");
      //Serial.println(channel1.measuredVoltage);
    } else if(variable.indexOf("measuredCurrent") != -1){                     //measuredCurrent
      channel1.measuredCurrent = value;
      //Serial.print("channel1.measuredCurrent: ");
      //Serial.println(channel1.measuredCurrent);
    } else if(variable.indexOf("voltageLimiting") != -1){                     //voltageLimiting
      channel1.voltageLimiting = value;
      //Serial.print("channel1.voltageLimiting: ");
      //Serial.println(channel1.voltageLimiting);
    } else if(variable.indexOf("currentLimiting") != -1){                      //currentLimiting
      channel1.currentLimiting = value;
      //Serial.print("channel1.currentLimiting: ");
      //Serial.println(channel1.currentLimiting);
    } 
  } else if (variable.indexOf("channel2") != -1){
    if(variable.indexOf("measuredVoltage") != -1){                            //measuredVoltage
      channel2.measuredVoltage = value;
      //Serial.print("channel2.measuredVoltage: ");
      //Serial.println(channel2.measuredVoltage);
    } else if(variable.indexOf("measuredCurrent") != -1){                     //measuredCurrent
      channel2.measuredCurrent = value;
      //Serial.print("channel2.measuredCurrent: ");
      //Serial.println(channel2.measuredCurrent);
    } else if(variable.indexOf("voltageLimiting") != -1){                     //voltageLimiting
      channel2.voltageLimiting = value;
      //Serial.print("channel2.voltageLimiting: ");
      //Serial.println(channel2.voltageLimiting);
    } else if(variable.indexOf("currentLimiting") != -1){                     //currentLimiting
      channel2.currentLimiting = value;
      //Serial.print("channel2.currentLimiting: ");
      //Serial.println(channel2.currentLimiting);
    } 
  }
}  

/* Writes values from JSON */
void writeSerial(){
  Serial.print("channel1.controlVoltage: ");
  Serial.println(channel1.controlVoltage);
  delay(10);
  Serial.print("channel1.controlCurrent: ");
  Serial.println(channel1.controlCurrent);
  delay(10);
  Serial.print("channel1.limitingPreference: ");
  Serial.println(channel1.limitingPreference);
  delay(10);
  
  Serial.print("channel2.controlVoltage: ");
  Serial.println(channel2.controlVoltage);
  delay(10);
  Serial.print("channel2.controlCurrent: ");
  Serial.println(channel2.controlCurrent);
  delay(10);
  Serial.print("channel2.limitingPreference: ");
  Serial.println(channel2.limitingPreference);
  delay(10);
}

/* Build JSON Response for Client */
void buildResponse(int index){
  if(index == 1 || index == 0){                                                       //if channel1
    JsonArray& CHANNEL_1 = ROOT.createNestedArray("channel1");
    JsonObject& OBJECT_1 = CHANNEL_1.createNestedObject();
    OBJECT_1["controlVoltage"] = channel1.controlVoltage;
    OBJECT_1["controlCurrent"] = channel1.controlCurrent;
    OBJECT_1["limitingPreference"] = channel1.limitingPreference;
    OBJECT_1["measuredVoltage"] = channel1.measuredVoltage;
    OBJECT_1["measuredCurrent"] = channel1.measuredCurrent;
    OBJECT_1["voltageLimiting"] = channel1.voltageLimiting;
    OBJECT_1["currentLimiting"] = channel1.currentLimiting;
  }
  if (index == 2 || index == 0){                                                      //if channel2
    JsonArray& CHANNEL_2 = ROOT.createNestedArray("channel2");
    JsonObject& OBJECT_2 = CHANNEL_2.createNestedObject();
    OBJECT_2["controlVoltage"] = channel2.controlVoltage;
    OBJECT_2["controlCurrent"] = channel2.controlCurrent;
    OBJECT_2["limitingPreference"] = channel2.limitingPreference;
    OBJECT_2["measuredVoltage"] = channel2.measuredVoltage;
    OBJECT_2["measuredCurrent"] = channel2.measuredCurrent;
    OBJECT_2["voltageLimiting"] = channel2.voltageLimiting;
    OBJECT_2["currentLimiting"] = channel2.currentLimiting;
  } 
  if (index == 403){
    ROOT["errorMessage"] = "Sorry, URI not found";
  }
}


