/*
  RaggedPi Project
  Arduino "Nutmeg" - Circulation Fan
  Written by david durost <david.durost@gmail.com>
*/
/* Includes */
#include <SoftwareSerial.h>     // Serial library
#include <Relay.h>              // Relay library
#include <DHT.h>                // Thermometer library

/* Misc Constants */
#define SDA 2                   // SDA
#define SDL 3                   // SDL
#define CS 10                   // Chipselect
#define LED 13                  // LED pin
#define DHT_PIN 5               // Analog pin
#define DHT_F true              // Use fahrenheit
#define TEMP_SETTING 62         // Temp setting to trigger relay in F
#define SLEEP_TIME 90000        // ms

/* Objects */
Relay fanRelay(RELAY4, LOW);
DHT dht(DHT_PIN, DHT11);

/* Enums */
enum Modes {
    MONITOR_MODE,
    SLEEP_MODE,
    MANUAL_MODE
};

/* Variables */
unsigned long sleep = 0;
Modes status = MONITOR_MODE;
bool override = false;
float temp = 32.00;

/**
 * Monitor mode
 */
void monitorMode() {
    // If relay is off and temp rises above threshold
    if (!override && fanRelay.isOff() && temp >= TEMP_SETTING)  {
        Serial.print("Temp: ");
        Serial.print(temp);
        Serial.println(" F\tFan Activated.");
        fanRelay.on();
    } 
    // Else if relay is on and temp falls below threshold
    else if (!override && fanRelay.isOn() && temp < TEMP_SETTING) {
        Serial.print("Temp: ");
        Serial.print(temp);
        Serial.println(" F\tFan Deactivated.");
        fanRelay.off();
        status = SLEEP_MODE;
        sleep = millis();
    } 
    // Else if override is true
    else if (override) {
        fanRelay.toggle();
        if (fanRelay.isOff()) {
            status = SLEEP_MODE;
            sleep = millis();
        } else {
            Serial.println("Fan manually activated.");
        }
    }
}

/**
 * Sleep mode
 */
void sleepMode() {
    long sleeptime = millis() - sleep;
    fanRelay.off();
    Serial.println("Sleeping...");

    if (sleeptime > SLEEP_TIME) {
        status = MONITOR_MODE;
    }
}

/**
 * Manual mode
 */
void manualMode() {
    override = true;
    status = MONITOR_MODE;
}

/**
 * Setup
 */
void setup() {
    Serial.begin(9600);
    while (!Serial);

    /* Set pins ***************************************************************/
    pinMode(DHT_PIN, INPUT);
    pinMode(LED, OUTPUT);
    pinMode(CS, OUTPUT);

    Serial.println("RaggedPi Project Codename Nutmeg Initializing...");
    
    /* Relay ******************************************************************/
    Serial.print("Initializing relays...");
    fanRelay.begin();
    Serial.println("[OK]");
    delay(600);
    Serial.print("Initializing dht...");
    dht.begin();
    Serial.println("[OK]");
    delay(600);
    Serial.println("System initialized.");
    delay(600);
}

void loop() {
    // Read dht
    temp = dht.readTemperature(DHT_F);
    // Verify reading
    if (isnan(temp)) {
        fanRelay.on();
        Serial.print("[ERROR] Failed to read from DHT sensor.  Fan will remain on.");   
        override = true;
    }
    
    switch (status) {
        case MONITOR_MODE:
            monitorMode();
            break;
        case SLEEP_MODE:
            sleepMode();
            break;
        case MANUAL_MODE:
            manualMode();
            break;
    }

    delay(3000);
}