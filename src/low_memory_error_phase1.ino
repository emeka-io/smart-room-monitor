#include <Arduino.h>
#include <U8g2lib.h>
#include "DHT.h"

#define DHTPIN 2     
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// MEMORY ERROR LOCATION: The "_F_" suffix stands for "Full Frame Buffer."
// This line allocates 1,024 bytes of RAM (50% of an Arduino Uno's memory) immediately.
// Combined with the DHT library, this is what triggered your 92% RAM usage warning.
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

bool toggle = false; // For the flickering status indicator

void setup() {
  Serial.begin(9600); // Start serial communication for debugging
  dht.begin();
  u8g2.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  //  MEMORY USAGE: clearBuffer() works with the Full Frame Buffer defined above.
  u8g2.clearBuffer();					
  u8g2.setFont(u8g2_font_ncenB08_tr); 
  u8g2.drawStr(0, 10, "ROOM ENVIRONMENT");
  u8g2.drawHLine(0, 12, 128); // Header line

  if (isnan(h) || isnan(t)) {
    u8g2.drawStr(0, 40, "Sensor Error!");
    Serial.println("Failed to read from DHT sensor!");
  } else {
    // Temperature Display
    u8g2.setCursor(0, 35);
    u8g2.print("Temp:  "); u8g2.print(t, 1); u8g2.print(" C");
    
    // Humidity Display
    u8g2.setCursor(0, 55);
    u8g2.print("Humid: "); u8g2.print(h, 1); u8g2.print(" %");

    // "Reading..." Status Bar (Flickers every update)
    if (toggle) {
      u8g2.setFont(u8g2_font_u8glib_4_tf); // Tiny font for status
      u8g2.drawStr(90, 62, "READING...");
    }
    toggle = !toggle; // Flip the state for the next 2-second loop

    // Also send data to Serial Monitor just in case
    Serial.print("Humidity: "); Serial.print(h);
    Serial.print("%  Temperature: "); Serial.print(t); Serial.println("°C");
  }
  
  //  MEMORY USAGE: sendBuffer() pushes the heavy 1KB buffer to the screen.
  u8g2.sendBuffer();					
  delay(2000); // Mandatory 2-second delay for DHT11 accuracy
}
