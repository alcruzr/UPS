#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

#define TFT_DC 9
#define TFT_CS 10
#define TFT_RST 8
// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

float batt_v = 0;//input
float pri_v = 0;//input
float sec_v = 0;//input
float out_v = 0;//set static value for testing

int ac_ok = 110;



bool peak_hours = 0;//used to track if we are in peak hours range. 1 = true
//bool primary = 0;
//bool secondary = 0;
//bool battery = 0;


 
//int pri_led = 5;//output pin. Used for testing purposes to visually see what source is powering output
//int sec_led = 6;//output pin
//int batt_led = 7;//output pin
//int ph1_led = 8;//output pin
//int ph2_led = 9;//output pin

int relay1 = 1;
int relay2 = 2;
int relay4 = 3;
int relay5 = 4;

int batt_v_pin = A0;//input pin, change to analog pin for actuall readings
int out_v_pin = A3;//input pin, change to analog pin for actuall readings
int pri_v_pin = A2;//input pin, change to analog pin for actuall readings
int sec_v_pin = A1;//input pin, change to analog pin for actuall readings
int ph1_pin = 6;//for testing and demo only
int ph2_pin = 7;//for testing and demo only



void setup() {
  Serial.begin(115200);
  analogReference(AR_INTERNAL);  // set ADC positive reference voltage to 1.5V (internal)
  displaySetup();
 
  pinMode(batt_v_pin, INPUT);//for testing purposes use pull down resistors on these four pins
  pinMode(pri_v_pin, INPUT);
  pinMode(sec_v_pin, INPUT);
  pinMode(out_v_pin, INPUT);
  
  pinMode(relay1, OUTPUT);//switch between primary and secondary
  pinMode(relay2, OUTPUT);//controls peak hours/main shut off
  pinMode(relay4, OUTPUT);//controls inverter/main shut off
  pinMode(relay5, OUTPUT);//turns charger ON OFF
  
  
  //For testing
  pinMode(ph1_pin, INPUT);
  pinMode(ph2_pin, INPUT);
//  pinMode(pri_led, OUTPUT);
//  pinMode(sec_led, OUTPUT);
//  pinMode(batt_led, OUTPUT);
//  pinMode(ph1_led, OUTPUT);
//  pinMode(ph2_led, OUTPUT);
  
  //setup wifi, save time to RTC, and turn OFF wifi. only on first starup of UPS.
  //get time from RTC
  
  
  batt_v = get_vtg(batt_v_pin);//read battery voltage, for testing purposes we use digitalRead and input will either be 0(off) or 1(on), for actuall reading we will call the get_vtg() function
  pri_v = get_vtg(pri_v_pin);//read primary voltage, for testing purposes we use digitalRead and input will either be 0(off) or 1(on), for actuall reading we will call the get_vtg() function
  sec_v = get_vtg(sec_v_pin);//read secondary voltage, for testing purposes we use digitalRead and input will either be 0(off) or 1(on, for actuall reading we will call the get_vtg() function
  while(batt_v < 10){//test to see if battery is good or bad, if bad then display error on screen and do not initiate startup. For testing purposes 0 is OFF and 1 is ON.
    Serial.println("Battery bad. Replace Imediately"); 
    digitalWrite(relay2, 0);
    digitalWrite(relay4, 0);
    batt_v = get_vtg(batt_v_pin);
    
  }

//  digitalWrite(pri_led, 0);
//  digitalWrite(sec_led, 0);
//  digitalWrite(batt_led, 0);
//  digitalWrite(ph1_led, 0);
//  digitalWrite(ph2_led, 0);

  digitalWrite(relay1, 0);
  digitalWrite(relay2, 1);
  digitalWrite(relay4, 1);
  digitalWrite(relay5, 0);
  
  Serial.print("Initiating...");
  delay(3000);
}


