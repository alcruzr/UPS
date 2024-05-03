float getAmps() {
  // To measure current we need to know the frequency of current
  // By default 50Hz is used, but you can specify own, if necessary
  return sensor.getCurrentAC(60)-ampSensorOffset;
}
