ESP32 Pocket Info Hub 🚀

A multi-page, Wi-Fi-enabled dashboard built with an ESP32 and OLED display to fetch and show real-time data.

This project turns an ESP32 and a tiny OLED screen into a powerful "Pocket Dashboard." It connects to your Wi-Fi and cycles through multiple pages of live data pulled from various public APIs.

Features

Page 1: Live Clock ⏰

Syncs with an NTP (Network Time Protocol) server to display the highly accurate time and date.

Page 2: Weather Station ☀️

Pulls live temperature, humidity, and weather conditions (like "Clouds", "Rain") from the OpenWeatherMap API for any specified city.


Multi-Page Navigation

Uses four push buttons for Previous Page, Next Page, Refresh, and Select actions.

🛠️ Hardware Required

1 x ESP32 Dev Board (WROOM-32)

1 x SSD1306 0.96" I2C OLED Display (128x64)

4 x Tactile Push Buttons

1 x Breadboard

Jumper Wires

🔌 Connections & Wiring

I2C OLED Display


💻 Software & Setup

1. Arduino IDE

This project was written and compiled using the Arduino IDE.

2. Board Manager

Make sure you have the ESP32 boards added to your Arduino IDE. You can add them using this URL in Preferences:
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Select ESP32 Dev Module as your board.

3. Required Libraries

Install these libraries from the Arduino Library Manager (Tools > Manage Libraries...):

Adafruit_GFX

Adafruit_SSD1306

ArduinoJson (v6 or higher)

HTTPClient (Comes built-in with the ESP32 core)

4. Configuration

Before uploading the code, you must update the following variables in the .ino file:

// --- Configuration ---

// Wi-Fi Credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// OpenWeatherMap API Key
// Get one for free at [https://openweathermap.org/](https://openweathermap.org/)
String openWeatherApiKey = "YOUR_OPENWEATHERMAP_API_KEY";
String city = "Karaikal"; // Your city
String countryCode = "IN"; // Your country code


👥 Contributors

This project was built in collaboration by:

Jaikrishnan P (@Jaikrishnan-p15)

Yusuf (@yusuf0205)

Feel free to fork this repository, add new pages, and submit a pull request!
