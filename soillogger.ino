#include <stdio.h>
#include <string.h>

//------------------------------------------------------------------------------
#include "RTClib.h"

RTC_PCF8523 rtc;
char dateAndTimeData[20]; // space for YYYY-MM-DDTHH-MM-SS, plus the null char terminator
uint16_t thisYear;
int8_t thisMonth, thisDay, thisHour, thisMinute, thisSecond;

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

void drawProgressBar() {
  if (progressIndicator > 19) {
    progressIndicator = 0;
    lcd.setCursor(0, 0);
    lcd.print("____________________");
  }
  lcd.setCursor(progressIndicator, 0);
  lcd.write(byte(0));
  progressIndicator++;
}

//------------------------------------------------------------------------------

void skullduggery() {

  for (int position = 6; position < 15; position++) {
    lcd.setCursor(position - 1, 3);
    lcd.print(" ");
    lcd.setCursor(position, 3);
    lcd.write(byte(0));
    delay(50);
  }
  for (int position = 14; position > 4; position--) {
    lcd.setCursor(position + 1, 3);
    lcd.print(" ");
    lcd.setCursor(position, 3);
    lcd.write(byte(0));
    delay(50);
  }
}

//------------------------------------------------------------------------------

void getDateAndTime() {

  DateTime now = rtc.now();

  thisYear = now.year();
  thisMonth = now.month();
  thisDay = now.day();
  thisHour = now.hour();
  thisMinute = now.minute();
  thisSecond = now.second();

  sprintf(dateAndTimeData, ("%04d-%02d-%02dT%02d:%02d:%02d"), thisYear, thisMonth, thisDay, thisHour, thisMinute, thisSecond);

}

//------------------------------------------------------------------------------

void printDateAndTime() {
  Serial.print(dateAndTimeData);
  Serial.print(",");
  lcd.setCursor(0,2);
  lcd.print(dateAndTimeData); 

}

//------------------------------------------------------------------------------

char* get_measurement() {
  char* service_request = sdi_serial_connection.sdi_query("?M!10013", 1000);
  // the time  above is to wait for service_request_complete
  char* service_request_complete = sdi_serial_connection.wait_for_response(1000);
  // will return once it gets a response
  return sdi_serial_connection.sdi_query("?D0!", 1000);
}

//------------------------------------------------------------------------------

void processMeasurement(char* str) {
  lcd.setCursor(0, 1);
  char * pch;
  pch = strtok (str, "+-");
  while (pch != NULL) {
    Serial.print(pch);
    lcd.print(pch);
    lcd.print(" ");
    Serial.print(",");
    pch = strtok (NULL, "+-");
  }
  Serial.println();
}

//------------------------------------------------------------------------------

void setup() {
  lcd.begin(20, 4);
  lcd.createChar(0, Skull);
  sdi_serial_connection.begin();
  Serial.begin(9600);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
  if (! rtc.initialized()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  
  lcd.print("OK INITIALIZED");
  Serial.println("OK INITIALIZED");
  delay(3000); // startup delay to allow sensor to powerup and output its DDI serial string
  lcd.clear();
  lcd.print("____________________");

  getDateAndTime();

  printDateAndTime();
}

//------------------------------------------------------------------------------

void loop() {
//  uint8_t wait_for_response_ms = 1000;

  getDateAndTime();
  
  char* response = get_measurement(); // get measurement data

  printDateAndTime();

  processMeasurement(response);

  drawProgressBar();

  skullduggery();
}

//------------------------------------------------------------------------------
