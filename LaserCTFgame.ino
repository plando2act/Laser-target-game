//***CTF Laser Target               ***
//***Used in November 201           ***
//***2 teams can play on 2 devices  ***
//***While flashing uncomment blocks***
//***that deal with wifi for A or B ***
//***Version 1.0 - By Bob           ***

//**START Declerations and includes***********
//*** HARDWARE
//Pin D1 = SDC of OLED.096 display
//Pin D2 = SDA of OLED.096 display
//Pin D8 Clock laser detector - used for inputting values
//Pin D5 Bit0 Least Significant Byte
//Pin D6 Bit1 
//Pin D7 Bit2 Most Significant Byte
//Avoid D0 and D4 due to NodeMCU onboard leds
//Avoid D3 because it will mess up the bootsequence of the NodeMCU

//*** CTF FLAG ****
const char *FLAG = "TECH4952"; //Not more than 10 positions - shared by team A and B

//For ESP8266 WiFi
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
const char *ssid =     "LaserCTF_A";    //team A box
const char *password = "Ethanhunt";     //team A box
//const char *ssid =     "LaserCTF_B";  //team B box change while flashing the other set
//const char *password = "Benjidunn";   //team B box change while flashing the other set

//For webserver
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);

//For AP IP stack 
IPAddress local_IP(192,168,42,42);
IPAddress gateway(192,168,42,42);
IPAddress subnet(255,255,255,0);

//For Display the Adafruit driver files were used. Thank you for all that magnificent documentation and energy.
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//For Laser Target program
//int Sequence [] = { 0,1,2,3,4,5,6,7 } ;       // Change if you like
//int Sequence [] = { 0,2,4 } ;                 // Debug because you need a short sequence with easy numbers
int Sequence [] = { 1,0,2,4,7,5,3,7,4,2,7,1 } ; // Team A and B share the same problem
const int Sequence_SIZE = sizeof(Sequence) / sizeof(int);
int Stage = 0;
int Laserbits = 0;
int Laserbits_again = 0;
int randNumber =1;
int inputClock = D8;  // Clock laser detector
int inputBit0  = D5;  // set pin D5 as input 
int inputBit1  = D6;  // set pin D6 as input
int inputBit2  = D7;  // set pin D7 as input
int LED        = D0;  // prepare to use onboard LED 
int klok  = 0;        // variable to store the read value
int bit0  = 0;        // variable to store the read value
int bit1  = 0;        // variable to store the read value
int bit2  = 0;        // variable to store the read value
long measuretick = 0    ;              // the last time the range was checked
long refresh_measuretick = 1000 ;      // at least 1000 ms before next measurement call
boolean newtick = false         ;      // variable to indicate a tick
boolean armed = true            ;      // variable to indicate if the challenge is still on

//**END Declerations and includes***********


//****************RUN once***********
void setup() {
  delay(1000);
  Serial.begin(115200); // let this run also in production mode Never know where you will need this
  Serial.println();
  //Start AP in ESP8266
  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
  Serial.print("Starting soft-AP ... ");
  Serial.println(WiFi.softAP("ESPsoftAP_01") ? "Ready" : "Failed!");
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  //Start WebServer in ESP8266
  server.on("/", handleRoot);
  server.on("/flag", [](){
    server.send(200, "text/plain", "really.. no.. just no - nice try but now get on with the challenge");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  
  //Start Display attached to ESP8266
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  delay(1000);
  oled_AP();
  delay(5000);
  //Define pins
  pinMode (inputClock, INPUT); 
  pinMode (inputBit0, INPUT);  
  pinMode (inputBit1, INPUT); 
  pinMode (inputBit2, INPUT);  
  //Reset measuretick
  measuretick = millis();
}
//****************END of RUN once***********


//**START***MAIN LOOP*****************************************************************************
void loop() {
  server.handleClient();
//  klok = digitalRead(inputClock);  // read the Clock bit to see if someone entered data 
  if (digitalRead(inputClock) == HIGH) {   
     Serial.print("Data input from laserbits: ");
     Laserbits = readdetector();
     Serial.println(Laserbits);
     delay(150); // re-read again to check if the value is really ment to be
     Laserbits_again = readdetector();
     if (Laserbits == Laserbits_again) {
         Serial.println("equalbits");
         if (Laserbits == Sequence[Stage]) { 
           Stage= Stage + 1;
           } ;
         if ( Stage == Sequence_SIZE)        { 
          flagscreen()    ;
          Stage=0         ;
          };
     }    ;   
  }
      
  if ((millis() - measuretick) > refresh_measuretick) {
     newtick = true ;
     measuretick = millis();  //update last measurement time
  }

  if ( (newtick == true) && (armed == true) ) {
     // this block is handy for debug only as you can read stuff in the serial port debug stream
     // no need to take it out in real life CTF 
     Serial.print("Tick ... Stage = ");
     Serial.print(Stage);
     Laserbits = readdetector();
     Serial.print(" and Laserbits = ");
     Serial.print(Laserbits);
     Serial.print(" and Sequence_SIZE = ");
     Serial.println(Sequence_SIZE);
     Serial.print(" NEXT number = ");
     Serial.println(Sequence[Stage]);
     detailscreen();
     newtick = false ; //reset the boolean
    }    
}
//**END***MAIN LOOP******************************************************************************

//Read Laser target
//******************************************************
int readdetector() {
  int number = 0 ;
  bit0  = digitalRead(inputBit0);   // read the bit0 
  bit1  = digitalRead(inputBit1);   // read the bit1 
  bit2  = digitalRead(inputBit2);   // read the bit2 
  number = ( (bit0*1) + (bit1*2) + (bit2*4) );
  return number;
}


//*********Webserver Stuff*********
void handleRoot() {
  String message = "Welcome at the root of the challenge\nThe goal is to count binary numbers with laser pointers\n\n";
  message += "The config of the laser targets in front of you is like this:\nCLOCK       Bit2        Bit1      Bit0\n\n\n";
  message += "You need to count in this order:\n";
  for (uint8_t i=0; i<Sequence_SIZE; i++){
    message += " " + String(Sequence[i]);
  }
  message += "\n\n1) Determine the binary numbers first\n2) Light up the Bit2-Bit0 targets with your laser pens for a 1 value\n3) Light CLOCK Target to input your number\n4) Work towards the end of the sequence\n\nGood luck!\n";
  message += "\n****Refresh page for updates****\nNext number :" + String(Sequence[Stage]);
  message += "\nNumbers to go :" + String(Sequence_SIZE-Stage)+ "\n\n";
  server.send(200, "text/plain", message);
}

void handleNotFound(){
  String message = "404 File Not Found.. stop messing around and complete the challenge.. otherwise you waste time on the /flag directory also..\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

//*********Display Stuff*********
void oled_AP(){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("SSID:");
  display.println(ssid);
  display.println("Password:");
  display.println(password);
  display.display();
  }

void detailscreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Stage:");
  display.println(Stage);
  display.println("Laserbits:");
  display.println(Laserbits);
  display.display();
}

void flagscreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("**!FLAG!**");
  display.println("----------");
  display.println(FLAG);
  display.println("----------");
  display.display();
  armed = false;
}
