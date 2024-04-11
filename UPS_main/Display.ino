void displaySetup(){
  pinMode(TFT_CS, OUTPUT);//setup display
  digitalWrite(TFT_CS, HIGH);//set display CS high(OFF) initially
  tft.begin();//initiate display
  tft.setRotation(3);//set screen orientation
  tft.fillScreen(ILI9341_BLACK);
}
