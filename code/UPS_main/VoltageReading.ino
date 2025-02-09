float vratio2 = 0.0640;//DC vout/vin ratio  R1=390k, R2=27k Vin=10V-14.6V, 0.0653

float get_vtgDC() {//function called to get voltage measurement 
  float final_v = 0;
  int readings = 40;
  
  float val1 = 0;
  float val2 = 0;
  for (int i = 0; i < readings; i++){
    val2 = analogRead(batt_v_pin);
    val1 = val1 + val2;
    delayMicroseconds(2);
  }
  val1 = val1/readings;
  val1 = val1 * (5.0/1023);
  val1 = val1 / vratio2;
  final_v = val1;
  
  return final_v;
}
