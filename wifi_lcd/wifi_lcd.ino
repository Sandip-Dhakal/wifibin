#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#define ARRAYSIZE 7

extern "C" {
  #include "user_interface.h"
}

const char* ssid = "dayprtfzpew-a2adp";
const char* password = "possible99";
String payload;

DynamicJsonBuffer jsonBuffer;
String results[ARRAYSIZE]; 
String passwords;

unsigned long previousMillis = 0;        
const long interval = 2000;           
int clientCounter = 0;

LiquidCrystal_I2C lcd(0x3F,16,2);  //LCD address 0x37 for a 16 chars and 2 line display
signed int analogCurrent, analogInitial = 0;

void setup()
{
  lcd.init();                      // initialize the lcd 
  lcd.backlight();
   lcd.setCursor(0, 0);
  lcd.print("Gionee...      ");
   lcd.setCursor(0, 1);
  lcd.print("My City My Pride");
  analogInitial = readADC();

  Serial.begin(115200);
  delay(10);

  Serial.println("No. of clients: "+clientCounter);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid);
 
   while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  wifibin();
}


void loop()
{
  lcd.setCursor(0, 0);
  lcd.print("Distance: ");
  lcd.print(readADC());
  lcd.print(" cm ");
  delay(200);
  
  switch(checkTrigger()){
  case 1:
     lcd.setCursor(0, 1);
     lcd.print("Code: "+results[clientCounter]+"      ");
     clientCounter += 1;
     delay(5000);
    break;
  case 0:
  
  lcd.setCursor(0, 1);
    lcd.print("Dustbin Jammed! ");
    delay(2000);
    break;
  default:
     lcd.setCursor(0, 1);
    lcd.print("No Request!!    ");
    break;
}

  
}

int checkTrigger(){
  analogCurrent = readADC();
   
  if(abs(analogInitial - analogCurrent) >= 7){
      lcd.setCursor(0, 1);
      lcd.print("Wait...         ");
  
      delay(3000);
      analogCurrent = readADC();
      if(abs(analogCurrent - analogInitial) <= 5){
         
        return 1;
      }else{
        return 0;  
      }
    }
  return 2;
}

int readADC(){
  float volts = (analogRead(A0)*5.0)/1024;   // value from sensor * (5/1024) - if running 3.3.volts then change 5 to 3.3
  float distance = 27*pow(volts, -1.10);          // worked out from graph 65 = theretical distance / (1/Volts)S - luckylarry.co.uk
  return int(distance);
}

void wifibin(){
   if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
    
    String url = "http://192.168.100.110/wifibin/pass.txt";
    
    http.setTimeout(2000);
    http.begin(url);  //Specify request destination
    int httpCode = http.GET();   //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
 
    }else{
      Serial.println("Server error");
      return;
    }
 
    http.end();   //Close connection
    JsonObject& root = jsonBuffer.parseObject(payload);

    for (int i =0; i< ARRAYSIZE; i++) {
      results[i] = root["codes"][i].as<String>();
         
    Serial.println(results[i]);
    
    }
      
 delay(3000);    
  }
}

