
#define WEBDUINO_FAIL_MESSAGE "<h1>EPIC FAIL</h1>"

#include "SPI.h"
#include "Ethernet.h"
#include "WebServer.h"
//#include "avr/pgmspace.h" // new include



//At this tim, IP addresses must be statically set in code.  Future revisions should allow for DHCP 
static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
static uint8_t ip[] = { 192, 168, 10, 56 };
static uint8_t gateway[] = { 192, 168, 60, 1};	
static uint8_t subnet[] = { 255, 255, 255, 0 };
const char* password = "test";

#define PREFIX ""
WebServer webserver(PREFIX, 8080);



//Define the pins for this AB switch
int switchA = A0;
int switchB = A1;
int switchC = A2;
int switchD = A3;
int garageDoorSensor = A4;


int stateA = 0;
int stateB = 0;
int stateC = 0;
int stateD = 0;

int relayA = 2;
int relayB = 3;
int relayC = 4;
int relayD = 5;

void printPage(WebServer &server, WebServer::ConnectionType type)
{
  server.httpSuccess();
  // We'll always print the JSON state of all switches 
  server.print("{ ");
  server.print("GarageLights: " + String(stateA) + "; ");
  server.print("DrivewayLights: " + String(stateB) + "; ");
  server.print("GarageDoor: " + garageDoorStatus() + "; ");
  server.print("WalkwayLights: " + String(stateD) + " ");
  server.print("}");
  
}


void defaultCmd(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete)
{
  if (type == WebServer::GET)
  {
    bool authenticated = false;
    bool flipA = false;
    bool flipB = false;
    bool flipC = false;
    bool flipD = false;
    
    bool onA = false;
    bool onB = false;
    bool onD = false;
    
    bool offA = false;
    bool offB = false;
    bool offD = false;
    
    bool repeat = false;
    char name[16], value[16];
    
    URLPARAM_RESULT rc;
    
    if (strlen(url_tail))
    {
      
      while (strlen(url_tail))
      {
        rc = server.nextURLparam(&url_tail, name, 16, value, 16);
        if (strcmp(name, "auth") == 0 && strcmp(value, password) == 0) authenticated = true;
        
        if (strcmp(name, "flipA") == 0) flipA = true;
        if (strcmp(name, "flipB") == 0) flipB = true;
        if (strcmp(name, "flipC") == 0) flipC = true;
        if (strcmp(name, "flipD") == 0) flipD = true;
        
        if (strcmp(name, "onA") == 0) onA = true;
        if (strcmp(name, "onB") == 0) onB = true;
        if (strcmp(name, "onD") == 0) onD = true;
        
        if (strcmp(name, "offA") == 0) offA = true;
        if (strcmp(name, "offB") == 0) offB = true;
        if (strcmp(name, "offD") == 0) offD = true;
      
      }
    }
    
    
    if (authenticated && flipA) toggleA();
    if (authenticated && flipB) toggleB();
    if (authenticated && flipC) toggleC();
    if (authenticated && flipD) toggleD();
    
    if (authenticated && onA) turnOnA();
    if (authenticated && onB) turnOnB();
    if (authenticated && onD) turnOnD();
    
    if (authenticated && offA) turnOffA();
    if (authenticated && offB) turnOffB();
    if (authenticated && offD) turnOffD();
    
  }
      
  printPage(server, type); 
}

void setup()
{
  // set pins 0-8 for digital input
  pinMode(relayA, OUTPUT);
  pinMode(relayB, OUTPUT);
  pinMode(relayC, OUTPUT);
  pinMode(relayD, OUTPUT);
  
  pinMode(switchA, INPUT);
  digitalWrite(switchA, HIGH);
  pinMode(switchB, INPUT);
  digitalWrite(switchB, HIGH);
  pinMode(switchC, INPUT);
  digitalWrite(switchC, HIGH);
  pinMode(switchD, INPUT);
  digitalWrite(switchD, HIGH);
  pinMode(garageDoorSensor, INPUT);
  digitalWrite(garageDoorSensor, HIGH);
  
  Ethernet.begin(mac, ip, gateway, subnet);
  webserver.begin();

  webserver.setDefaultCommand(&defaultCmd);
  //webserver.addCommand("form", &formCmd);
}

void loop()
{
  char buff[64];
  int len = 64;

  /* process incoming connections one at a time forever */
  webserver.processConnection(buff, &len);
  
  checkButtons();
}


void checkButtons()
{
  if (digitalRead(switchA) == LOW)
  {
    delay(500);
    if (digitalRead(switchA) == LOW)
    {
      toggleA();
      delay(2000);
    }
    
  }
  
  if (digitalRead(switchB) == LOW)
  {
    delay(500);
    if (digitalRead(switchB) == LOW)
    {
      toggleB();
      delay(2000);
    }
  }
  
  if (digitalRead(switchC) == LOW)
  {
    delay(500);
    if (digitalRead(switchC) == LOW)
    {
      toggleC();
      delay(2000);
    }
  }
  
  if (digitalRead(switchD) == LOW)
  {
    delay(1000);
    if (digitalRead(switchD) == LOW)
    {
      toggleD();
      delay(2000);
    }
  }
  
}


void toggleA()
{
  if (stateA == 0)
    turnOnA();
  else
    turnOffA();
}

void turnOnA()
{
  digitalWrite(relayA, HIGH);
  stateA = 1;
}

void turnOffA()
{
  digitalWrite(relayA, LOW);
  stateA = 0;
}


void toggleB()
{
  if (stateB == 0)
    turnOnB();
  else
    turnOffB();
}

void turnOnB()
{
  digitalWrite(relayB, HIGH);
  stateB = 1;
}

void turnOffB()
{
  digitalWrite(relayB, LOW);
  stateB = 0;
}

void toggleC()
{
  
  // This is the garage door opener relay.  All we need to do is close the relay for about half a second each time
  digitalWrite(relayC, HIGH);
  delay(400);
  digitalWrite(relayC, LOW);  
  
  
}



void toggleD()
{
  if (stateD == 0)
    turnOnD();
  else
    turnOffD();
}

void turnOnD()
{
  digitalWrite(relayD, HIGH);
  stateD = 1;
}

void turnOffD()
{
  digitalWrite(relayD, LOW);
  stateD = 0;
}




String garageDoorStatus()
{
  if (digitalRead(garageDoorSensor) == LOW)
    return "OPEN";
   else
     return "CLOSED";
}


