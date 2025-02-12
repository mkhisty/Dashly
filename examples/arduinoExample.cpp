#include <Wire.h>
#include <DHT.h>
#include <Adafruit_SI1145.h>

// Sensor Pins
#define DHTPIN 2          // DHT22 sensor connected to digital pin 2
#define DHTTYPE DHT22     // Using DHT22 sensor

// Sensor Objects
DHT dht(DHTPIN, DHTTYPE);
Adafruit_SI1145 sunlightSensor = Adafruit_SI1145();

// Data Storage (24-hour storage)
float temperatureData[24];
float humidityData[24];
float sunlightData[24];

// Timing variables
const unsigned long interval = 3600000; // 1 hour in milliseconds
unsigned long previousMillis = 0;
int index = 0;

void setup() {
    Serial.begin(9600);
    dht.begin();
    
    if (!sunlightSensor.begin()) {
        Serial.println("Error: SI1145 Sunlight Sensor not detected!");
        while (1);
    } else {
        Serial.println("SI1145 Sunlight Sensor initialized.");
    }

    Serial.println("System Ready. Logging data every hour.");
}

void loop() {
    unsigned long currentMillis = millis();

    // Take a reading every hour
    if (currentMillis - previousMillis >= interval && index < 24) {
        previousMillis = currentMillis;

        // Read Temperature & Humidity
        float temperature = dht.readTemperature();
        float humidity = dht.readHumidity();

        // Read Sunlight Sensor Data
        float visible = sunlightSensor.readVisible();
        float infrared = sunlightSensor.readIR();
        float uv = sunlightSensor.readUV() / 100.0;  // Convert to UV index

        // Store data
        temperatureData[index] = temperature;
        humidityData[index] = humidity;
        sunlightData[index] = uv;

        // Print Data
        Serial.print("Hour ");
        Serial.print(index + 1);
        Serial.print(" - Temp: ");
        Serial.print(temperature);
        Serial.print("°C, Humidity: ");
        Serial.print(humidity);
        Serial.print("%, UV Index: ");
        Serial.println(uv);

        // Increment index
        index++;
    }

    // Stop loop after 24 readings
    if (index >= 24) {
        Serial.println("Data collection complete. Reset Arduino to restart.");
        while (1); // Halt execution
    }
}
