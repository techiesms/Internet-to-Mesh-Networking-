/*************************************************************
 * 
 *   This is the code for Internet to Mesh Project
 *   
 *   This code needs to be uploaded on the board from which 
 *   we will be receiving the data from another ESP32 board 
 *   via UART (TX2,RX2)
 * 

    Code written by team techiesms 

    To watch the full tutorial video of this project, click the link below
    https://youtu.be/yZvhygHA_sw

 *************************************************************/
// Necessary Libraries
#include "painlessMesh.h"
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>


// Serial2 pins of ESP32
#define RXD2 16
#define TXD2 17

// WiFi Credentials for Mesh Networking
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555


// Variables
int led;
int led_status;
int board_status;
int board;
int pin;
int pin_status;
bool message_ready = true;
bool message_received = false;
String message = "";


Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;


// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain
void send_request() ;

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
Task taskSendRequest( TASK_SECOND * 1 , TASK_FOREVER, &send_request );

void sendMessage()
{

  DynamicJsonDocument doc(1024);
  doc["board"] = board_status;
  doc["pin"] = led;
  doc["status"] =  led_status;
  //doc["status_1"] = led_status_1;
  String msg ;
  serializeJson(doc, msg);
  mesh.sendBroadcast( msg );
  Serial.print("Mesh Broadcast - "); Serial.println(msg);

 // taskSendMessage.setInterval((TASK_SECOND * 1));
}


// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {}

void newConnectionCallback(uint32_t nodeId) {
  //Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  //Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  //Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
  Serial.begin(115200);  // For Debugging purpose
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2); // For sending data to another ESP32

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  userScheduler.addTask( taskSendRequest );
  taskSendMessage.enable();
  taskSendRequest.enable();
  // timer.setInterval(1000L, send_request);
}

void loop()
{

  if (Serial2.available())
  {
    message = Serial2.readString();
    message_ready = true;
  }
  Serial.print("Received Message - "); Serial.println(message);
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, message);
  board_status = doc["board_status"];
  led = doc["led"];
  led_status = doc["status"];


  // it will run the user scheduler as well
  mesh.update();
  //timer.run();
}

void send_request()
{
  DynamicJsonDocument doc_request(1024);
  doc_request["type"] = "request";  
  Serial.print("Sending Request - "); serializeJson(doc_request, Serial); //{"type":"request"}
  serializeJson(doc_request, Serial2);
  //taskSendMessage.setInterval((TASK_SECOND * 1));
}
