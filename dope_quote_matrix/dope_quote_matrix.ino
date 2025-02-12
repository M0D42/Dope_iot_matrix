#include <ESP8266WiFi.h>        // For ESP8266 (use <WiFi.h> for ESP32)
#include <ESP8266HTTPClient.h>   // For HTTP requests (use HTTPClient for ESP32)
#include <Max72xxPanel.h>        // Display library
#include <ArduinoJson.h>         // ArduinoJson library for JSON parsing

// Wi-Fi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// API details
const String type = "random";  // Replace with your desired API type
const String url = "https://zenquotes.io/api/" + type;

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
  matrix.setIntensity(10);  // Adjust brightness (0-15)
  matrix.setRotation(0, false);  // No rotation
  matrix.fillScreen(LOW);  // Clear the display
}

void loop() {
  int delayTime = delays[random(0, 7)];  // Random delay from the array
  String quoteText = fetchQuoteData();  // Fetch the quote data and get the quote text

  if (quoteText.length() > 0) {
    // Scroll the quote on the display
    for (int i = 0; i < quoteText.length() * 6; i++) {
      matrix.fillScreen(LOW);
      matrix.setCursor(-i, 0);
      matrix.print(quoteText);
      matrix.write();
      delay(100);  // Scroll speed
    }
  }

  delay(delayTime * 60000);  // Wait for the specified delay in minutes
}

String fetchQuoteData() {
  HTTPClient http;
  http.begin(url);  // Initialize HTTP request

  int httpCode = http.GET();  // Send GET request
  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();  // Get the response payload

    // Create a JSON document and parse the payload
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.println("Failed to parse JSON response.");
      return "";
    }

    // Extract the quote text from the JSON
    String quote = doc[0]["q"].as<String>();  // Assuming the JSON array format is {"q": "quote"}
    String author = doc[0]["a"].as<String>(); // Assuming the JSON array format is {"a": "author"}

    String quoteText = "\"" + quote + "\" - " + author;  // Format the quote text
    return quoteText;
  } else {
    Serial.println("Failed to fetch data.");
    return "";
  }
}
