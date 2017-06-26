#include <ESP8266WiFi.h>
#include "myconfig.h"
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>
 
#define PWM_VALUE 63
int gamma_table[PWM_VALUE+1] = {
    0, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 9, 10,
    11, 12, 13, 15, 17, 19, 21, 23, 26, 29, 32, 36, 40, 44, 49, 55,
    61, 68, 76, 85, 94, 105, 117, 131, 146, 162, 181, 202, 225, 250,
    279, 311, 346, 386, 430, 479, 534, 595, 663, 739, 824, 918, 1023
};

// RGB FET
#define redPIN    13
#define greenPIN  15
#define bluePIN   12


// W FET
#define w1PIN     14
#define w2PIN     4

// onbaord green LED D1
#define LEDPIN    5
// onbaord red LED D2
#define LED2PIN   1

// note 
// TX GPIO2 @Serial1 (Serial ONE)
// RX GPIO3 @Serial    


#define LEDoff digitalWrite(LEDPIN,HIGH)
#define LEDon digitalWrite(LEDPIN,LOW)

#define LED2off digitalWrite(LED2PIN,HIGH)
#define LED2on digitalWrite(LED2PIN,LOW)

// Start a TCP Server on port 5577
WiFiServer server(5577);
WiFiClient client;

const uint8_t CMD_ON[] = { 71, 23, 94};
const uint8_t CMD_OFF[] = { 71, 24, 95};
const uint8_t CMD_COLOR[] = { 31 };

int RED, GREEN, BLUE, WHITE;

void setup()
{
    // Setup console
  Serial1.begin(115200);
  delay(10);
  Serial1.println();
  Serial1.println();
  
  pinMode(LEDPIN, OUTPUT);  
  pinMode(LED2PIN, OUTPUT);  
  
  pinMode(redPIN, OUTPUT);
  pinMode(greenPIN, OUTPUT);
  pinMode(bluePIN, OUTPUT);
  pinMode(w1PIN, OUTPUT);
  pinMode(w2PIN, OUTPUT);    

  LEDoff;
  // We start by connecting to a WiFi network
  if(debugOutput){
    Serial1.println();
    Serial1.println();
    Serial1.print("Connecting to ");
    Serial1.println(ssid);
  }
  WiFi.begin(ssid, password);
  int maxWait = 500;
  while (WiFi.status() != WL_CONNECTED) {
    LED2off;
    delay(500);
    if(debugOutput) Serial1.print(".");
    if(maxWait <= 0)
     ESP.restart();
    maxWait--;
    LED2on;
  }
  if(debugOutput){
    Serial1.println("");
    Serial1.println("WiFi connected");  
    Serial1.println("IP address: ");
    Serial1.println(WiFi.localIP());
  }  
  delay(500);
  do_update();
  // Start the TCP server
  server.begin();
  server.setNoDelay(true);
  if(debugOutput){
    Serial1.println("");
    Serial1.println("TCP Socket opened, ready for action!");  
  } 
  delay(500); 
}


void loop()
{
  if(WiFi.status() != WL_CONNECTED) {
      ESP.restart();
      delay(100);
  }
  if(!server.hasClient())
  {
    return;
  }
  uint8_t readBuffer[8];
  int i = 0;
  client = server.available();
  if(debugOutput) Serial1.println("found a connection...");
  while (client.connected()) {     
      i = client.available();
      if(i > 0)
      {
        client.read(readBuffer,i);
        if(debugOutput){
          Serial1.print("received: ");
          for(int x = 0; x < i; x++)
          {
            Serial1.print("0x");Serial1.print(readBuffer[x],HEX);Serial1.print(" ");
          }
          Serial1.println("");
        }
        if( i == 3)
        {
          //ON Command can be ignored...
          if(memcmp(readBuffer, CMD_OFF,3))
          {
            analogWrite(redPIN,0);
            analogWrite(greenPIN,0);
            analogWrite(bluePIN,0);
            analogWrite(w1PIN,0);
            LEDoff;
          }
        }
        if(i  == 8 && memcmp(readBuffer,CMD_COLOR , 1) && checkChecksum(readBuffer))
        {
          // set value of RGB controller
          int red = map((int)readBuffer[1],0,255,0,PWM_VALUE); 
          red = constrain(red,0,PWM_VALUE);
          int green = map((int)readBuffer[2],0,255,0,PWM_VALUE);
          green = constrain(green, 0, PWM_VALUE);
          int blue = map((int)readBuffer[3],0,255,0,PWM_VALUE);
          blue = constrain(blue,0,PWM_VALUE);
          int w1 = map((int)readBuffer[4],0,255,0,1023); 
          w1 = constrain(w1,0,1023);
          
          RED = gamma_table[red];
          GREEN = gamma_table[green];
          BLUE = gamma_table[blue];
          WHITE = w1; 
          analogWrite(redPIN,RED);
          analogWrite(greenPIN,GREEN);
          analogWrite(bluePIN,BLUE);
          analogWrite(w1PIN,WHITE);
          if(RED-GREEN-BLUE-WHITE == 0)
            LEDoff;
          else
            LEDon;
        }
        i = 0;
      }
    }
  client.stop();
}

bool checkChecksum(uint8_t rBuffer[])
{
  uint8_t checkSum = 0;
  for(int i = 0; i < 7; i++)
    checkSum += rBuffer[i];
  checkSum %= 0x100;
  if(checkSum - rBuffer[7] == 0)
   return true;
  return false;
}

void do_update(){
  if(debugOutput) Serial1.println("do update");
  t_httpUpdate_return ret = ESPhttpUpdate.update(update_server, 80, update_uri, firmware_version);
  switch(ret) {
    case HTTP_UPDATE_FAILED:
        if(debugOutput) Serial1.println("[update] Update failed.");
        break;
    case HTTP_UPDATE_NO_UPDATES:
        if(debugOutput )Serial1.println("[update] no Update needed");
        break;
    case HTTP_UPDATE_OK:
        if(debugOutput) Serial1.println("[update] Update ok."); // may not called we reboot the ESP
        break;
  }
}
