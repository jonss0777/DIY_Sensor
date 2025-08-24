/*!
* @brief
* @param
*/
void runLM35(){

  unsigned int val;
  unsigned int dat;
  val=analogRead(11);
  dat=(500 * val) /1024;
 
  delay(500);
  sensor_data.temp_sensor_LM35_sensor= String(dat);
  Serial.println("LM35 Sensor");
  Serial.print("Temp:"); 
  Serial.print(sensor_data.temp_sensor_LM35_sensor);
  Serial.println("C");
}