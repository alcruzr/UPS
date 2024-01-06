float vratio1 = 0.005;//AC vout/vin ratio    .0025
float vratio2 = 0.0715;//DC vout/vin ratio  R1=390k, R2=27k Vin=10V-14.6V

float get_vtg(int source) {//function called to get voltage measurement 
  float final_v = 0;
  int readings = 40;
  if(source == batt_v_pin){//read DC
    float val1 = 0;
    float val2 = 0;
    for (int i = 0; i < readings; i++){
      val2 = analogRead(source);
      val1 = val1 + val2;
      delayMicroseconds(5);
    }
    val1 = val1/readings;
    val1 = val1 * (1.5/1023);
    val1 = val1 / vratio2;
    final_v = val1;
  }
  
  else{//read AC
    float ac_v = 0;
    float val3 = 0;
    float val4 = 0;
    for(uint8_t j = 0; j < readings; j++){
      for(uint8_t i = 0; i < 100; i++) {
        float r = analogRead(source);
        if(ac_v < r){
          ac_v = r;
        }
        //Serial.println("test1");
        delayMicroseconds(5);
        //Serial.println("test2");
      }
      val4 = ac_v;
      val3 = val3 + val4;  
      //Serial.println("test3333333");
    }
    val3 = val3 / readings;
    val3 = val3 * (1.5/1023);
    val3 = val3 / vratio1;
    final_v = val3; 
  }
  return final_v;
}
