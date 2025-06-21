#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <DHT.h>
#include <Servo.h>

// Pin Definitions
#define PIR_PIN 2
#define DHT_PIN 4
#define RELAY_PIN 5
#define BUZZER_PIN 6
#define LED_PIN 7
#define SD_CS_PIN 10
#define LDR_PIN A0
#define FAN_PIN 3
#define LIGHT_PIN 9

// DHT Sensor Settings
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

// LCD Settings (I2C address: 0x27, 16 columns x 2 rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Servo Motor
Servo servo;

// Flags
bool intrusionDetected = false;

void setup() {
  Serial.begin(115200);

  // Initialize Pins
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);

  // Initialize DHT Sensor
  dht.begin();

  // Initialize SD Card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card initialization failed!");
    return;
  }
  Serial.println("SD Card initialized.");

  // Initialize LCD
  lcd.init();
  lcd.backlight();

  // Initialize Servo
  servo.attach(8); // Servo connected to pin 3
  servo.write(0);  // Start position

  // Display a startup message
  lcd.setCursor(0, 0);
  lcd.print("Smart Home Ready");
  lcd.setCursor(0, 1);
  lcd.print("Initializing...");
  delay(2000);
}

void loop() {
  // Read PIR Sensor
  int pirValue = digitalRead(PIR_PIN);
  if (pirValue == HIGH) {
    intrusionDetected = true;
    tone(BUZZER_PIN, 2500);
    digitalWrite(LED_PIN, HIGH);
    Serial.println("Motion Detected");
    logEvent("Intrusion detected!");
  } else {
    intrusionDetected = false;
    noTone(BUZZER_PIN);
    digitalWrite(LED_PIN, LOW);
    Serial.println("No Motion Detected");
  }

  // Read Temperature and Humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Control Fan based on Temperature
  if (temperature > 30.0) {
    digitalWrite(FAN_PIN, HIGH);
    servo.write(180); // Adjust servo to mid-point
    delay(1000);
    servo.write(150); 
    delay(2000);
    servo.write(120); 
    delay(1000);
    servo.write(90); 
    delay(1000);
    servo.write(70); 
    delay(2000);
    servo.write(50); 
    delay(2000);
    servo.write(40); 
    delay(2000);
    servo.write(30); 
    delay(2000); 
    servo.write(20); 
    delay(2000);
    servo.write(10); 
    delay(2000);

  } else {
    digitalWrite(FAN_PIN, LOW);
    servo.write(0); // Reset servo position
    //delay((2000));
  }

  // Read LDR Value for Light Adjustment
  int ldrValue = analogRead(LDR_PIN);
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  // Adjust Relay based on LDR and Temperature
  if (ldrValue < 500) { // Dim light detected
    digitalWrite(RELAY_PIN, HIGH); // Turn on lights
    digitalWrite(LIGHT_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW); // Turn off lights
    digitalWrite(LIGHT_PIN, LOW);
  }

  // Display Data on LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.print("C ");
  lcd.print("Hum: ");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0, 1);
  if (intrusionDetected) {
    lcd.print("Intrusion Alert!  ");
  } else {
    lcd.print("LDR: ");
    lcd.print(ldrValue);
    lcd.print("       ");
  }

  // Send Status to Serial Monitor
  Serial.println("=== System Status ===");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Intrusion: ");
  Serial.println(intrusionDetected ? "Yes" : "No");
  Serial.println("=====================");

  delay(1000);
}

// Log Event to SD Card
void logEvent(String event) {
  File logFile = SD.open("log.txt", FILE_WRITE);
  if (logFile) {
    logFile.println(event);
    logFile.close();
    Serial.println("Event logged: " + event);
  } else {
    Serial.println("Failed to open log file!");
  }
}
