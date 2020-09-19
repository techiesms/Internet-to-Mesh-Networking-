/*************************************************************
 * 
 *   This is the code for Internet to Mesh Project
 *   
 *   This code needs to be uploaded on the board from which 
 *   we will be receiving the data from Blynk app. 
 * 
 * 
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

    Code written by team techiesms 

    To watch the full tutorial video of this project, click the link below
    https://youtu.be/yZvhygHA_sw

 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

// Necesaary Libraries
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ArduinoJson.h>

// Serial2 pins of ESP32
#define RXD2 16
#define TXD2 17

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "Your_blynk_auth_token";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "SSID";
char pass[] = "PASS";

// Variables
int board;
int pin;
int pin_status;
String message = "";
bool messageReady = false;


// Data Coming fro Blynk App
BLYNK_WRITE(V1)
{
  board = 1;
  pin = 22;
  pin_status = param.asInt();
}

BLYNK_WRITE(V2)
{
  board = 2;
  pin = 22;
  pin_status = param.asInt();
}

//BLYNK_WRITE(V3)
//{
//  board = 1;
//  pin = 4;
//  pin_status = param.asInt();
//}
//
//BLYNK_WRITE(V4)
//{
//  board = 1;
//  pin = 15;
//  pin_status = param.asInt();
//}
//
//BLYNK_WRITE(V5)
//{
//  board = 2;
//  pin = 22;
//  pin_status = param.asInt();
//}
//
//BLYNK_WRITE(V6)
//{
//  board = 2;
//  pin = 2;
//  pin_status = param.asInt();
//}
//
//BLYNK_WRITE(V7)
//{
//  board = 2;
//  pin = 4;
//  pin_status = param.asInt();
//}
//
//BLYNK_WRITE(V8)
//{
//  board = 2;
//  pin = 15;
//  pin_status = param.asInt();
//}



void setup()
{
  // Debug console
  Serial.begin(115200); // For Debugging purpose
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // For sending data to another ESP32

  Blynk.begin(auth, ssid, pass); // Establishing Communication with Blynk Server
}

void loop()
{


  while (Serial2.available()) //
  {
    message = Serial2.readString();
    Serial.println(message);
    messageReady = true;
    Serial.print("Received message - "); Serial.println(message);
  }

  // Only process message if there's one
  if (messageReady)
  {
    // The only messages we'll parse will be formatted in JSON
    DynamicJsonDocument doc(1024); // ArduinoJson version 6+
    // Attempt to deserialize the message
    DeserializationError error = deserializeJson(doc, message);
    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      messageReady = false;
      return;
    }

    // If request is received from another ESP32 board
    if (doc["type"] == "request")
    {
      Serial.println("Received Request");
      doc["type"] = "response";
      // Get data from virtual pin
      doc["board_status"] = board;
      doc["led"] = pin;
      doc["status"] = pin_status;
      serializeJson(doc, Serial2); // Sending data to another ESP32
      Serial.println("Sending Data - "); serializeJson(doc, Serial); //{"type":"response","board_status":1/2,"led": pin_number, "status": 1/0}
    }
    messageReady = false;
  }

  Blynk.run(); // Handeling Blynk Services
}
