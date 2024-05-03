#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <ZMPT101B.h>
#include "RTClib.h"
#include "ACS712_muratdemirtas.h"

#define TFT_DC 49
#define TFT_CS 53
#define TFT_RST 48

#define SENSITIVITYpri 138.0f//calibrated 4-10-24
#define SENSITIVITYsec 141.75f//calibrated 4-10-24
#define SENSITIVITYout 147.75f//calibrated 4-10-24

int top = 40;

int _hour;
int _minute;

int circleRadius = 8;


// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

float batt_v = 0;//input
int pri_v = 0;//input
int sec_v = 0;//input
int out_v = 0;//set static value for testing
float out_c = 0;//input
float out_w = 0;

String pri_v_s;
String sec_v_s;

int ac_ok = 105;



bool peak_hours = 0;//used to track if we are in peak hours range. 1 = true
//bool primary = 0;
//bool secondary = 0;
//bool battery = 0;


 
//int pri_led = 5;//output pin. Used for testing purposes to visually see what source is powering output
//int sec_led = 6;//output pin
//int batt_led = 7;//output pin
//int ph1_led = 8;//output pin
//int ph2_led = 9;//output pin

int relay1 = 22;
int relay2 = 24;
int relay4 = 26;
int relay5 = 28;

int batt_v_pin = A4;//input pin, change to analog pin for actuall readings
int out_c_pin = A3;//input pin, change to analog pin for actuall readings
int out_v_pin = A2;//input pin, change to analog pin for actuall readings
int pri_v_pin = A0;//input pin, change to analog pin for actuall readings
int sec_v_pin = A1;//input pin, change to analog pin for actuall readings
int ph1_pin = 30;//for testing and demo only, yellow
int ph2_pin = 32;//for testing and demo only, green

int chargerState = 1;//keep charger off(0) for initial testing in box only

String _time = String("0");
String _time2 = String("am");

int ampSensorCalibrateValue = 510;
float ampSensorOffset = 0.07;

ZMPT101B voltageSensorPri(pri_v_pin, 60.0);
ZMPT101B voltageSensorSec(sec_v_pin, 60.0);
ZMPT101B voltageSensorOut(out_v_pin, 60.0);

ACS712 sensor(ACS712_05B, out_c_pin);

RTC_PCF8523 rtc;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};


void setup() {
  Serial.begin(115200);
  voltageSensorPri.setSensitivity(SENSITIVITYpri);
  voltageSensorSec.setSensitivity(SENSITIVITYsec);
  voltageSensorOut.setSensitivity(SENSITIVITYout);

  sensor.setZeroPoint(ampSensorCalibrateValue);
  
  displaySetup();
  timeSetup();
 
  pinMode(batt_v_pin, INPUT);//for testing purposes use pull down resistors on these four pins
  pinMode(pri_v_pin, INPUT);
  pinMode(sec_v_pin, INPUT);
  pinMode(out_v_pin, INPUT);
  pinMode(out_c_pin, INPUT);
  
  pinMode(relay1, OUTPUT);//switch between primary and secondary
  pinMode(relay2, OUTPUT);//controls peak hours/main shut off
  pinMode(relay4, OUTPUT);//controls inverter/main shut off
  pinMode(relay5, OUTPUT);//turns charger ON OFF
  
  
  //For testing
  pinMode(ph1_pin, INPUT_PULLUP);
  pinMode(ph2_pin, INPUT_PULLUP);
//  pinMode(pri_led, OUTPUT);
//  pinMode(sec_led, OUTPUT);
//  pinMode(batt_led, OUTPUT);
//  pinMode(ph1_led, OUTPUT);
//  pinMode(ph2_led, OUTPUT);
  
  //setup wifi, save time to RTC, and turn OFF wifi. only on first starup of UPS.
  //get time from RTC
  
  
  batt_v = get_vtgDC();//read battery voltage, for testing purposes we use digitalRead and input will either be 0(off) or 1(on), for actuall reading we will call the get_vtg() function
  pri_v = voltageSensorPri.getRmsVoltage();//read primary voltage, for testing purposes we use digitalRead and input will either be 0(off) or 1(on), for actuall reading we will call the get_vtg() function
  sec_v = voltageSensorSec.getRmsVoltage();//read secondary voltage, for testing purposes we use digitalRead and input will either be 0(off) or 1(on, for actuall reading we will call the get_vtg() function
  while(batt_v < 10){//test to see if battery is good or bad, if bad then display error on screen and do not initiate startup. For testing purposes 0 is OFF and 1 is ON.
    Serial.println("Battery bad. Replace Imediately"); 
    tft.setCursor(0, 0);
    tft.setTextColor(ILI9341_GREEN,ILI9341_WHITE);  tft.setTextSize(3);
    tft.print("Battery Below Good Voltage");
    digitalWrite(relay2, 0);
    digitalWrite(relay4, 0);
    batt_v = get_vtgDC();
    
  }

//  digitalWrite(pri_led, 0);
//  digitalWrite(sec_led, 0);
//  digitalWrite(batt_led, 0);
//  digitalWrite(ph1_led, 0);
//  digitalWrite(ph2_led, 0);

  digitalWrite(relay1, 0);
  digitalWrite(relay2, 1);
  digitalWrite(relay4, 1);
  digitalWrite(relay5, chargerState);//keep charger off(0) for initial testing in box only

  Serial.print("Initiating...");
  tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_GREEN,ILI9341_WHITE);  tft.setTextSize(3);
  tft.print("Initiating...");
  delay(3000);
  tft.fillScreen(ILI9341_WHITE);
}


