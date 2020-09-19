/*************************************************************
 * 
 *   This is the code for Internet to Mesh Project
 *   
 *   This code needs to be uploaded on the board on which  
 *   we will be receiving the data via Mesh using which 
 *   we will be controlling the appliances as well.
 *   
 *   This is the second receiver board
 * 
 * 
    Code written by team techiesms 

    To watch the full tutorial video of this project, click the link below
    https://youtu.be/yZvhygHA_sw

 *************************************************************/

// Necessary Libraries
#include "painlessMesh.h"
#include <ArduinoJson.h>
#include <DHT.h>


// WiFi Credentials
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

int  led;
int  led_status = 0;
int  board_status = 0;

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;


// User stub
void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage()
{
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg )
{
  //Deserializing
  String json;
  DynamicJsonDocument doc(1024);
  json = msg.c_str();
  DeserializationError error = deserializeJson(doc, json);
  Serial.println(json);
  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }
  board_status = doc["board"];
  led = doc["pin"];
  led_status = doc["status"];
  Serial.println(board_status);
  Serial.println(led);
  Serial.println(led_status);
  if (board_status == 2){
  digitalWrite(led, led_status);

  }
}
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
  Serial.begin(115200);

  pinMode(22, OUTPUT);

  digitalWrite(22,LOW);

  //mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // set before init() so that you can see startup messages
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {
  // it will run the user scheduler as well
  mesh.update();
    }
