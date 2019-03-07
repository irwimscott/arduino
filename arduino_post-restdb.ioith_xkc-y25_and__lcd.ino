//ESP IMPORTS
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR 0x3F
#define BACKLIGHT_PIN 3
#define En 2
#define Rw 1
#define Rs 0
#define D4 4
#define D5 5
#define D6 6
#define D7 7

LiquidCrystal_I2C lcd(I2C_ADDR,En,Rw,Rs,D4,D5,D6,D7);

const int pinoSensor = 3; //PINO DIGITAL UTILIZADO PELO SENSOR - AQUI GPIO3 = TX DO ESP8266-01

// Replace with your network credentials
const char* ssid = "MYSSID";
const char* password = "MYSSIDPASSWD";

void setup(){  

  Wire.begin(2, 0); //SDA-2 , SCL-0
  lcd.begin (16,2);
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(HIGH);
  
  lcd.setCursor(0,0);
  lcd.print("LIGANDO LCD");
  delay(1000);
  
  Serial.begin(115200);
  //pinMode(pinoSensor, INPUT); //DEFINE O PINO COMO ENTRADA
  pinMode(pinoSensor, INPUT_PULLUP);
  WiFi.begin(ssid, password);   //WiFi connection
 
  while (WiFi.status() != WL_CONNECTED) {  //Wait for the WiFI connection completion
    
    delay(500);
    Serial.println("Waiting for connection");
    
        lcd.setCursor(0,0);
        lcd.print("Wait SSID...");
        lcd.setCursor(0,1);
        String stringOne = "SSID: ";
        String finaltext = stringOne + ssid;
        lcd.print(finaltext);
       
  }
  
    Serial.println("");
    Serial.print("Connected to ");
    
        lcd.setCursor(0,0);
        lcd.print("My server ip is:");
        lcd.setCursor(0,1);
        lcd.print("                ");
        delay(3000);
        
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

        lcd.setCursor(0,1);
        lcd.print(WiFi.localIP());
        delay(5000);
        
}

void cofee_ready(){

      Serial.println("END:AGUA BAIXOU DE NIVEL");
           
            lcd.setCursor(0,0);
            lcd.print("Nivel baixou!   ");
            lcd.setCursor(0,1);
            lcd.print("                ");   
      
          HTTPClient http;    //Declare object of class HTTPClient
          http.begin("https://restdb.io/rest/MYDATABASE", "14 13 5e 9e ef 1f db bc ea 58 bc c6 b3 a8 e2 6d d6 28 1e 70");
          http.addHeader("Content-Type", "application/json");
          http.addHeader("x-apikey", "XXXXXXXXXXXXXXXXXXXXX");
          String postMessage = "{\"machine_code\": 1, \"status\": 1}";
          int httpCode = http.POST(postMessage);
          Serial.print(postMessage);
          Serial.print("http result:");
          Serial.println(httpCode);
          http.writeToStream(&Serial);
          String payload = http.getString();
          http.end();
          delay(1000);
}

void loop(){

    lcd.setCursor(0,0);
    lcd.print("Lendo o sensor..");
    lcd.setCursor(0,1);
    lcd.print("                ");
        
    if(digitalRead(pinoSensor) == HIGH){ //SE A LEITURA DO PINO FOR IGUAL A HIGH, INFORMA QUE O LIQUIDO FOI DETECTADO.
      
      Serial.println("START:AGUA DETECTADA - Notificando e aguardando o reservatorio baixar.");
        lcd.setCursor(0,0);
        lcd.print("Agua Detectada..");
        lcd.setCursor(0,1);
        lcd.print("                ");       
          
          HTTPClient http;    //Declare object of class HTTPClient
          http.begin("https://restdb.io/rest/MYDATABASE", "14 13 5e 9e ef 1f db bc ea 58 bc c6 b3 a8 e2 6d d6 28 1e 70");
          http.addHeader("Content-Type", "application/json");
          http.addHeader("x-apikey", "XXXXXXXXXXXXXXXXXX");
          String postMessage = "{\"machine_code\": 1, \"status\": 0}";
          int httpCode = http.POST(postMessage);
          Serial.print(postMessage);
          Serial.print("http result:");
          Serial.println(httpCode);
          http.writeToStream(&Serial);
          String payload = http.getString();
          http.end();
          delay(1000);
       
            while(digitalRead(pinoSensor) == HIGH){
                  Serial.println("INFO:AGUARDANDO O RESERVATORIO BAIXAR");
                  delay(1000);
            }
            cofee_ready(); //INVOCO A FUNCAO QUE INFORMA A DATABASE REST QUE O NIVEL DA AGUA BAIXOU
    }
}
