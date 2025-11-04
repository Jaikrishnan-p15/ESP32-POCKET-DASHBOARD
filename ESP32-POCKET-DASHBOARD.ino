#include <WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "time.h"
#include <HTTPClient.h>   // <-- NEW: For making web requests
#include <ArduinoJson.h> // <-- NEW: For parsing JSON data

// --- Wi-Fi & NTP Configuration ---
const char* ssid = "YOUR_SSID";
const char* password = "PASSWORD";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 19800; // IST +5:30
const int   daylightOffset_sec = 0;

// --- NEW: OpenWeatherMap Config ---
String apiKey = "API_KEY";
String city = "CITY_NAME; // e.g., "Puducherry"
String units = "metric"; // For Celsius

// --- NEW: Weather Data Variables ---
// We store weather data globally so it stays on screen
String weatherTemp = "--";
String weatherHumidity = "--";
String weatherDescription = "Loading...";

// --- NEW: Weather Update Timer ---
unsigned long lastWeatherUpdate = 0;
// OpenWeatherMap free plan update interval (10 minutes)
const long weatherUpdateInterval = 600000; // 10 * 60 * 1000 ms

// --- OLED Display ---
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// --- Button Pins ---
#define BUTTON_PREV 12
#define BUTTON_NEXT 13
#define BUTTON_REFRESH 14
#define BUTTON_SELECT 27

// --- Page Navigation ---
int currentPage = 1;
int totalPages = 2; 
bool buttonPressed = false;

// --- NEW: Function Prototypes ---
void getWeatherData();


// --- Setup ---
void setup() {
  Serial.begin(115200);

  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }

  // Init Buttons
  pinMode(BUTTON_PREV, INPUT_PULLUP);
  pinMode(BUTTON_NEXT, INPUT_PULLUP);
  pinMode(BUTTON_REFRESH, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);

  // Show connecting message
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Connecting to WiFi...");
  display.display();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }

  // Init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // --- NEW: Get initial weather data ---
  getWeatherData();
}

// --- Page 1: Clock Display Function ---
void displayClock() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Failed to get time.");
    display.display();
    return;
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(18, 10);

  char timeString[9];
  strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeinfo);
  display.println(timeString);

  display.setTextSize(1);
  display.setCursor(22, 40);

  char dateString[11];
  strftime(dateString, sizeof(dateString), "%d-%m-%Y", &timeinfo);
  display.println(dateString);
}

// --- NEW: Page 2: Weather Display Function ---
void displayWeather() {
  // --- NEW: Auto-refresh weather data every 10 mins ---
  if (millis() - lastWeatherUpdate > weatherUpdateInterval) {
    getWeatherData();
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  // Display City
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Weather in ");
  display.println(city);

  // Display Temperature
  // Display Temperature
  display.setTextSize(2);
  display.setCursor(10, 20);
  display.print(weatherTemp);
  display.write(247); // This is the code for the '°' degree symbol
  display.print("C");

  // Display Humidity & Description
  display.setTextSize(1);
  display.setCursor(0, 45);
  display.print("Humidity: ");
  display.print(weatherHumidity);
  display.println("%");
  
  display.setCursor(0, 55);
  display.print(weatherDescription);
}

// --- NEW: Function to Fetch Weather Data ---
void getWeatherData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    // Construct the API URL
    String url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=" + units;

    Serial.print("Fetching URL: ");
    Serial.println(url);

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) { // Check for a successful response
      String payload = http.getString();
      Serial.println("API Response:");
      Serial.println(payload);

      // Allocate a JSON document
      // Use ArduinoJson Assistant (v6) to calculate size: https://arduinojson.org/v6/assistant/
      StaticJsonDocument<1024> doc; 

      // Deserialize the JSON document
      DeserializationError error = deserializeJson(doc, payload);
      if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        weatherDescription = "JSON Parse Error";
        return;
      }

      // Extract data
      float temp = doc["main"]["temp"];
      int humidity = doc["main"]["humidity"];
      // weather[0].main (e.g., "Clouds", "Rain")
      const char* description = doc["weather"][0]["main"]; 

      // Update global variables
      weatherTemp = String(temp, 1); // Format to 1 decimal place
      weatherHumidity = String(humidity);
      weatherDescription = String(description);
      
      lastWeatherUpdate = millis(); // Reset timer

    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpCode);
      weatherDescription = "API Error " + String(httpCode);
    }

    http.end(); // Free the resources
  } else {
    Serial.println("WiFi not connected");
    weatherDescription = "WiFi Error";
  }
}


// --- Button Check Function ---
void checkButtons() {
  // Check NEXT button
  if (digitalRead(BUTTON_NEXT) == LOW) {
    delay(50); 
    if (digitalRead(BUTTON_NEXT) == LOW && !buttonPressed) {
      currentPage++;
      if (currentPage > totalPages) currentPage = 1;
      buttonPressed = true;
      Serial.print("Button Next: Page "); Serial.println(currentPage);
    }
  } 
  // Check PREV button
  else if (digitalRead(BUTTON_PREV) == LOW) {
    delay(50);
    if (digitalRead(BUTTON_PREV) == LOW && !buttonPressed) {
      currentPage--;
      if (currentPage < 1) currentPage = totalPages;
      buttonPressed = true;
      Serial.print("Button Prev: Page "); Serial.println(currentPage);
    }
  }
  // --- NEW: Check REFRESH button ---
  else if (digitalRead(BUTTON_REFRESH) == LOW) {
    delay(50);
    if (digitalRead(BUTTON_REFRESH) == LOW && !buttonPressed) {
      if (currentPage == 2) { // Only refresh if on weather page
        Serial.println("Button Refresh: Fetching weather...");
        weatherDescription = "Refreshing...";
        display.display(); // Show "Refreshing..." immediately
        getWeatherData();
      }
      buttonPressed = true;
    }
  }
  // Check if buttons are released
  else if (digitalRead(BUTTON_NEXT) == HIGH && digitalRead(BUTTON_PREV) == HIGH && digitalRead(BUTTON_REFRESH) == HIGH) {
    buttonPressed = false; // Reset flag
  }
}


// --- Main Loop ---
void loop() {
  checkButtons();

  switch (currentPage) {
    case 1:
      displayClock();
      break;
    case 2:
      displayWeather();
      break;
  }
  
  display.display();

  if (currentPage == 1) {
    delay(1000); // Clock needs 1-sec update
  } else {
    delay(100); // Other pages are event-driven
  }
}
