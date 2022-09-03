#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <WiFi.h>
#include <ThingSpeak.h>

#define i2c_Address 0x3c
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SENSOR_PIN 26

const char* ssid = "Raj";   
const char* password = "raju1234";
int writeChannelID = 1837480;
char writeAPIKey[] = "81PQDLQVBE6QY26J";
char readAPIKey[] = "FXUMI21X9IO5UIIW";

WiFiClient client;

long currentMillis = 0;
long previousMillis = 0;
int interval = 1000;
volatile byte pulseCount;
float calibrationFactor = 4.5;
byte pulse1Sec = 0;
float flowRate;
unsigned long flowMilliLitres;
unsigned int totalMilliLitres;
float flowLitres;
float totalLitres;

Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void IRAM_ATTR pulseCounter()
{
  pulseCount++;
}

void setup()   {

  Serial.begin(115200);

  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      Serial.println("Couldn't connect...");
      delay(200);
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("Connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }

  ThingSpeak.begin(client);
  
  delay(250);
  display.begin(i2c_Address, true);
  display.display();

  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);

  delay(2000);
  display.clearDisplay();
  
  pinMode(SENSOR_PIN, INPUT_PULLUP);
  pulseCount = 0;
  flowRate = 0.0;
  flowMilliLitres = 0;
  totalMilliLitres = 0;
 
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), pulseCounter, FALLING);
}


void loop() {
  
  currentMillis = millis();
//  if (currentMillis - previousMillis > interval) 
//  {
    pulse1Sec = pulseCount;
    pulseCount = 0;
    flowRate = ((1000.0 / (millis() - previousMillis)) * pulse1Sec) / calibrationFactor;
    previousMillis = millis();
    flowMilliLitres = (flowRate / 60) * 1000;
    flowLitres = (flowRate / 60);
    totalMilliLitres += flowMilliLitres;
    totalLitres += flowLitres;

  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
    
    display.print("Flow Rate: ");
    display.println(float(flowRate));
    Serial.println(float(flowRate));  
    display.print("Total Flow: ");
    display.println(totalLitres);
    Serial.println(totalLitres);
    display.display();
    delay(2000);
    display.clearDisplay();

    ThingSpeak.setField(1, flowRate);
    ThingSpeak.setField(2, totalLitres);
    
    int y = ThingSpeak.writeFields(writeChannelID, writeAPIKey); 
    if(y == 200) {
        Serial.println("worked"); 
    }
//  }
}
