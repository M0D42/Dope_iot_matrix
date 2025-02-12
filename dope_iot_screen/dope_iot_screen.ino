#include <ESP8266WiFi.h>  // Use <WiFi.h> for ESP32 or ESP8266WiFi.h for ESP8266
#include <ESP8266HTTPClient.h>  // For HTTP requests (use HTTPClient for ESP32)
#include <Max72xxPanel.h>  // Display library

// Wi-Fi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// Weather API details
const String api_key = "Api_Key";  // Replace with your API key
const String location = "country"; // Replace with your location
const String url = "https://api.weatherapi.com/v1/current.json?q=" + location + "&key=" + api_key;

// Define the pins for the LED and the MAX7219 matrix
const int dataIn = D1;
const int load = D2;
const int clk = D3;
Max72xxPanel matrix = Max72xxPanel(dataIn, clk, load, 4);  // 4 modules, adjust as necessary

// Initialize the purple LED pin
const int purpleLED = D4;

// Delay array
int delays[] = {1, 4, 10, 8, 9, 6, 11};

void setup() {
  // Start serial communication
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  
  // Initialize the LED pin
  pinMode(purpleLED, OUTPUT);
  digitalWrite(purpleLED, LOW);  // Turn off the LED initially

  // Initialize the matrix
  matrix.setIntensity(5);  // Adjust brightness (0-15)
  matrix.setRotation(0, false);  // No rotation
  matrix.fillScreen(LOW);  // Clear the display
}

void loop() {
  int delayTime = delays[random(0, 7)];  // Random delay from the array
  String tempText = fetchWeatherData();  // Fetch the weather data and get temperature text

  if (tempText.length() > 0) {
    // Scroll the temperature on the display
    for (int i = 0; i < tempText.length() * 6; i++) {
      matrix.fillScreen(LOW);
      matrix.setCursor(-i, 0);
      matrix.print(tempText);
      matrix.write();
      delay(100);  // Scroll speed
    }
  }

  delay(delayTime * 60000);  // Wait for the specified delay in minutes
}

String fetchWeatherData() {
  HTTPClient http;
  http.begin(url);  // Initialize HTTP request

  int httpCode = http.GET();  // Send GET request
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();  // Get the response payload

    // Parse the JSON response to extract the temperature
    int startIdx = payload.indexOf("\"temp_c\":");
    if (startIdx == -1) {
      Serial.println("Failed to parse temperature from response");
      return "";
    }

    startIdx += 9;  // Move to the actual temperature value
    int endIdx = payload.indexOf(",", startIdx);
    String tempString = payload.substring(startIdx, endIdx);
    String tempText = "Temperature: " + tempString + "°C";
    return tempText;
  } else {
    Serial.println("Failed to fetch weather data.");
    return "";
  }
}
