#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Thermistor and calculation parameters
int THERMISTORPIN = A0, BCOEFFICIENT = 3380;
float THERMISTORNOMINAL = 10000, TEMPERATURENOMINAL = 25, SERIESRESISTOR = 10000;

// Define I2C LCD address and dimensions
LiquidCrystal_I2C lcd(0x27, 16, 2); 

int sample[5];
int relayPin = 7; // Pin connected to the relay module

void setup() {
  Serial.begin(9600);
  
  lcd.init();          // Initialize the LCD
  lcd.backlight();     // Turn on the LCD backlight

  pinMode(relayPin, OUTPUT); // Set the relay pin as output
  digitalWrite(relayPin, LOW); // Ensure relay is off initially
}

void loop() {
  int i;
  float average;

  // Take 5 samples with a slight delay
  for (i = 0; i < 5; i++) {
    sample[i] = analogRead(THERMISTORPIN);
    delay(10);
  }

  // Calculate the average of the samples
  average = 0;
  for (i = 0; i < 5; i++) {
    average += sample[i];
  }
  average /= 5;

  // Convert the value to resistance
  average = 1023 / average - 1;
  average = SERIESRESISTOR / average;

  // Apply Steinhart-Hart equation to calculate temperature
  float steinhart;
  steinhart = average / THERMISTORNOMINAL;           // (R/Ro)
  steinhart = log(steinhart);                        // ln(R/Ro)
  steinhart /= BCOEFFICIENT;                         // 1/B * ln(R/Ro)
  steinhart += 1.0 / (TEMPERATURENOMINAL + 273.15);  // + (1/To)
  steinhart = 1.0 / steinhart;                       // Invert
  steinhart -= 273.15;                               // Convert to Celsius

  // Print temperature to Serial Monitor
  Serial.print("Temp = ");
  Serial.println((int)steinhart);

  // Display temperature on I2C LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print((int)steinhart);
  lcd.print(" C");

  // Check temperature and control the relay
  if (steinhart > 30) {
    digitalWrite(relayPin, LOW); // Turn on relay
    lcd.setCursor(0, 1);
    lcd.print("Fan: ON");
  } else {
    digitalWrite(relayPin, HIGH); // Turn off relay
    lcd.setCursor(0, 1);
    lcd.print("Fan: OFF");
  }

  delay(500);
}
