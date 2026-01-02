/********************
 * Project : ESP32 Motion + Temperature Monitor
 * Author  : Milad Mohseni
 ********************/

#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// -------------------- Pin Definitions --------------------
#define PIR_PIN       4
#define TEMP_PIN      18
#define LED_PIN       2
#define BUZZER_PIN    15

// -------------------- OLED Configuration -----------------
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
#define OLED_ADDR     0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// -------------------- Temperature Sensor -----------------
OneWire oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);

// -------------------- PWM Settings (ESP32 New Core) ------
#define BUZZER_FREQ      2000   // Loud audible frequency
#define BUZZER_RES       8      // 0–255
#define BUZZER_DUTY_ON   220    // Very loud
#define BUZZER_DUTY_OFF  0

float temperatureC = 0.0;
bool motionDetected = false;

void setup() {
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  // Attach PWM directly to buzzer pin (NEW ESP32 API)
  ledcAttach(BUZZER_PIN, BUZZER_FREQ, BUZZER_RES);
  ledcWrite(BUZZER_PIN, BUZZER_DUTY_OFF);

  sensors.begin();

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  motionDetected = digitalRead(PIR_PIN);

  sensors.requestTemperatures();
  temperatureC = sensors.getTempCByIndex(0);

  // -------- Motion Logic --------
  if (motionDetected) {
    digitalWrite(LED_PIN, HIGH);
    ledcWrite(BUZZER_PIN, BUZZER_DUTY_ON);   // 🔊 Continuous loud beep
  } else {
    digitalWrite(LED_PIN, LOW);
    ledcWrite(BUZZER_PIN, BUZZER_DUTY_OFF);  // Silence
  }

  // -------- OLED Display --------
  display.clearDisplay();
  display.setTextSize(1);

  display.setCursor(0, 0);
  display.println("ESP32 MONITOR");

  display.setCursor(0, 16);
  display.print("Temp: ");
  display.print(temperatureC, 1);
  display.println(" C");

  display.setCursor(0, 32);
  display.print("Motion: ");
  display.println(motionDetected ? "DETECTED" : "IDLE");

  display.setCursor(0, 48);
  display.print("Buzzer: ");
  display.println(motionDetected ? "ON" : "OFF");

  display.display();

  delay(200);
}