/*
  RaggedPi Project
  Arduino 2 "Nutmeg" - Circulation Fan
  Written by david durost <david.durost@gmail.com>
*/
#include <SoftwareSerial.h>     // Serial library
#include <TimeLib.h>            // Time library
#include <Wire.h>               // One Wire library
#include <DS1307RTC.h>          // Realtime Clock library
#include <SPI.h>                // SPI library
#include <SD.h>                 // SD library
#include <Relay.h>              // Relay library
#include <DHT.h>
extern "C" {
    #include <Utility.h>
}

// Debugging
// #define DEBUG                   // uncomment for debugger
//#define XBEE                    // uncomment to enable xbee transmission
#define LOG                     // uncomment to enable datalogging
#define SERIAL                  // uncomment to enable serial output
// Thermometer
#define DHTPIN 5                // digital pin
#define DHTFAHRENHEIT true      // Fahrenheit (true) or Celsius (false)
#define TEMP 58                 // Temperature trigger (Fahrenheit)

// Variables
Relay fanRelay(RELAY1, LOW);    // relay
DHT dht(DHTPIN, DHT11);         // dht11
#ifdef LOG
File logfile;                   // log file
#endif
#ifdef XBEE
SoftwareSerial XBee(SDA, SDL);  // XBee2
#endif
//char* msg;                      // reused variable
//tmElements_t rtc;               // RTC


/**
 * Setup
 */
void setup() {
    bool parse=false;
    bool config=false;

    Serial.begin(9600);
    while (!Serial);

    #ifdef XBEE
    XBee.begin(9600);
    #endif
    print("RaggedPi Project Codename Nutmeg Initializing");
    dotdotdot();
    println();
    
    // SPI
    SPI.begin();

    // RTC 
    print("Initializing RTC");
    dotdotdot(5);
    
    if (setDateFromString(__DATE__) && setTimeFromString(__TIME__)) {
        parse = true;
        if (RTC.write(rtc)) {
            config = true;
        }
    }
    delay(200);
    if (parse && config) {
    } else if (parse) {
        println("Failed");
        error("DS1307 Communication Error");
    } else {
        println("Failed");
        sprintf(msg, "Could not parse info from the compiler, Time=\"%s\", Date=\"%s\"", __TIME__, __DATE__);
        error(msg);
    }
    println("Initialized.");

    // SD
    print("Initializing SD card");
    dotdotdot(10, ".", 100, true);
    if (!SD.begin(CS))  error("SD card failed or is missing");
    println("Initialized.");

    // Logging
    #ifdef LOG
    char filename[] = "LOG00.csv";
    for (uint8_t i = 0; i < 100; i++) {
        filename[3] = i / 10 + '0';
        filename[4] = i % 10 + '0';
        if (!SD.exists(filename)) {
            logfile = SD.open(filename, FILE_WRITE);
            break;
        }
    }
    if (!logfile) {
        error("log file creation failed.  System will continue.", false);
        #undef LOG
    }
    else {
        sprintf(msg, "Logfile: %s", filename);
        println(msg);
    }
    #endif

    // 1 WIRE
    Wire.begin();

    // Relay
    fanRelay.begin();

    // Set pins
    pinMode(DHTPIN, INPUT);
    pinMode(LED, OUTPUT);
    pinMode(CS, OUTPUT);
    println("System initialized.");
}

/**
 * Loop
 */
void loop() {
    RTC.read(rtc);

    // Read DHT data (~250ms)
    float temp = dht.readTemperature(DHTFAHRENHEIT);
    if (isnan(temp))   warning("Failed to read from DHT sensor! (t)");   

    // Fan relay
    if ((TEMP == temp && fanRelay.isOff()) || (TEMP == temp && fanRelay.isOn())) {
        fanRelay.toggle();
        sprintf(msg, "%s [%s] - fan", getDateTimeString(), temp);
        if (fanRelay.isOn()) {
            sprintf(msg,"%s %s", msg, "activated");
            startTimer();
        } else {
            stopTimer();
            sprintf(msg, "%s %s [%s]", msg, "deactivated", calculateTimerResults(endTime - startTime));
        }
        println(msg);
        delay(3000);
        logfile.close();
    }
}
