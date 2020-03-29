#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
#include <SDISerial.h>

#define DATALINE_PIN 2  //choose a pin that supports interupts
#define INVERTED 1

SDISerial sdi_serial_connection(DATALINE_PIN, INVERTED);

//------------------------------------------------------------------------------
#include <LiquidCrystal.h>

const int8_t rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int8_t progressIndicator = 0;

byte Skull[] = {
  B00000,
  B01110,
  B10101,
  B11011,
  B01110,
  B01110,
  B00000,
  B00000
};

//------------------------------------------------------------------------------

void drawProgressBar(){
  if (progressIndicator > 19) {
    progressIndicator = 0;
    lcd.setCursor(0,0);
    lcd.print("____________________");
    }
  lcd.setCursor(progressIndicator,0);
  lcd.write(byte(0));
  progressIndicator++;
}

//------------------------------------------------------------------------------

char* get_measurement(){
  char* service_request = sdi_serial_connection.sdi_query("?M!10013",1000);
  // the time  above is to wait for service_request_complete
  char* service_request_complete = sdi_serial_connection.wait_for_response(1000);
  // will return once it gets a response
  return sdi_serial_connection.sdi_query("?D0!",1000);
}

//------------------------------------------------------------------------------

void processString(char* str){
  lcd.setCursor(0,1);
  char * pch;
  pch = strtok (str,"+-");
  while (pch != NULL){
    Serial.print(pch);
    lcd.print(pch);
    lcd.print(" ");
    Serial.print(",");
    pch = strtok (NULL, "+-");
  }
  Serial.println();
}

//------------------------------------------------------------------------------

void setup(){
  lcd.begin(20, 4);
  lcd.createChar(0, Skull);
  sdi_serial_connection.begin(); 
  Serial.begin(9600);
  lcd.print("OK INITIALIZED");
  Serial.println("OK INITIALIZED");
  delay(3000); // startup delay to allow sensor to powerup and output its DDI serial string
  lcd.clear();
  lcd.print("____________________");
}

//------------------------------------------------------------------------------

void loop(){
  uint8_t wait_for_response_ms = 1000;
  char* response = get_measurement(); // get measurement data
  
  processString(response);

  drawProgressBar();
  
  delay(500);
}

//------------------------------------------------------------------------------
