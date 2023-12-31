#include <LiquidCrystal_I2C.h>
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

char auth[] = "21o8KyuqMF7n69400Ga0mh48M0w8W-88";  //Blynk Auth token
char ssid[] = "HUAWEI nova 9";                     //WIFI SSID
char pass[] = "Cosmin2007";                        //WIFI Password
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, 10800); 
DHT dht(D4, DHT11);  //(DHT sensor pin,sensor type)  D4 DHT11 Temperature Sensor
BlynkTimer timer;
const int x = 65 ;
int sensorState = 0;
//Define component pins
#define soil A0  //A0 Soil Moisture Sensor
#define PIR D5   //D5 PIR Motion Sensor
#define Water D0
int PIR_ToggleValue;

void checkPhysicalButton();
int relay1State = LOW;
int ok = 0;
int pushButton1State = HIGH;
#define RELAY_PIN_2 D6
#define RELAY_PIN_1 D3    //D3 Relay
#define PUSH_BUTTON_1 D7  //D7 Button
#define VPIN_BUTTON_1 V12
#define VPIN_BUTTON_2 V2


//Create three variables for pressure
double T, P;
char status;



void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, pass);
  lcd.clear();
  lcd.begin();
  lcd.backlight();
  pinMode(PIR, INPUT);
  pinMode(Water, INPUT);

  pinMode(RELAY_PIN_1, OUTPUT);
  digitalWrite(RELAY_PIN_1, LOW);
  pinMode(RELAY_PIN_2, OUTPUT);
  digitalWrite(RELAY_PIN_2, LOW);
  pinMode(PUSH_BUTTON_1, INPUT_PULLUP);
  digitalWrite(RELAY_PIN_1, relay1State);
  digitalWrite(RELAY_PIN_2, relay1State);


  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("  Initializing  ");
  for (int a = 5; a <= 10; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(11, 1);
  lcd.print("W:OFF");
  //Call the function
  timer.setInterval(100L, soilMoistureSensor);
  timer.setInterval(100L, DHT11sensor);
  timer.setInterval(500L, checkPhysicalButton);
  timeClient.begin();
}


//Get the DHT11 sensor values
void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(h);
}


//Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(soil);
  value = map(value, 0, 1024, 0, 100);

  Blynk.virtualWrite(V3, value);
  if (ok) {
    if (value < x ) {
      if(timeClient.getHours()>=9 && timeClient.getHours()<=21){
      relay1State = HIGH;
      Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);
      if(sensorState==1)
      digitalWrite(RELAY_PIN_1, relay1State);
      else
      digitalWrite(RELAY_PIN_2, relay1State);}
    } else {
      relay1State = LOW;
      Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);
      if(sensorState==1)
      digitalWrite(RELAY_PIN_1, relay1State);
      else
      digitalWrite(RELAY_PIN_2, relay1State);
    }
  } else {
    if (value < x)
      Blynk.logEvent("sol_uscat", "Alertă! Udați plantele");
  }
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(value);
  lcd.print(" ");
}

//Get the PIR sensor values
void PIRsensor() {
  bool value = digitalRead(PIR);
  if (value) {
    Blynk.logEvent("pir", "Alertă! Posibil animal prin apropiere");  //Enter your Event Name
    WidgetLED LED(V5);
    LED.on();
  } else {
    WidgetLED LED(V5);
    LED.off();
  }
}

BLYNK_WRITE(V6) {
  PIR_ToggleValue = param.asInt();
}


BLYNK_CONNECTED() {
  // Request the latest state from the server
  Blynk.syncVirtual(VPIN_BUTTON_1);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  relay1State = param.asInt();
  if(sensorState==1)
      digitalWrite(RELAY_PIN_1, relay1State);
  else
      digitalWrite(RELAY_PIN_2, relay1State);
}
BLYNK_WRITE(VPIN_BUTTON_2) {
  ok = param.asInt();
}
void checkPhysicalButton() {
  if (digitalRead(PUSH_BUTTON_1) == LOW) {
    // pushButton1State is used to avoid sequential toggles
    if (pushButton1State != LOW) {

      // Toggle Relay state
      relay1State = !relay1State;
      if(sensorState==1)
      digitalWrite(RELAY_PIN_1, relay1State);
      else
      digitalWrite(RELAY_PIN_2, relay1State);

      // Update Button Widget
      Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);
    }
    pushButton1State = LOW;
  } else {
    pushButton1State = HIGH;
  }
}


void loop() {
  timeClient.update();
  sensorState = digitalRead(D0);
  if (PIR_ToggleValue == 1) {
    lcd.setCursor(5, 1);
    lcd.print("M:ON ");
    PIRsensor();
  } else {
    lcd.setCursor(5, 1);
    lcd.print("M:OFF");
    WidgetLED LED(V5);
    LED.off();
  }

  if (relay1State == HIGH) {
    lcd.setCursor(11, 1);
    lcd.print("W:ON ");
  } else if (relay1State == LOW) {
    lcd.setCursor(11, 1);
    lcd.print("W:OFF");
  }
  Blynk.run();  //Run the Blynk library
  timer.run();  //Run the Blynk timer
}
