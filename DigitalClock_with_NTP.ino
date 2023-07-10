#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h> 
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char *ssid     = "HUAWEI nova 9";
const char *password = "Cosmin2007";
WiFiUDP ntpUDP;
/*
   for INDIA 5:30
   5X60X60 = 18000
   30x60 = 1800
   1800+18000 = 198000
*/
NTPClient timeClient(ntpUDP, "ro.pool.ntp.org", 10800, 60000);
String voidloop[] = {"v", "o", "i", "d", " ", "l", "o", "o", "p", "(", ")"};
String arr_days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
String date_time;

void setup() {
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {

    for (int i = 0; i <= 10; i++)
    {
      lcd.setCursor(i + 3, 0);
      lcd.print(voidloop[i]);
      delay(250);
      Serial.print ( "." );
    }
  }

  Serial.println("connected... :)");
  timeClient.begin();

  lcd.clear();



}

void loop() {
  timeClient.update();  // time update from NTP server

  Serial.print(timeClient.getFormattedTime());
  Serial.print(" ");


  int hh = timeClient.getHours();
  int mm = timeClient.getMinutes();
  int ss = timeClient.getSeconds();

  lcd.setCursor(1, 0);
  if (hh > 12)
  {
    hh = hh - 12;
    if (hh < 10) {
      lcd.print(0);
      lcd.print(hh);
    }
    else
    {
      lcd.print(hh);
    }
    
    lcd.print(":");
    if (mm < 10) {
      lcd.print(0);
      lcd.print(mm);
    }
    else
    {
      lcd.print(mm);
    }
    lcd.print(":");
    if (ss < 10) {
      lcd.print(0);
      lcd.print(ss);
    }
    else
    {
      lcd.print(ss);
    }
    lcd.print(" PM");
  }
  else
  {
    if (hh < 10) {
      lcd.print(0);
      lcd.print(hh);
    }
    else
    {
      lcd.print(hh);
    }
    
    lcd.print(":");
    if (mm < 10) {
      lcd.print(0);
      lcd.print(mm);
    }
    else
    {
      lcd.print(mm);
    }
    lcd.print(":");
    if (ss < 10) {
      lcd.print(0);
      lcd.print(ss);
    }
    else
    {
      lcd.print(ss);
    }
    lcd.print(" AM");
  }



  int day = timeClient.getDay();
  lcd.print(" ");
  lcd.print(arr_days[day]);  //  it will print day
  lcd.print(" ");

  lcd.setCursor(3, 1);


}
