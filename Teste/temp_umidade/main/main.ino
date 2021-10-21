#include "DHT.h"
#include "LiquidCrystal_I2C.h"

LiquidCrystal_I2C lcd(0x3F, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#define DHTPIN 7     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define fan 4

//int maxHum = 60;
//int maxTemp = 40;

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600); 
  dht.begin();
  lcd.begin (16, 2);
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float umid = dht.readHumidity();
  // Read temperature as Celsius
  float temp = dht.readTemperature();
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(umid) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  
//  if(h > maxHum || t > maxTemp) {
//      digitalWrite(fan, HIGH);
//  } else {
//     digitalWrite(fan, LOW); 
//  }
  
  Serial.print("Humidity: "); 
  Serial.print(umid);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(temp);
  Serial.println(" *C ");
  lcd.setCursor(0,0);
  lcd.print("Umidade:");
  lcd.setCursor(8,0);
  lcd.print(umid);
  lcd.setCursor(13,0);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Temp:");
  lcd.setCursor(8,1);
  lcd.print(temp);
  lcd.setCursor(13,1);
  lcd.print("C");


}
