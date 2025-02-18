#include <ArduinoJson.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <HTTPClient.h>
#include <WiFi.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
int delays[] = {10,14,15,12,17,19,11};
String url = "https://zenquotes.io/api/random";
String payload = "";

  String quote = ""; // "The moment you doubt whether you can fly, you cease for ever to ...
  String author = "";
// Uncomment according to your hardware type
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW

#define MAX_DEVICES 4
#define CS_PIN 5
MD_Parola Display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
unsigned long lastActionTime = 0;  // Last time action was taken
unsigned long numdInterval = 0;    // Interval duration based on numd value

void setup() {
    Serial.begin(115200);
    WiFiManager wm;

    bool res;
    // res = wm.autoConnect(); // auto generated AP name from chipid
    // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
    res = wm.autoConnect("AutoConnectAP","password"); // password protected ap

    if(!res) {
        Serial.println("Failed to connect");
        // ESP.restart();
    } 
    else {
        //if you get here you have connected to the WiFi    
        Serial.println("connected...yeey :)");
    }
    
  Display.begin();
  Display.setIntensity(9);
  Display.displayClear();
  Display.write("matrix");

  delay(3000);
  Display.displayClear();
  delay(200);
}

void loop() {
    unsigned long currentMillis = millis();  // Get current time

    // If enough time has passed based on numdInterval, perform the actions
    if (currentMillis - lastActionTime >= numdInterval) {
        lastActionTime = currentMillis;  // Update the last action time
        int numd = delays[random(0, 6)];  // Random value from the delays array
        numdInterval = numd * 6000;  // Set the interval based on numd (in milliseconds)

        fetchapi();  // Fetch new quote
        parseapi();  // Parse the quote
        screenAnimate();  // Animate the quote on the LED matrix
    }

    // Continuously animate the scrolling text
    if (Display.displayAnimate()) {
        Display.displayReset();  // Reset the display once the scrolling is complete
    }
}


void fetchapi(){
      if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();
    if(httpCode > 0){
      payload = http.getString();
      Serial.println("New payload");
      Serial.println(payload);
          }else{
      Serial.println("request failed :<");
    }

  }else{
   Serial.println("Wifi disconected");
  }
  }

void parseapi(){
  DynamicJsonDocument doc(192);
  DeserializationError error = deserializeJson(doc,payload);
  if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}
  JsonObject quoteOb = doc[0];
   quote = quoteOb["q"].as<String>();; // "The moment you doubt whether you can fly, you cease for ever to ...
   author = quoteOb["a"].as<String>();;

  Serial.println(quote);
  Serial.println(author);
  }
// Animate the quote on the LED matrix
void screenAnimate(){
  Serial.println("Screen Started");

  // Combine quote and author into a single string
  String msg = quote + " by " + author ;

  // Debug: Check if the message is too short or empty
  
  Serial.println(msg.c_str());
  
    // Scroll the message across the LED matrix
    Display.displayScroll(msg.c_str(), PA_RIGHT, PA_SCROLL_LEFT, 70);
    Display.displayClear();
    // Once the message finishes scrolling, reset the display
    delay(2000);
   
}
