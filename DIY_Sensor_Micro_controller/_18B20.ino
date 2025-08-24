/*!
* @brief
* @param
*/
void run18B20(){ 
  byte data[12];
  byte addr[8];
 
  if ( !ds.search(addr)) {
      //no more sensors on chain, reset search
      ds.reset_search();
      sensor_data.temp_18b20_sensor = String(0.0);
      return ; 
  }
 
  if ( OneWire::crc8( addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      sensor_data.temp_18b20_sensor = String(0.0);
      return ;   
  }
 
  if ( addr[0] != 0x10 && addr[0] != 0x28) {
      Serial.print("Device is not recognized");
      sensor_data.temp_18b20_sensor = String(0.0);
      return ;
  }
 
  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // start conversion, with parasite power on at the end
 
  byte present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE); // Read Scratchpad
 
  for (int i = 0; i < 9; i++) { // we need 9 bytes
    data[i] = ds.read();
  }
  ds.reset_search();
   
  byte MSB = data[1];
  byte LSB = data[0];
 
  float tempRead = ((MSB << 8) | LSB); //using two's compliment
  Serial.println("18B20 Sensor ");
  Serial.print("Temperature: ");
  sensor_data.temp_18b20_sensor = String(tempRead / 16);
  Serial.println(sensor_data.temp_18b20_sensor);
  Serial.println();
}