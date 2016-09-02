
/*
Box model.


FoosBox deep sleeps.
Interrupt wakeup on rst pin by vibrate/shake goal.
Turn off interrupts.
Connect to internet. Ping server with side that scored.
Turn on interrupt. Go back to sleep.


Server updates homepage.

*/

#include <ESP8266WiFi.h>
#include "FS.h"

#define NORTH 0
#define SOUTH 1
#define INTERUPT_NORTH 13
#define INTERUPT_SOUTH 14

#define ESP8266_LED 2 // THIS IS CORRECT, HIGH == OFF
#define FLASH_BTN_PIN 0  // Correct gpio for flash button

const char* ssid     = "GarminGuests";
const char* password = "";
const char* host = "kangasp.com";
const char* test_host = "lit-sea-41725.herokuapp.com";
volatile int north_state = 0;
volatile int south_state = 0;
volatile int prev_north_state = 0;
volatile int prev_south_state = 0;
int sent = 0;



void setup() {
  Serial.begin(115200);
  delay(5);
  Serial.println("Starting up");
  Serial.println("Attaching interrupts");
  pinMode(INTERUPT_NORTH, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(INTERUPT_NORTH), north_isr, FALLING);
  pinMode(INTERUPT_SOUTH, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(INTERUPT_SOUTH), south_isr, FALLING);

  // pinMode(FLASH_BTN_PIN, OUTPUT);
  attachInterrupt(FLASH_BTN_PIN, south_isr, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(FLASH_BTN_PIN), south_isr, CHANGE);
  // pinMode(FLASH_BTN_PIN, INPUT_PULLUP);
  // pinMode(FLASH_BTN_PIN, INPUT);

  pinMode(ESP8266_LED, OUTPUT); 
  
  if (!SPIFFS.begin())
    {
    Serial.println("Failed to mount file system");
    return;
    }
  connect_wifi();
}


void loop() {
  String north_url = "/north";
  String south_url = "/south";
  // WiFi.forceSleepBegin(1000); // useconds
  delay(10);
/*
  if(HIGH == digitalRead(FLASH_BTN_PIN))
     {
     digitalWrite(ESP8266_LED, HIGH);
     }
  else
     {
     digitalWrite(ESP8266_LED, LOW);
     if( !sent )
       {
       sent = 1;
       test_ping(north_url);
       }
     }
*/
  if( north_state != prev_north_state )
    {
    Serial.print("North State changed: ");
    Serial.println(north_state);
    prev_north_state = north_state;
    // update_score(NORTH, north_state);
    }
  if( south_state != prev_south_state )
    {
    Serial.print("South State changed: ");
    Serial.println(south_state);
    if( south_state > 4 )
      {
      sent = 0;
      south_state = 0;
      }
    if( !sent )
      {
      sent = 1;
      test_ping(south_url);
      }
    prev_south_state = south_state;
    // update_score(SOUTH, south_state);
    }
}


void north_isr() {
    north_state++;
}

void south_isr() {
    south_state++; 
}

void update_score(int side, int score) {
  WiFi.forceSleepWake();
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/input/";
  if( side == NORTH )
    {
    url += "north/";
    // url += "score";
    }
  else
    {
    url += "south";
    }
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}


void connect_wifi() {
  int i;
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  i = 0;
  while (WiFi.status() != WL_CONNECTED)
    {
    delay(500);
    Serial.print(".");
    if( i++ > 10 )
      {
      ESP.restart();
      }
    }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  digitalWrite(ESP8266_LED, HIGH); 
}


void test_ping(String url) {
  Serial.println("Test Ping");
  // WiFi.forceSleepWake();
  Serial.print("connecting to ");
  Serial.println(test_host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(test_host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + test_host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  // client.stop();
}




/*
To think about

bool increment_failed_wifi() {
  File configFile = SPIFFS.open("/fail.bin", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }

  json.printTo(configFile);
  return true;
}

*/


