/*
  RaggedPi Project
  Arduino 3 "Nutmeg"
  Written by david durost <david.durost@gmail.com>
*/

/* Includes */
#include <DHT.h>

// Defines
// DHT
#define DHTPIN 4 // arduino digital pin
#define DHTTYPE DHT11
#define DHTTIME 2000 // wait 2 sec between measurements
#define DHTFAHRENHEIT true // fahrenheit (true) or celsius (false)
// MQ2
#define MQ2PIN A0 // arduino analog pin

/* Variables */
int ledState = LOW; // LED state

/* Initializations */
DHT dht(DHTPIN, DHTTYPE);

/**
 * Setup
 */
void setup() {
  Serial.begin(9600);
  Serial.println("RaggedPi Project Codename Nutmeg Initialized.");
  
  dht.begin();
}

/**
 * Loop
 */
void loop() {
  readDHT();
  readMq2();
}

/**
 * Read DHT sensor
 */
void readDHT() {
  delay(DHTTIME);
  
  // Read data (~250ms)
  float humidity = dht.readHumidity();
  float temp = dht.readTemperature(DHTFAHRENHEIT);
  
  // Check for read fails
  if(isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor. (h)");
    return;
  } else if (isnan(temp)) {
    Serial.println("Failed to read from DHT sensor! (t)");
    return;
  }

  // Compute heat index
  float hIndex = dht.computeHeatIndex(temp, humidity, DHTFAHRENHEIT);
  
  // Output
  String CF = "";
  if (DHTFAHRENHEIT) {
    CF = " *F";
  } else {
    CF = " *C";
  }
  
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(CF);
  Serial.print("\t");
  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Heat Index: ");
  Serial.print(hIndex);
  Serial.print(CF);
  Serial.print("\n");
}
