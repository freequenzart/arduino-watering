
// libs
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// screen setup
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// powerbank setup
#define POWER_ON_TIME 500
#define POWER_WAIT_TIME 10000

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

unsigned long last;
unsigned long lastPowerOn = 0;
bool pumpActive = false;

// the setup function runs once when you press reset or power the board
void setup() {
  last = millis();
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  
  display.setTextSize(1);
  display.setTextColor(WHITE);
}

// the loop function runs over and over again forever
void loop() {
  int sensorValue       = analogRead(A0);
  int lowerThreshold    = analogRead(A1);
  int upperThreshold    = analogRead(A2);
  unsigned long current = millis();
  unsigned long off     = last;
  bool powerActive      = false;

  
  if(sensorValue >= upperThreshold) {
    digitalWrite(2, HIGH);
    pumpActive = true;
    last = current;
  }
  
  if(sensorValue <= lowerThreshold){
    digitalWrite(2, LOW);
    pumpActive = false;
  }

  if(!pumpActive) {
    if((current - lastPowerOn) > POWER_WAIT_TIME) {
      lastPowerOn = current;
      digitalWrite(3, HIGH);
      delay(POWER_ON_TIME);
      digitalWrite(3, LOW);

      powerActive = true;
    }
  }
  
  display.clearDisplay();

  // Display wetness
  display.setCursor(0, 5); 
  display.println("Feuchte:"); // moistness
  display.setCursor(72, 5); 
  display.println(sensorValue);
  
  // Display last active
  display.setCursor(0, 20); 
  display.println("Aktiv vor:"); // active ###min ago
  display.setCursor(72, 20);
  
  int time    = (current - last) / 60000;
  String unit = String("min");
  display.println(time + unit);
  
  // Display threshold active
  display.setCursor(0, 35); 
  display.println("Pumpstart:"); // pump starts at this level of moistness
  display.setCursor(72, 35);
  display.println(upperThreshold);
  
  display.setCursor(0, 50); 
  display.println("Pumpende:"); // pump ends at this level of moistness
  display.setCursor(72, 50);
  display.println(lowerThreshold);

  if(powerActive) {
    display.setCursor(110, 50);
    display.println("+P");
  }
  
  display.display();
  
  delay(1000);
}