void loop() {
  
  //Serial.print("loop");
  batt_v = get_vtg(batt_v_pin);
  pri_v = get_vtg(pri_v_pin);
  sec_v = get_vtg(sec_v_pin);

  //for testing only, these three led's will get replaced by the voltages shown on the display
//  digitalWrite(pri_led, pri_v);
//  digitalWrite(sec_led, sec_v);
//  digitalWrite(batt_led, batt_v);
  
  //get output voltage
  //calculate output power, will need to measure output current
  //get current time
  
  //display all info to display
  //tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_GREEN,ILI9341_BLACK);  tft.setTextSize(3);
  tft.print("S1:"); tft.print(pri_v); 
  tft.setCursor(0, 25);
  tft.print("S2:"); tft.print(sec_v); 
  tft.setCursor(0, 50);
  tft.print("Batt:"); tft.print(batt_v); 
  tft.setCursor(0, 100);
  tft.setTextColor(ILI9341_BLUE,ILI9341_BLACK);  tft.setTextSize(3);
  tft.print("Peak Hours Time:"); tft.print("4pm-8pm");

  //in the following if statements, we use simple inputs to simulate we are in peak hours mode. Actual code will compare the current time with the peak hours time range.
  if(pri_v > ac_ok && digitalRead(ph1_pin) == 0 && digitalRead(ph2_pin) == 0){//using primary input. Outside of any peak hours range
    digitalWrite(relay1, 0);
    digitalWrite(relay2, 1);
    digitalWrite(relay5, 0);
    Serial.println("S1.1");
    tft.setCursor(0, 75);
    tft.setTextColor(ILI9341_RED,ILI9341_BLACK);  tft.setTextSize(3);
    tft.print("OUT:"); tft.print(out_v); tft.println("S1.1");
  }

  else if(pri_v < ac_ok && sec_v > ac_ok && digitalRead(ph1_pin) == 0 && digitalRead(ph2_pin) == 0){//using secondary input. Outside of any peak hours range
    digitalWrite(relay1, 1);
    digitalWrite(relay5, 0);
    Serial.println("S2.1");
    tft.setCursor(0, 75);
    tft.setTextColor(ILI9341_RED,ILI9341_BLACK);  tft.setTextSize(3);
    tft.print("OUT:"); tft.print(out_v); tft.println("S2.1");
  }

  else if(pri_v < ac_ok && sec_v < ac_ok && digitalRead(ph1_pin) == 0 && digitalRead(ph2_pin) == 0){//using battery. Outside of any peak hours range
    digitalWrite(relay1, 1);//keep relay connected to primary input
    digitalWrite(relay5, 0);
    Serial.println("Batt1");
    tft.setCursor(0, 75);
    tft.setTextColor(ILI9341_RED,ILI9341_BLACK);  tft.setTextSize(3);
    tft.print("OUT:"); tft.print(out_v); tft.println("Batt1");
  }

  else if(digitalRead(ph1_pin) == 1){//in first half of peak hours. using battery
    digitalWrite(relay2, 0);
    Serial.println("Batt2");
    tft.setCursor(0, 75);
    tft.setTextColor(ILI9341_RED,ILI9341_BLACK);  tft.setTextSize(3);
    tft.print("OUT:"); tft.print(out_v); tft.println("Batt2");
  }

  else if(digitalRead(ph2_pin) == 1){//in second half of peak hours. 
    digitalWrite(relay1, 0);
    digitalWrite(relay2, 1);
    digitalWrite(relay5, 1);//turn off charger
    if(pri_v < ac_ok){
      if(sec_v < ac_ok){
        digitalWrite(relay1, 0);//keep relay 1 connected to primary
        Serial.println("Batt3");
        tft.setCursor(0, 75);
        tft.setTextColor(ILI9341_RED,ILI9341_BLACK);  tft.setTextSize(3);
        tft.print("OUT:"); tft.print(out_v); tft.println("Batt3");
      }
      else{
        digitalWrite(relay1, 1);//switch to secondary
        Serial.println("S2.2");
        tft.setCursor(0, 75);
        tft.setTextColor(ILI9341_RED,ILI9341_BLACK);  tft.setTextSize(3);
        tft.print("OUT:"); tft.print(out_v); tft.println("S2.2");
      }
    }
    else{
      Serial.println("S1.2");
      tft.setCursor(0, 75);
      tft.setTextColor(ILI9341_RED,ILI9341_BLACK);  tft.setTextSize(3);
      tft.print("OUT:"); tft.print(out_v); tft.println("S1.2");
    }
  }


















}
