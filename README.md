# Connected Power Supply
The connected power supply is a project that a friend and I have been working on for some time now. My hopes are for it to work seemlessly with any mobile device (or any device for that matter). For right now it will serve as a REST API to change values like voltage, current, limiting, frequency, and wave shape. I would like for this to turn into a nice REST library application for the ESP8266 and Arduino boards.

I've written up most of my initial design work to my blog: http://thingable.co/tag/connected-power-supply-unit/

## REST API
This project currently supports GET requests. POST is just about there but I will sort that all out once I have fully finished and integrated my client. Because the power supply will need to be in constant contact with interface device POST requests directed at a certain URI will return the information a GET would have at that URI

####/v0.1/all/json 
Returns all data available for each channel, and frequency generator. 

####/v0.1/channel1/json 
Returns the following values to the client for the first variable channel in JSON format:

    double controlVoltage;    //knob voltage
    double controlCurrent;    //knob current
    bool limitingPreference;  //0 current / 1 voltage
    double measuredVoltage;   //measured ADC value
    double measuredCurrent;   //measured ADC value
    bool voltageLimiting;     //indicates voltage limiting
    bool currentLimiting;     //indicates current limiting
    
####/v0.1/channel2/json 
Returns the following values to the client for the second variable channel in JSON format:

    double controlVoltage;    //knob voltage
    double controlCurrent;    //knob current
    bool limitingPreference;  //0 current / 1 voltage
    double measuredVoltage;   //measured ADC value
    double measuredCurrent;   //measured ADC value
    bool voltageLimiting;     //indicates voltage limiting
    bool currentLimiting;     //indicates current limiting
    
####/v0.1/negitiveChannel/json 
Much like the previous channels except for a voltage that is below ground. It has not been implemented yet.

####/v0.1/frequencyGen/json 
This will provide information about the type of wave (square, saw tooth, triangle, or sine) as well as the variable output voltage. This has yet to be implemented

####/v0.1/settings/json
This is the place where extra settings will go. Stuff like WiFi passwords and clients allowed to access it. (I have something fancy in mind for client access and security)

## Stay tuned
This project isn't yet complete, but I would love for feedback and suggestions for best practice. Once this goes into a full ESP8266 REST api library I hope it becomes useful to everybody else.
