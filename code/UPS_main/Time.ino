void timeSetup(){
  rtc.begin();
  delay(2000);
//  if (! rtc.initialized() || rtc.lostPower()) {
//    Serial.println("RTC is NOT initialized, let's set the time!");
//    // When time needs to be set on a new device, or after a power loss, the
//    // following line sets the RTC to the date & time this sketch was compiled
//    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
//    // This line sets the RTC with an explicit date & time, for example to set
//    // January 21, 2014 at 3am you would call:
//    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
//    //
//    // Note: allow 2 seconds after inserting battery or applying external power
//    // without battery before calling adjust(). This gives the PCF8523's
//    // crystal oscillator time to stabilize. If you call adjust() very quickly
//    // after the RTC is powered, lostPower() may still return true.
//  }
  //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  rtc.start();
}