void loop() {
  
  //Serial.print("loop");
  batt_v = get_vtgDC();
  pri_v = voltageSensorPri.getRmsVoltage();
  pri_v_s = String(pri_v);
  sec_v = voltageSensorSec.getRmsVoltage();
  sec_v_s = String(sec_v);
  out_v = voltageSensorOut.getRmsVoltage();
  out_c = getAmps();
  if (pri_v >= ac_ok){
    pri_v_s = "120V";
  }
  else{
    pri_v_s = "OFF ";
  }
  if (sec_v >= ac_ok){
    sec_v_s = "120V";
  }
  else{
    sec_v_s = "OFF ";
  }
  if (out_v >= ac_ok){
    out_v = 120;
  }
  else{
    out_v = 0;
  }

  DateTime now = rtc.now(); 
  //_time = String(now.hour()-12) + String(":") + String(now.minute());
  
  //for testing only, these three led's will get replaced by the voltages shown on the display
//  digitalWrite(pri_led, pri_v);
//  digitalWrite(sec_led, sec_v);
//  digitalWrite(batt_led, batt_v);
  
  //get output voltage
  //calculate output power, will need to measure output current
  //get current time
  //display all info to display
  //tft.fillScreen(ILI9341_WHITE);
  tft.setCursor(20, top);
  tft.setTextColor(ILI9341_BLACK,ILI9341_WHITE);  tft.setTextSize(4);
  tft.print("IN 1: "); tft.print(pri_v_s);
  tft.setCursor(20, top+35);
  tft.print("IN 2: "); tft.print(sec_v_s);
  tft.setCursor(20, top+70);
  tft.print("Batt: "); tft.print(batt_v); tft.print("V"); 
  tft.setTextColor(ILI9341_RED,ILI9341_WHITE);
  tft.setCursor(160, top+145);
  tft.print(int(out_c*out_v)); tft.print("W  ");
  tft.setCursor(0, 5);
  tft.setTextColor(ILI9341_BLUE,ILI9341_WHITE);  tft.setTextSize(3);
  tft.print(" ECO Mode:"); tft.print("4pm-8pm");
  //tft.setCursor(0, 155);
  //tft.print("Current Time: ");  
  tft.setCursor(0, 210); 
  if(now.hour() >= 12){
    if(now.hour() ==12){
      _hour = now.hour();
    }
    else{
      _hour = now.hour() - 12;
    } 
    _time2 = String("pm");
  }
  else{
    _hour = now.hour();
    _time2 = String("am");
  }
  if(now.minute() < 10){
    tft.print(_hour); tft.print(":"); tft.print("0"); tft.print(now.minute()); tft.print(_time2);  
  }
  else{
    tft.print(_hour); tft.print(":"); tft.print(now.minute()); tft.print(_time2);
  }
//  tft.setCursor(200, 200);//????
//  tft.setTextColor(ILI9341_WHITE,ILI9341_WHITE);//????
//  tft.print("PkHr 8");//????
  
  
  //in the following if statements, we use simple inputs to simulate we are in peak hours mode. Actual code will compare the current time with the peak hours time range.
  if(pri_v > ac_ok && digitalRead(ph1_pin) == 1 && digitalRead(ph2_pin) == 1){//using primary input. Outside of any peak hours range
    digitalWrite(relay1, 0);
    digitalWrite(relay2, 1);
    digitalWrite(relay5, chargerState);//
    Serial.println("S1.1");
    delay(100);
    tft.setCursor(20, top+110);
    tft.setTextColor(ILI9341_RED,ILI9341_WHITE);  tft.setTextSize(4);
    tft.print(" OUT: "); tft.print(out_v); tft.print("V"); //tft.println(" IN1.1");
    tft.fillCircle(10, top+15, circleRadius, ILI9341_MAGENTA);//primary
    tft.fillCircle(10, top+35+15, circleRadius, ILI9341_WHITE);
    tft.fillCircle(10, top+70+15, circleRadius, ILI9341_WHITE);
    tft.fillCircle(300, 220, circleRadius, ILI9341_WHITE);//erase PH dot
  }

  else if(pri_v < ac_ok && sec_v > ac_ok && digitalRead(ph1_pin) == 1 && digitalRead(ph2_pin) == 1){//using secondary input. Outside of any peak hours range
    digitalWrite(relay1, 1);
    digitalWrite(relay5, chargerState);
    Serial.println("S2.1");
    delay(100);
    tft.setCursor(20, top+110);
    tft.setTextColor(ILI9341_RED,ILI9341_WHITE);  tft.setTextSize(4);
    tft.print(" OUT: "); tft.print(out_v); tft.print("V"); //tft.println(" IN2.1");
    tft.fillCircle(10, top+15, circleRadius, ILI9341_WHITE);//secondary
    tft.fillCircle(10, top+35+15, circleRadius, ILI9341_MAGENTA);
    tft.fillCircle(10, top+70+15, circleRadius, ILI9341_WHITE);
    tft.fillCircle(300, 220, circleRadius, ILI9341_WHITE);//erase PH dot
  }

  else if(pri_v < ac_ok && sec_v < ac_ok && digitalRead(ph1_pin) == 1 && digitalRead(ph2_pin) == 1){//using battery. Outside of any peak hours range
    digitalWrite(relay1, 1);//keep relay connected to primary input
    digitalWrite(relay5, chargerState);
    Serial.println(" Batt1");
    delay(100);
    tft.setCursor(20, top+110);
    tft.setTextColor(ILI9341_RED,ILI9341_WHITE);  tft.setTextSize(4);
    tft.print(" OUT: "); tft.print(out_v); tft.print("V"); //tft.println(" Batt1");
    tft.fillCircle(10, top+15, circleRadius, ILI9341_WHITE);//battery
    tft.fillCircle(10, top+35+15, circleRadius, ILI9341_WHITE);
    tft.fillCircle(10, top+70+15, circleRadius, ILI9341_MAGENTA);
    tft.fillCircle(300, 220, circleRadius, ILI9341_WHITE);//erase PH dot
  }

  else if(digitalRead(ph1_pin) == 0){//in first half of peak hours. using battery
    digitalWrite(relay2, 0);
    Serial.println(" Batt2");
    delay(100);
    tft.setCursor(20, top+110);
    tft.setTextColor(ILI9341_RED,ILI9341_WHITE);  tft.setTextSize(4);
    tft.print(" OUT: "); tft.print(out_v); tft.print("V"); //tft.println(" Batt2");
    tft.fillCircle(10, top+15, circleRadius, ILI9341_WHITE);//battery
    tft.fillCircle(10, top+35+15, circleRadius, ILI9341_WHITE);
    tft.fillCircle(10, top+70+15, circleRadius, ILI9341_MAGENTA);

//    tft.setCursor(200, 200);
//    tft.print("PkHr 1");
    tft.fillCircle(300, 220, circleRadius, ILI9341_ORANGE);
  }

  else if(digitalRead(ph2_pin) == 0){//in second half of peak hours. 
    digitalWrite(relay1, 0);
    digitalWrite(relay2, 1);
    digitalWrite(relay5, 0);//turn off charger
    if(pri_v < ac_ok){
      if(sec_v < ac_ok){
        digitalWrite(relay1, 0);//keep relay 1 connected to primary
        Serial.println(" Batt3");
        delay(100);
        tft.setCursor(20, top+110);
        tft.setTextColor(ILI9341_RED,ILI9341_WHITE);  tft.setTextSize(4);
        tft.print(" OUT: "); tft.print(out_v); tft.print("V"); //tft.println(" Batt3");
        tft.fillCircle(10, top+15, circleRadius, ILI9341_WHITE);//battery
        tft.fillCircle(10, top+35+15, circleRadius, ILI9341_WHITE);
        tft.fillCircle(10, top+70+15, circleRadius, ILI9341_MAGENTA);
      }
      else{
        digitalWrite(relay1, 1);//switch to secondary
        Serial.println(" S2.2");
        delay(100);
        tft.setCursor(20, top+110);
        tft.setTextColor(ILI9341_RED,ILI9341_WHITE);  tft.setTextSize(4);
        tft.print(" OUT: "); tft.print(out_v); tft.print("V"); //tft.println(" IN2.2");
        tft.fillCircle(10, top+15, circleRadius, ILI9341_WHITE);//secondary
        tft.fillCircle(10, top+35+15, circleRadius, ILI9341_MAGENTA);
        tft.fillCircle(10, top+70+15, circleRadius, ILI9341_WHITE);
      }
    }
    else{
      Serial.println(" S1.2");
      delay(100);
      tft.setCursor(20, top+110);
      tft.setTextColor(ILI9341_RED,ILI9341_WHITE);  tft.setTextSize(4);
      tft.print(" OUT: "); tft.print(out_v); tft.print("V"); //tft.println(" IN1.2");
      tft.fillCircle(10, top+15, circleRadius, ILI9341_MAGENTA);//primary
      tft.fillCircle(10, top+35+15, circleRadius, ILI9341_WHITE);
      tft.fillCircle(10, top+70+15, circleRadius, ILI9341_WHITE);
    }
//    tft.setCursor(200, 200);
//    tft.print("PkHr 2");
    tft.fillCircle(300, 220, circleRadius, ILI9341_ORANGE);
  }
  

















}
